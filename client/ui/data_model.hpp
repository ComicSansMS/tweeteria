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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_DATA_MODEL_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_DATA_MODEL_HPP

#include <QObject>

#include <tweeteria/id_types.hpp>
#include <tweeteria/user.hpp>
#include <tweeteria/tweet.hpp>

#include <boost/optional.hpp>

#include <mutex>
#include <unordered_map>

namespace tweeteria {
class Tweeteria;
struct User;
struct Tweet;

template <typename T>
class MultiPageResult;
}

class ClientDatabase;

class DataModel : public QObject
{
    Q_OBJECT
private:
    mutable std::mutex m_mtx;
    std::unordered_map<tweeteria::UserId, tweeteria::User> m_users;
    std::unordered_map<tweeteria::UserId, std::vector<tweeteria::TweetId>> m_userTimelines;
    std::unordered_map<tweeteria::TweetId, tweeteria::Tweet> m_tweets;

    tweeteria::UserId const m_owner;
public:
    DataModel(QObject* parent, tweeteria::User const& owner);

    DataModel(DataModel const&) = delete;
    DataModel& operator=(DataModel const&) = delete;

    void updateUser(tweeteria::User const& user);

    void updateTweet(tweeteria::Tweet const& tweet);

    std::vector<tweeteria::UserId> updateUserTimeline(tweeteria::UserId user_id, std::vector<tweeteria::Tweet> const& new_tweets);

    tweeteria::User getOwner() const;
    boost::optional<tweeteria::User> getUser(tweeteria::UserId user_id) const;
    std::vector<tweeteria::TweetId> getUserTimeline(tweeteria::UserId user_id) const;
    boost::optional<tweeteria::Tweet> getTweet(tweeteria::TweetId tweet_id) const;

signals:

public slots:
    
};

#endif
