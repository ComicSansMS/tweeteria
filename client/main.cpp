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
#include <ui/user_widget.hpp>
#include <ui/tweet_widget.hpp>

#include <metatype_declarations.hpp>
#include <image_provider.hpp>
#include <web_resource_provider.hpp>

#include <tweeteria/tweeteria.hpp>
#include <tweeteria/image_util.hpp>
#include <tweeteria/string_util.hpp>

#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <db/client_database.hpp>

#include <cpprest/http_client.h>

#include <QApplication>
#include <QMessageBox>

#include <fstream>
#include <unordered_map>

void registerMetatypes()
{
    qRegisterMetaType<tweeteria::User>();
    qRegisterMetaType<tweeteria::Tweet>();
}

int main(int argc, char* argv[])
{
    QApplication theApp(argc, argv);

    Ghulbus::Log::initializeLogging();
    auto log_guard = Ghulbus::finally(Ghulbus::Log::shutdownLogging);
    Ghulbus::Log::Handlers::LogToFile file_handler("tweeteria.log");
#if defined WIN32 && defined _DEBUG
    Ghulbus::Log::Handlers::LogMultiSink middle_handler(file_handler, Ghulbus::Log::Handlers::logToWindowsDebugger);
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Trace);
#else
    auto& middle_handler = file_handler;
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Warning);
#endif
    Ghulbus::Log::Handlers::LogAsync top_handler(middle_handler);
    top_handler.start();
    auto top_handler_guard = Ghulbus::finally([&top_handler]() { top_handler.stop(); });
    Ghulbus::Log::setLogHandler(top_handler);

    GHULBUS_LOG(Info, "Tweeteria client up and running.");

    registerMetatypes();

    tweeteria::OAuthCredentials credentials;
    {
        std::ifstream fin("tweeteria.cred", std::ios_base::binary);
        credentials = tweeteria::OAuthCredentials::deserialize(fin);
    }
    tweeteria::Tweeteria tweeteria(credentials);
    auto ft_cred = tweeteria.verifyCredentials();
    auto cred = ft_cred.get();
    if(!cred.is_verified) {
        QMessageBox msgBox;
        msgBox.setText("Could not verify your credentials.");
        msgBox.setInformativeText("The Twitter server rejected your login credentials.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        if(!cred.errors.errors.empty()) {
            auto const error = cred.errors.errors.front();
            msgBox.setDetailedText(QString("The reported error was:\n%1 - %2")
                                    .arg(error.code)
                                    .arg(QString::fromStdString(error.message)));
        }
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return 1;
    }

    auto my_friends_ids = tweeteria.getMyFriendsIds();
    std::vector<tweeteria::UserId> friend_ids;
    while(!my_friends_ids.done())
    {
        auto const new_friends = my_friends_ids.nextPage().get();
        friend_ids.insert(end(friend_ids), begin(new_friends), end(new_friends));
    }

    std::vector<tweeteria::User> users;
    for(std::size_t i=0; i<friend_ids.size();)
    {
        std::size_t const i_end = std::min(i + 100, friend_ids.size());
        std::vector<tweeteria::UserId> query_ids(begin(friend_ids) + i, begin(friend_ids) + i_end);
        i = i_end;
        auto const new_friends = tweeteria.getUsers(query_ids).get();
        users.insert(end(users), begin(new_friends), end(new_friends));
    }

    MainWindow main_window(tweeteria, *cred.user);
    main_window.populateUsers();
    //main_window.getCentralWidget()->populateUsers(users);

    main_window.resize(1230, 800);
    main_window.show();

    return theApp.exec();
}

