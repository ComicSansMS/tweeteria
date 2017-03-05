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
#include <unordered_map>

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

    std::vector<tweeteria::User> users;
    for(std::size_t i=0; i<friend_ids.size();)
    {
        std::size_t const i_end = std::min(i + 100, friend_ids.size());
        std::vector<tweeteria::UserId> query_ids(begin(friend_ids) + i, begin(friend_ids) + i_end);
        i = i_end;
        auto const new_friends = tweeteria.getUsers(query_ids).get();
        users.insert(end(users), begin(new_friends), end(new_friends));
    }

    std::vector<tweeteria::Tweet> tweets = tweeteria.getUserTimeline(users[59].id).get();

    WebResourceProvider wrp;
    ImageProvider image_provider(wrp);

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

    auto tweet_list = new QListWidget(parent);
    parent_layout->addWidget(tweet_list);
    tweet_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tweet_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    std::unordered_map<tweeteria::UserId, tweeteria::User> user_db;
    for(auto const& u : users) {
        user_db[u.id] = u;
    }
    std::vector<tweeteria::UserId> missing_authors;
    for(auto const& t : tweets) {
        if(user_db.find(t.user_id) == end(user_db)) {
            missing_authors.push_back(t.user_id);
        }
        if((t.in_reply_to_user_id != tweeteria::UserId(0)) && (user_db.find(t.in_reply_to_user_id) == end(user_db))) {
            missing_authors.push_back(t.in_reply_to_user_id);
        }
        if((t.retweeted_status) && (user_db.find(t.retweeted_status->user_id) == end(user_db))) {
            missing_authors.push_back(t.retweeted_status->user_id);
        }
        if((t.retweeted_status) && (t.retweeted_status->in_reply_to_user_id != tweeteria::UserId(0)) && (user_db.find(t.retweeted_status->in_reply_to_user_id) == end(user_db))) {
            missing_authors.push_back(t.retweeted_status->in_reply_to_user_id);
        }
    }
    std::vector<tweeteria::User> new_authors = tweeteria.getUsers(missing_authors).get();
    for(auto const& u : new_authors) {
        user_db[u.id] = u;
    }

    std::vector<TweetWidget*> tweet_widgets;
    std::vector<QListWidgetItem*> tweet_list_items;
    for(std::size_t i=0; i<tweets.size(); ++i)
    {
        tweeteria::Tweet const& tweet = (tweets[i].retweeted_status) ? (*tweets[i].retweeted_status) : tweets[i];
        tweeteria::User const& author = user_db[tweet.user_id];
        tweet_widgets.emplace_back(new TweetWidget(tweet, author, parent));
        tweet_list_items.emplace_back(new QListWidgetItem(tweet_list));
        tweet_list_items.back()->setSizeHint(tweet_widgets.back()->minimumSizeHint());
        tweet_list->setItemWidget(tweet_list_items.back(), tweet_widgets.back());


        TweetWidget* tweet_widget = tweet_widgets.back();
        auto const img_url = web::http::uri(
            tweeteria::convertUtf8ToUtf16(tweeteria::getProfileImageUrlsFromBaseUrl(author.profile_image_url_https).normal));
        image_provider.retrieve(tweeteria::convertUtf16ToUtf8(img_url.to_string()), [tweet_widget](QPixmap pic) {
            tweet_widget->imageArrived(pic);
        });
    }

    main_window.setWindowTitle("Tweeteria");
    main_window.setCentralWidget(parent);
    main_window.setStyleSheet("QMainWindow { background-color: white }");
    main_window.resize(1230, 800);
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

