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

#include <ui/user_widget.hpp>
#include <ui/tweet_widget.hpp>
#include <ui/tweets_list.hpp>

#include <image_provider.hpp>
#include <web_resource_provider.hpp>

#include <tweeteria/image_util.hpp>
#include <tweeteria/tweeteria.hpp>

CentralWidget::CentralWidget(tweeteria::Tweeteria& tweeteria, QWidget* parent)
    :QWidget(parent), m_tweeteria(&tweeteria),
     m_webResourceProvider(new WebResourceProvider()), m_imageProvider(new ImageProvider(*m_webResourceProvider)),
     m_centralLayout(QBoxLayout::Direction::LeftToRight),
     m_usersList(new QListWidget(this)), m_rightPaneLayout(QBoxLayout::Direction::TopToBottom),
     m_tweetsList(new TweetsList(this)), m_buttonsLayout(QBoxLayout::Direction::LeftToRight),
     m_nextPage(new QPushButton(this)), m_previousPage(new QPushButton(this)), m_selectedUser(nullptr)
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
    connect(this, &CentralWidget::tweetsChanged, this, &CentralWidget::populateTweets, Qt::ConnectionType::QueuedConnection);

    connect(m_nextPage, &QPushButton::clicked, this, &CentralWidget::nextPage);
}

CentralWidget::~CentralWidget()
{
    // needed for unique_ptrs to forward declared providers
}

void CentralWidget::userSelected(QModelIndex const& user_item)
{
    auto const& user = m_users[user_item.row()];
    m_selectedUser = &user;
    m_tweeteria->getUserTimeline(user.id).then([this](std::vector<tweeteria::Tweet> tweets) {
        {
            std::lock_guard<std::mutex> lk(m_mtx);
            m_tweets.swap(tweets);
        }
        emit tweetsChanged();
    });
}

void CentralWidget::nextPage()
{
    tweeteria::TweetId current_max_id(0);
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        if(!m_tweets.empty()) {
            current_max_id = m_tweets.back().id;
        }
    }
    if(m_selectedUser) {
        m_tweeteria->getUserTimeline(m_selectedUser->id, current_max_id).then([this](std::vector<tweeteria::Tweet> tweets) {
            {
                std::lock_guard<std::mutex> lk(m_mtx);
                m_tweets.swap(tweets);
            }
            emit tweetsChanged();
        });
    }
}

void CentralWidget::populateUsers(std::vector<tweeteria::User> const& users)
{
    m_users = users;
    std::vector<UserWidget*> user_widgets;
    std::vector<QListWidgetItem*> list_items;
    for(auto const& u : users) {
        user_widgets.emplace_back(new UserWidget(u, this));
        list_items.emplace_back(new QListWidgetItem(m_usersList));
        list_items.back()->setSizeHint(user_widgets.back()->minimumSizeHint());
        m_usersList->setItemWidget(list_items.back(), user_widgets.back());
        m_userDb[u.id] = u;
    }

    for(std::size_t i=0; i<users.size(); ++i) {
        auto user_widget_i = user_widgets[i];
        auto const img_url = tweeteria::getProfileImageUrlsFromBaseUrl(users[i].profile_image_url_https).original;

        m_imageProvider->retrieve(img_url, [user_widget_i](QPixmap pic) {
            user_widget_i->imageArrived(pic.scaledToHeight(200, Qt::SmoothTransformation));
        });
    }
}

void CentralWidget::populateTweets()
{
    std::lock_guard<std::mutex> lk(m_mtx);

    // populate user db
    std::vector<tweeteria::UserId> missing_authors;
    for(auto const& t : m_tweets) {
        if(m_userDb.find(t.user_id) == end(m_userDb)) {
            missing_authors.push_back(t.user_id);
        }
        if((t.in_reply_to_user_id != tweeteria::UserId(0)) && (m_userDb.find(t.in_reply_to_user_id) == end(m_userDb))) {
            missing_authors.push_back(t.in_reply_to_user_id);
        }
        if((t.retweeted_status) && (m_userDb.find(t.retweeted_status->user_id) == end(m_userDb))) {
            missing_authors.push_back(t.retweeted_status->user_id);
        }
        if((t.retweeted_status) && (t.retweeted_status->in_reply_to_user_id != tweeteria::UserId(0)) && (m_userDb.find(t.retweeted_status->in_reply_to_user_id) == end(m_userDb))) {
            missing_authors.push_back(t.retweeted_status->in_reply_to_user_id);
        }
    }
    std::vector<tweeteria::User> new_authors = m_tweeteria->getUsers(missing_authors).get();
    for(auto const& u : new_authors) {
        m_userDb[u.id] = u;
    }

    m_tweetsList->clearAllTweets();
    // populate widgets
    std::vector<TweetWidget*> tweet_widgets;
    std::vector<QListWidgetItem*> tweet_list_items;
    for(std::size_t i=0; i<m_tweets.size(); ++i)
    {
        tweeteria::Tweet const& tweet = (m_tweets[i].retweeted_status) ? (*m_tweets[i].retweeted_status) : m_tweets[i];
        tweeteria::User const& author = m_userDb[tweet.user_id];
        auto tweet_widget = m_tweetsList->addTweetWidget(tweet, author);

        auto const img_url = tweeteria::getProfileImageUrlsFromBaseUrl(author.profile_image_url_https).normal;
        m_imageProvider->retrieve(img_url, [tweet_widget](QPixmap pic) {
            emit tweet_widget->imageArrived(pic);
        });

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
    }
}
