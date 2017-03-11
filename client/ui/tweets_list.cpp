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
#include <ui/tweets_list.hpp>

#include <tweeteria/tweet.hpp>
#include <tweeteria/user.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

#include <algorithm>

TweetsList::TweetsList(QWidget* parent)
    :QScrollArea(parent), m_list(new QWidget(this)), m_layout(QBoxLayout::TopToBottom)
{
    m_list->setLayout(&m_layout);
    setWidget(m_list);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void TweetsList::clearAllTweets()
{
    for(TweetWidget* w : m_elements) {
        delete w;
    }
    m_elements.clear();
}

TweetWidget* TweetsList::addTweetWidget(tweeteria::Tweet const& tweet, tweeteria::User const& author)
{
    auto tweet_widget = new TweetWidget(tweet, author, m_list);
    m_layout.addWidget(tweet_widget);
    m_elements.push_back(tweet_widget);
    return tweet_widget;
}
