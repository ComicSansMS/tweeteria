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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_MAIN_WINDOW_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_MAIN_WINDOW_HPP

#include <qt_begin_disable_warnings.hpp>
#include <QMainWindow>
#include <QVector>
#include <QBoxLayout>
#include <QListWidget>
#include <qt_end_disable_warnings.hpp>

#include <tweeteria/id_types.hpp>

#include <boost/optional.hpp>

#include <memory>

class CentralWidget;
class DataModel;

namespace tweeteria {
class Tweeteria;
struct User;
struct Tweet;

template <typename T>
class MultiPageResult;
}

class ClientDatabase;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    std::shared_ptr<tweeteria::Tweeteria> m_tweeteria;
    DataModel* m_dataModel;
    CentralWidget* m_centralWidget;
    std::unique_ptr<ClientDatabase> m_database;
public:
    MainWindow(std::shared_ptr<tweeteria::Tweeteria> tweeteria, tweeteria::User const& user);

    ~MainWindow();

    void populateUsers();

    CentralWidget* getCentralWidget();

signals:
    void userInfoUpdate(tweeteria::UserId, bool is_friend);
    void userTimelineUpdate(tweeteria::UserId);
    void unreadForUserChanged(tweeteria::UserId, int);

public slots:
    void markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId user_id);
    void onUserSelectionChange(tweeteria::UserId selected_user_id);
    void onAdditionalTimelineTweetsRequest(tweeteria::UserId user, tweeteria::TweetId max_id);
    void onUserTimelineUpdate(tweeteria::UserId user_id);

private:
    void getUserIds_impl(std::shared_ptr<tweeteria::MultiPageResult<std::vector<tweeteria::UserId>>> mpres,
                         std::vector<tweeteria::UserId>&& acc);

    void getUserDetails_impl(std::vector<tweeteria::User> const& new_users, bool is_friend);

    void getUserTimeline_impl(tweeteria::UserId user, std::vector<tweeteria::Tweet> const& tweets);

    void updateLastRead(tweeteria::UserId user_id, boost::optional<tweeteria::TweetId> const& last_read_id);
};

#endif
