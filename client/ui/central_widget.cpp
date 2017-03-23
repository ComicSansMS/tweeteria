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
#include <ui/users_list.hpp>

#include <image_provider.hpp>
#include <web_resource_provider.hpp>

#include <tweeteria/image_util.hpp>
#include <tweeteria/tweeteria.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

CentralWidget::CentralWidget(tweeteria::Tweeteria& tweeteria, DataModel& data_model, QWidget* parent)
    :QWidget(parent), m_tweeteria(&tweeteria), m_dataModel(&data_model),
     m_webResourceProvider(new WebResourceProvider()), m_imageProvider(new ImageProvider(*m_webResourceProvider)),
     m_centralLayout(QBoxLayout::Direction::LeftToRight), m_sortingComboBox(new QComboBox(this)),
     m_usersList(new UsersList(this, *m_dataModel)), m_rightPaneLayout(QBoxLayout::Direction::TopToBottom),
     m_tweetsList(new TweetsList(this, *m_dataModel)), m_buttonsLayout(QBoxLayout::Direction::LeftToRight),
     m_nextPage(new QPushButton(this)), m_selectedUser(tweeteria::UserId(0))
{
    m_usersList->setMinimumWidth(600);
    m_centralLayout.addLayout(&m_leftPaneLayout);
    m_sortingComboBox->insertItem(static_cast<int>(UserSortOrder::DateAdded), "Sort Friends by date added");
    m_sortingComboBox->insertItem(static_cast<int>(UserSortOrder::Alphabetical), "Sort Friends alphabetically");
    m_sortingComboBox->insertItem(static_cast<int>(UserSortOrder::Unread), "Sort Friends by unread tweets");
    m_leftPaneLayout.addWidget(m_sortingComboBox);
    m_leftPaneLayout.addWidget(m_usersList);
    m_usersList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_usersList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_centralLayout.addLayout(&m_rightPaneLayout);
    m_tweetsList->setMinimumWidth(600);
    m_rightPaneLayout.addWidget(m_tweetsList);

    m_rightPaneLayout.addLayout(&m_buttonsLayout);
    m_nextPage->setText("...");
    m_nextPage->setEnabled(false);
    m_buttonsLayout.addWidget(m_nextPage);
    setLayout(&m_centralLayout);

    connect(m_usersList, &UsersList::userSelected, this, &CentralWidget::userSelected);
    connect(m_nextPage, &QPushButton::clicked, this, &CentralWidget::onNextPageClicked);

    connect(m_sortingComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this](int index) { onSortingChanged(static_cast<UserSortOrder>(index)); });
}

CentralWidget::~CentralWidget()
{
    // needed for unique_ptrs to forward declared providers
}

void CentralWidget::userSelected(UserWidget* user_widget)
{
    m_selectedUser = user_widget->getUserId();
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
        auto user_widget = m_usersList->addUserWidget(updated_user);
        m_usersInList.push_back(user_widget);

        auto const img_url = tweeteria::getProfileImageUrlsFromBaseUrl(updated_user.profile_image_url_https).bigger;

        m_imageProvider->retrieve(img_url, [user_widget](QPixmap pic) {
            // @todo user_widget could be dead
            emit user_widget->imageArrived(pic);
        });
    }

}

void CentralWidget::onUserTimelineUpdate(tweeteria::UserId updated_user_id)
{
    if(updated_user_id != m_selectedUser) { return; }

    auto updated_timeline = m_dataModel->getUserTimeline(updated_user_id);

    std::size_t start_index;
    if((updated_timeline.empty()) || ((!m_tweets.empty()) && (updated_timeline.front().id != m_tweets.front().id))) {
        m_tweetsList->clearAllTweets();
        start_index = 0;
        m_tweets = updated_timeline;
    } else {
        start_index = m_tweets.size();
        m_tweets.insert(m_tweets.end(), begin(updated_timeline) + m_tweets.size(), end(updated_timeline));
    }

    // populate widgets
    std::vector<TweetWidget*> tweet_widgets;
    std::vector<QListWidgetItem*> tweet_list_items;
    for(std::size_t i=start_index; i<m_tweets.size(); ++i)
    {
        tweeteria::Tweet const tweet = *m_dataModel->getTweet(m_tweets[i]);
        auto tweet_widget = m_tweetsList->addTweetWidget(tweet);

        m_dataModel->awaitUserInfo(tweet_widget->getDisplayedAuthorId(), [this, tweet_widget](tweeteria::User const& displayed_author) {
            auto const img_url = tweeteria::getProfileImageUrlsFromBaseUrl(displayed_author.profile_image_url_https).normal;
            m_imageProvider->retrieve(img_url, [tweet_widget](QPixmap pic) {
                // @todo tweet_widget could be dead
                emit tweet_widget->imageArrived(pic);
            });
        });

        if(!tweet.entities.media.empty()) {
            if(tweet.entities.media.size() > 1) {
                tweet.entities.media.size();    // todo
            }
            auto const& media = tweet.entities.media.back();
            if(media.type == "photo") {
                m_imageProvider->retrieve(media.media_url_https, [tweet_widget, this](QPixmap pic) {
                    // @todo tweet_widget could be dead
                    emit tweet_widget->mediaArrived(pic);
                });
            } else {
                auto type = tweet.entities.media.back().type;    // todo
            }
        }
        connect(tweet_widget, &TweetWidget::markedAsRead, this, &CentralWidget::markTweetAsRead);
    }

    m_nextPage->setEnabled(true);
}

void CentralWidget::markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId author_id)
{
    emit tweetMarkedAsRead(tweet_id, author_id);
}

void CentralWidget::onUnreadForUserChanged(tweeteria::UserId user_id, int unread_count)
{
    auto user_widget = m_usersList->getUserById(user_id);
    user_widget->onUnreadUpdated(unread_count);
}

void CentralWidget::onSortingChanged(UserSortOrder sorting)
{
    m_usersList->sortElements(sorting);
}
