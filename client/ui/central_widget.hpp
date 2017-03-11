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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_CENTRAL_WIDGET_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_CENTRAL_WIDGET_HPP

#include <QWidget>

#include <QBoxLayout>
#include <QListWidget>
#include <QPushButton>

#include <tweeteria/user.hpp>
#include <tweeteria/tweet.hpp>

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace tweeteria {
class Tweeteria;
}

class WebResourceProvider;
class ImageProvider;
class TweetsList;

class CentralWidget : public QWidget
{
    Q_OBJECT
private:
    tweeteria::Tweeteria* m_tweeteria;
    std::unique_ptr<WebResourceProvider> m_webResourceProvider;
    std::unique_ptr<ImageProvider> m_imageProvider;

    QBoxLayout m_centralLayout;

    QListWidget* m_usersList;

    QBoxLayout m_rightPaneLayout;
    //QListWidget* m_tweetsList;
    TweetsList* m_tweetsList;

    QBoxLayout m_buttonsLayout;
    QPushButton* m_nextPage;
    QPushButton* m_previousPage;

    std::vector<tweeteria::User> m_users;
    std::vector<tweeteria::Tweet> m_tweets;
    std::mutex m_mtx;

    std::unordered_map<tweeteria::UserId, tweeteria::User> m_userDb;

    tweeteria::User const* m_selectedUser;
public:
    CentralWidget(tweeteria::Tweeteria& tweeteria, QWidget* parent);
    ~CentralWidget();

signals:
    void tweetsChanged();

public slots:
    void userSelected(QModelIndex const& user_item);
    void populateUsers(std::vector<tweeteria::User> const& users);
    void populateTweets();
    void nextPage();
private slots:
    void markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId author_id);
};

#endif
