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
#include <db/client_database.hpp>
#include <metatype_declarations.hpp>

#include <QFileInfo>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

MainWindow::MainWindow(tweeteria::Tweeteria& tweeteria, tweeteria::User const& user)
    :QMainWindow(), m_tweeteria(&tweeteria), m_centralWidget(new CentralWidget(tweeteria, user, this)), m_database(nullptr)
{
    setWindowTitle("Tweeteria");
    setCentralWidget(m_centralWidget);
    setStyleSheet("QMainWindow { background-color: white }");

    std::string database_filename = user.screen_name + ".db";
    QFileInfo check_file(QString::fromStdString(database_filename));
    if(check_file.exists()) {
        m_database.reset(new ClientDatabase(database_filename));
    } else {
        m_database.reset(new ClientDatabase(ClientDatabase::createNewDatabase(database_filename)));
    }

    connect(m_centralWidget, &CentralWidget::tweetMarkedAsRead, this, &MainWindow::markTweetAsRead);
    connect(this, &MainWindow::userInfoUpdate, m_centralWidget, &CentralWidget::onUserInfoUpdate, Qt::QueuedConnection);
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
            m_tweeteria->getUsers(acc).then([this](std::vector<tweeteria::User> const& new_users) { getUserDetails_impl(new_users); });
            acc.assign(begin(new_ids) + left_to_full, end(new_ids));
        } else {
            acc.insert(end(acc), begin(new_ids), end(new_ids));
        }
        if(mpres->done()) {
            if(!acc.empty()) {
                m_tweeteria->getUsers(acc).then([this](std::vector<tweeteria::User> const& new_users) { getUserDetails_impl(new_users); });
            }
        } else {
            getUserIds_impl(std::move(mpres), std::move(acc));
        }
    });
}

void MainWindow::getUserDetails_impl(std::vector<tweeteria::User> const& new_users)
{
    for(auto const& u : new_users) {
        m_tweeteria->getUserTimeline(u.id).then([this, u_id = u.id](std::vector<tweeteria::Tweet> const& tweets) {
            GHULBUS_LOG(Trace, "New tweets for user " << u_id.id);
            emit newTweets(QVector<tweeteria::Tweet>::fromStdVector(tweets));
        });
        GHULBUS_LOG(Trace, "Update info for user " << u.id.id);
        emit userInfoUpdate(u);
    }
}

CentralWidget* MainWindow::getCentralWidget()
{
    return m_centralWidget;
}

void MainWindow::markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId user_id)
{
    m_database->updateUserRead(user_id, tweet_id);
}
