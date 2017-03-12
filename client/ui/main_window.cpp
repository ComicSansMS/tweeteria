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

#include <QFileInfo>

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
}

MainWindow::~MainWindow()
{
    // for forward-declared database unique_ptr
}

CentralWidget* MainWindow::getCentralWidget()
{
    return m_centralWidget;
}

void MainWindow::markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId user_id)
{
    m_database->updateUserRead(user_id, tweet_id);
}
