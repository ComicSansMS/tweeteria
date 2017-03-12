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

#include <QMainWindow>

#include <QBoxLayout>
#include <QListWidget>

#include <tweeteria/id_types.hpp>

#include <memory>

class CentralWidget;

namespace tweeteria {
class Tweeteria;
struct User;
}

class ClientDatabase;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    tweeteria::Tweeteria* m_tweeteria;
    CentralWidget* m_centralWidget;
    std::unique_ptr<ClientDatabase> m_database;
public:
    MainWindow(tweeteria::Tweeteria& tweeteria, tweeteria::User const& user);

    ~MainWindow();

    CentralWidget* getCentralWidget();

public slots:
    void markTweetAsRead(tweeteria::TweetId tweet_id, tweeteria::UserId user_id);
};

#endif
