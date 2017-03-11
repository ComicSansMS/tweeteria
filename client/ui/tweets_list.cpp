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

#include <QScrollBar>

#include <tweeteria/tweet.hpp>
#include <tweeteria/user.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

#include <algorithm>

TweetsList::TweetsList(QWidget* parent)
    :QScrollArea(parent), m_list(new QWidget(this)), m_outerLayout(QBoxLayout::LeftToRight), m_layout(QBoxLayout::TopToBottom)
{
    m_outerLayout.addStretch();
    m_outerLayout.addLayout(&m_layout);
    m_outerLayout.addStretch();
    m_list->setLayout(&m_outerLayout);
    setWidget(m_list);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_outerLayout.setContentsMargins(0, 0, verticalScrollBar()->width(), 0);
}

void TweetsList::clearAllTweets()
{
    for(TweetWidget* w : m_elements) {
        delete w;
    }
    m_elements.clear();
    verticalScrollBar()->setValue(verticalScrollBar()->minimum());
    for(int i = m_layout.count() - 1; i >= 0; --i)
    {
        auto item = m_layout.takeAt(i);
        delete item;
    }
}

TweetWidget* TweetsList::addTweetWidget(tweeteria::Tweet const& tweet, tweeteria::User const& author)
{
    auto tweet_widget = new TweetWidget(tweet, author, m_list);
    if(!m_elements.empty()) { m_layout.addSpacing(10); }
    m_layout.addWidget(tweet_widget);
    m_elements.push_back(tweet_widget);
    return tweet_widget;
}
