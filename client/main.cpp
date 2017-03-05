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
#include <ui/user_widget.hpp>
#include <ui/tweet_widget.hpp>

#include <image_provider.hpp>
#include <web_resource_provider.hpp>

#include <tweeteria/tweeteria.hpp>
#include <tweeteria/image_util.hpp>
#include <tweeteria/string_util.hpp>

#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <cpprest/http_client.h>

#include <QApplication>

#include <fstream>

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

    tweeteria::OAuthCredentials credentials;
    {
        std::ifstream fin("tweeteria.cred", std::ios_base::binary);
        credentials = tweeteria::OAuthCredentials::deserialize(fin);
    }
    tweeteria::Tweeteria tweeteria(credentials);
    auto ft_cred = tweeteria.verifyCredentials();
    auto cred = ft_cred.get();

    auto my_friends_ids = tweeteria.getMyFriendsIds();
    std::vector<tweeteria::UserId> friend_ids;
    while(!my_friends_ids.done())
    {
        auto const new_friends = my_friends_ids.nextPage().get();
        friend_ids.insert(end(friend_ids), begin(new_friends), end(new_friends));
    }

    auto users1 = tweeteria.getUsers(friend_ids).get();

    std::vector<tweeteria::User> users;
    auto friends_pending = tweeteria.getMyFriendsList();
    while(!friends_pending.done())
    {
        auto const new_friends = friends_pending.nextPage().get();
        users.insert(end(users), begin(new_friends), end(new_friends));
    }

    WebResourceProvider wrp;
    ImageProvider image_provider(wrp);

    auto tweets = tweeteria::json_test_tweets();

    MainWindow main_window;

    QWidget* parent = new QWidget(&main_window);
    QBoxLayout* parent_layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, parent);

    auto list = new QListWidget(parent);
    list->setMinimumWidth(600);
    parent_layout->addWidget(list);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    std::vector<UserWidget*> user_widgets;
    std::vector<QListWidgetItem*> list_items;
    for(std::size_t i=0; i<users.size(); ++i) {
        user_widgets.emplace_back(new UserWidget(users[i], parent));
        list_items.emplace_back(new QListWidgetItem(list));
        list_items.back()->setSizeHint(user_widgets.back()->minimumSizeHint());
        list->setItemWidget(list_items.back(), user_widgets.back());
    }

    auto tweet = new TweetWidget(tweets[0], parent);
    parent_layout->addWidget(tweet);

    main_window.setWindowTitle("Tweeteria");
    main_window.setCentralWidget(parent);
    main_window.setStyleSheet("QMainWindow { background-color: white }");
    main_window.show();

    for(std::size_t i=0; i<users.size(); ++i) {
        auto user_widget_i = user_widgets[i];
        auto const img_url = web::http::uri(
            tweeteria::convertUtf8ToUtf16(
                tweeteria::getProfileImageUrlsFromBaseUrl(users[i].profile_image_url_https).original));

        image_provider.retrieve(tweeteria::convertUtf16ToUtf8(img_url.to_string()), [user_widget_i](QPixmap pic) {
            user_widget_i->imageArrived(pic.scaledToHeight(200, Qt::SmoothTransformation));
        });
    }

    return theApp.exec();
}

