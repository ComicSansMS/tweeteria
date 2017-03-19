/*
 * Tweeteria - A minimalistic tweet reader.
 * Copyright (C) 2017  Andreas Weis (der_ghulbus@ghulbus-inc.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ui/central_widget.hpp>

#include <ui/data_model.hpp>
#include <ui/user_widget.hpp>
#include <ui/tweet_widget.hpp>
#include <ui/tweets_list.hpp>

#include <image_provider.hpp>
#include <web_resource_provider.hpp>

#include <tweeteria/image_util.hpp>
#include <tweeteria/tweeteria.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

CentralWidget::CentralWidget(tweeteria::Tweeteria& tweeteria, DataModel& data_model, QWidget* parent)
    :QWidget(parent), m_tweeteria(&tweeteria), m_dataModel(&data_model),
     m_webResourceProvider(new WebResourceProvider()), m_imageProvider(new ImageProvider(*m_webResourceProvider)),
     m_centralLayout(QBoxLayout::Direction::LeftToRight),
     m_usersList(new QListWidget(this)), m_rightPaneLayout(QBoxLayout::Direction::TopToBottom),
     m_tweetsList(new TweetsList(this)), m_buttonsLayout(QBoxLayout::Direction::LeftToRight),
     m_nextPage(new QPushButton(this)), m_previousPage(new QPushButton(this)), m_selectedUser(tweeteria::UserId(0))
{
    m_usersList->setMinimumWidth(600);
    m_centralLayout.addWidget(m_usersList);
    m_usersList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_usersList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_usersList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_centralLayout.addLayout(&m_rightPaneLayout);
    m_tweetsList->setMinimumWidth(600);
    m_rightPaneLayout.addWidget(m_tweetsList);

    m_rightPaneLayout.addLayout(&m_buttonsLayout);
    m_previousPage->setText("<<");
    m_buttonsLayout.addWidget(m_previousPage);
    m_nextPage->setText(">>");
    m_buttonsLayout.addWidget(m_nextPage);
    setLayout(&m_centralLayout);

    connect(m_usersList, &QListWidget::clicked, this, &CentralWidget::userSelected);
    connect(m_nextPage, &QPushButton::clicked, this, &CentralWidget::onNextPageClicked);
}

CentralWidget::~CentralWidget()
{
    // needed for unique_ptrs to forward declared providers
}

void CentralWidget::userSelected(QModelIndex const& user_item)
{
    auto const& user = m_usersInList[user_item.row()];
    m_selectedUser = user;
    emit userSelectionChanged(m_selectedUser);
}

void CentralWidget::onNextPageClicked()
{
    tweeteria::TweetId current_max_id = (!m_tweets.empty()) ? m_tweets.back() : tweeteria::TweetId(0);

    if(m_selectedUser != tweeteria::UserId(0)) {
        emit additionalTimelineTweetsRequest(m_selectedUser, current_max_id);
    }
}

void CentralWidget::onUserInfoUpdate(tweeteria::UserId updated_user_id, bool is_friend)
{
    auto const updated_user = *m_dataModel->getUser(updated_user_id);

    if(is_friend) {
        auto user_widget = new UserWidget(updated_user, this);
        auto list_item = new QListWidgetItem(m_usersList);
        list_item->setSizeHint(user_widget->minimumSizeHint());
        m_usersList->setItemWidget(list_item, user_widget);
        m_usersInList.push_back(updated_user.id);

        auto const img_url = tweeteria::getProfileImageUrlsFromBaseUrl(updated_user.profile_image_url_https).bigger;

        m_imageProvider->retrieve(img_url, [user_widget](QPixmap pic) {
            emit user_widget->imageArrived(pic);
        });
    }

}

void CentralWidget::onUserTimelineUpdate(tweeteria::UserId updated_user_id)
{
    if(updated_user_id != m_selectedUser) { return; }
    m_tweetsList->clearAllTweets();

    m_tweets = m_dataModel->getUserTimeline(updated_user_id);

    // populate widgets
    std::vector<TweetWidget*> tweet_widgets;
    std::vector<QListWidgetItem*> tweet_list_items;
    for(std::size_t i=0; i<m_tweets.size(); ++i)
    {
        tweeteria::Tweet const tweet = *m_dataModel->getTweet(m_tweets[i]);
        tweeteria::Tweet const& displayed_tweet = (tweet.retweeted_status) ? (*tweet.retweeted_status) : tweet;
        tweeteria::User const author = *m_dataModel->getUser(tweet.user_id);
        auto const opt_displayed_author = m_dataModel->getUser(displayed_tweet.user_id);
        tweeteria::User displayed_author;
        if(opt_displayed_author) {
            displayed_author = *opt_displayed_author;
        } else {
            displayed_author.id = tweeteria::UserId(1);
            displayed_author.name = "Unknown Author";
            displayed_author.screen_name = "unknown";
        }
        auto tweet_widget = m_tweetsList->addTweetWidget(tweet, author, displayed_author);
        

        if(!displayed_author.profile_image_url_https.empty()) {
            auto const img_url = tweeteria::getProfileImageUrlsFromBaseUrl(displayed_author.profile_image_url_https).normal;
            m_imageProvider->retrieve(img_url, [tweet_widget](QPixmap pic) {
                emit tweet_widget->imageArrived(pic);
            });
        }

        if(!tweet.entities.media.empty()) {
            if(tweet.entities.media.size() > 1) {
                tweet.entities.media.size();    // todo
            }
            auto const& media = tweet.entities.media.back();
            if(media.type == "photo") {
                m_imageProvider->retrieve(media.media_url_https, [tweet_widget, this](QPixmap pic) {
                    emit tweet_widget->mediaArrived(pic);
                });
            } else {
                auto type = tweet.entities.media.back().type;    // todo
            }
        }
        connect(tweet_widget, &TweetWidget::markedAsRead, this, &CentralWidget::markTweetAsRead);
    }
}

void CentralWidget::markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId author_id)
{
    emit tweetMarkedAsRead(tweet_id, author_id);
}
