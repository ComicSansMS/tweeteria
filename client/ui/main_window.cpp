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
#include <ui/main_window.hpp>

#include <ui/central_widget.hpp>
#include <ui/data_model.hpp>
#include <db/client_database.hpp>
#include <metatype_declarations.hpp>

#include <qt_begin_disable_warnings.hpp>
#include <QFileInfo>
#include <qt_end_disable_warnings.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

MainWindow::MainWindow(std::shared_ptr<tweeteria::Tweeteria> tweeteria, tweeteria::User const& user)
    :QMainWindow(), m_tweeteria(std::move(tweeteria)), m_dataModel(new DataModel(this, user)), 
     m_centralWidget(new CentralWidget(*m_tweeteria, *m_dataModel, this)), m_database(nullptr)
{
    setWindowTitle("Tweeteria");
    resize(1230, 800);
    setCentralWidget(m_centralWidget);
    setStyleSheet("QMainWindow { background-color: white }");

    std::string database_filename = user.screen_name + ".db";
    QFileInfo check_file(QString::fromStdString(database_filename));
    if(check_file.exists()) {
        m_database.reset(new ClientDatabase(database_filename));
    } else {
        m_database.reset(new ClientDatabase(ClientDatabase::createNewDatabase(database_filename)));
    }

    connect(this, &MainWindow::userInfoUpdate, m_centralWidget, &CentralWidget::onUserInfoUpdate, Qt::QueuedConnection);
    connect(this, &MainWindow::userTimelineUpdate, m_centralWidget, &CentralWidget::onUserTimelineUpdate, Qt::QueuedConnection);
    connect(this, &MainWindow::unreadForUserChanged, m_centralWidget, &CentralWidget::onUnreadForUserChanged);
    connect(this, &MainWindow::userTimelineUpdate, this, &MainWindow::onUserTimelineUpdate, Qt::QueuedConnection);
    connect(m_centralWidget, &CentralWidget::tweetMarkedAsRead, this, &MainWindow::markTweetAsRead);
    connect(m_centralWidget, &CentralWidget::userSelectionChanged, this, &MainWindow::onUserSelectionChange);
    connect(m_centralWidget, &CentralWidget::additionalTimelineTweetsRequest, this, &MainWindow::onAdditionalTimelineTweetsRequest);
}

MainWindow::~MainWindow()
{
    // for forward-declared database unique_ptr
}

void MainWindow::populateUsers()
{
    std::vector<tweeteria::UserId> acc;
    acc.reserve(100);
    getUserIds_impl(std::make_shared<tweeteria::MultiPageResult<std::vector<tweeteria::UserId>>>(m_tweeteria->getMyFriendsIds()), std::move(acc));
}

void MainWindow::getUserIds_impl(std::shared_ptr<tweeteria::MultiPageResult<std::vector<tweeteria::UserId>>> mpres,
                                 std::vector<tweeteria::UserId>&& acc)
{
    GHULBUS_PRECONDITION(!mpres->done());
    mpres->nextPage().then([this, mpres, acc = std::move(acc)](std::vector<tweeteria::UserId> const& new_ids) mutable {
        std::size_t left_to_full = 100 - acc.size();
        if(new_ids.size() > left_to_full) {
            acc.insert(end(acc), begin(new_ids), begin(new_ids) + left_to_full);
            m_tweeteria->getUsers(acc).then([this](std::vector<tweeteria::User> const& new_users) { getUserDetails_impl(new_users, true); });
            acc.assign(begin(new_ids) + left_to_full, end(new_ids));
        } else {
            acc.insert(end(acc), begin(new_ids), end(new_ids));
        }
        if(mpres->done()) {
            if(!acc.empty()) {
                m_tweeteria->getUsers(acc).then([this](std::vector<tweeteria::User> const& new_users) { getUserDetails_impl(new_users, true); });
            }
        } else {
            getUserIds_impl(std::move(mpres), std::move(acc));
        }
    });
}

void MainWindow::getUserDetails_impl(std::vector<tweeteria::User> const& new_users, bool is_friend)
{
    for(auto const& u : new_users) {
        if(is_friend) {
            m_tweeteria->getUserTimeline(u.id).then([this, u_id = u.id](std::vector<tweeteria::Tweet> const& tweets) {
                getUserTimeline_impl(u_id, tweets);
            });
        }
        GHULBUS_LOG(Trace, "Update info for user " << u.id.id);
        m_dataModel->updateUser(u);
        emit userInfoUpdate(u.id, is_friend);
    }
}

CentralWidget* MainWindow::getCentralWidget()
{
    return m_centralWidget;
}

void MainWindow::markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId user_id)
{
    auto const new_last_read = m_database->updateUserRead(user_id, tweet_id);
    if(new_last_read == tweet_id) {
        updateLastRead(user_id, tweet_id);
    }
}

void MainWindow::onUserSelectionChange(tweeteria::UserId selected_user_id)
{
    auto const selected_user = m_dataModel->getUser(selected_user_id);
    GHULBUS_LOG(Trace, "User @" << selected_user->screen_name << " selected in UI.");
    emit userTimelineUpdate(selected_user_id);
}

void MainWindow::onAdditionalTimelineTweetsRequest(tweeteria::UserId user, tweeteria::TweetId max_id)
{
    m_tweeteria->getUserTimeline(user, max_id).then([this, u_id = user](std::vector<tweeteria::Tweet> const& tweets) {
        getUserTimeline_impl(u_id, tweets);
    });
}

void MainWindow::getUserTimeline_impl(tweeteria::UserId user, std::vector<tweeteria::Tweet> const& tweets)
{
    GHULBUS_LOG(Trace, tweets.size() << " new tweets for user " << user.id << ".");
    auto const missing_authors = m_dataModel->updateUserTimeline(user, tweets);
    emit userTimelineUpdate(user);
    m_tweeteria->getUsers(missing_authors).then([this](std::vector<tweeteria::User> const& missing_author_details) {
        getUserDetails_impl(missing_author_details, false);
    });
}

void MainWindow::onUserTimelineUpdate(tweeteria::UserId user_id)
{
    auto const last_read_id = m_database->getLastReadForUser(user_id);
    updateLastRead(user_id, last_read_id);
}

void MainWindow::updateLastRead(tweeteria::UserId user_id, boost::optional<tweeteria::TweetId> const& last_read_id)
{
    int unread_count = -1;
    auto timeline = m_dataModel->getUserTimeline(user_id);
    if(timeline.empty()) {
        unread_count = 0;
    } else if(last_read_id) {
        int count = 0;
        for(auto const& t : timeline) {
            if(t <= *last_read_id) { break; }
            ++count;
        }
        unread_count = (count < timeline.size()) ? count : 50;
    }
    GHULBUS_LOG(Trace, "New unread count for user " << user_id.id << " is " << unread_count << ".");
    emit unreadForUserChanged(user_id, unread_count);
}
