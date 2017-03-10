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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_TWEETS_LIST_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_TWEETS_LIST_HPP

#include <ui/tweet_widget.hpp>

#include <QBoxLayout>
#include <QScrollArea>
#include <QWidget>

#include <vector>

namespace tweeteria {
struct Tweet;
struct User;
}

class TweetsList : public QScrollArea
{
    Q_OBJECT
private:
    QWidget* m_list;
    QBoxLayout m_layout;
    std::vector<TweetWidget*> m_elements;
public:
    TweetsList(QWidget* parent);

    void clearAllTweets();
    TweetWidget* addTweetWidget(tweeteria::Tweet const& tweet, tweeteria::User const& author);
};

#endif
