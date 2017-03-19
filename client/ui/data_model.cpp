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
#include <ui/data_model.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

DataModel::DataModel(QObject* parent, tweeteria::User const& owner)
    :QObject(parent), m_owner(owner.id)
{
    updateUser(owner);
}

void DataModel::updateUser(tweeteria::User const& user)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_users[user.id] = user;
}

void DataModel::updateTweet(tweeteria::Tweet const& tweet)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_tweets[tweet.id] = tweet;
}

std::vector<tweeteria::UserId> DataModel::updateUserTimeline(tweeteria::UserId user_id, std::vector<tweeteria::Tweet> const& tweets)
{
    std::vector<tweeteria::UserId> missing_authors;
    std::lock_guard<std::mutex> lk(m_mtx);
    GHULBUS_PRECONDITION_MESSAGE(m_users.find(user_id) != end(m_users), "Attempt to update timeline of unknown user.");
    auto it_timeline = m_userTimelines.find(user_id);
    if(it_timeline == end(m_userTimelines)) {
        auto res = m_userTimelines.emplace(user_id, std::vector<tweeteria::TweetId>());
        GHULBUS_ASSERT(res.second);
        it_timeline = res.first;
    }

    for(auto const& t : tweets) {
        m_tweets[t.id] = t;
        it_timeline->second.push_back(t.id);
        if(m_users.find(t.user_id) == end(m_users)) {
            missing_authors.push_back(t.user_id);
        }
        if((t.in_reply_to_user_id != tweeteria::UserId(0)) && (m_users.find(t.in_reply_to_user_id) == end(m_users))) {
            missing_authors.push_back(t.in_reply_to_user_id);
        }
        if((t.retweeted_status) && (m_users.find(t.retweeted_status->user_id) == end(m_users))) {
            missing_authors.push_back(t.retweeted_status->user_id);
        }
        if((t.retweeted_status) &&
           (t.retweeted_status->in_reply_to_user_id != tweeteria::UserId(0)) &&
           (m_users.find(t.retweeted_status->in_reply_to_user_id) == end(m_users))) {
            missing_authors.push_back(t.retweeted_status->in_reply_to_user_id);
        }
    }
    return missing_authors;
}

tweeteria::User DataModel::getOwner() const
{
    auto ret = getUser(m_owner);
    GHULBUS_ASSERT_MESSAGE(ret, "Seems we lost our owner.");
    return *ret;
}

boost::optional<tweeteria::User> DataModel::getUser(tweeteria::UserId user_id) const
{
    std::lock_guard<std::mutex> lk(m_mtx);
    auto it = m_users.find(user_id);
    return it->second;
}

std::vector<tweeteria::Tweet> DataModel::getUserTimeline(tweeteria::UserId user_id) const
{
    std::vector<tweeteria::Tweet> ret;
    auto it = m_userTimelines.find(user_id);
    if(it != end(m_userTimelines)) {
        auto const& tweet_ids = it->second;
        ret.reserve(tweet_ids.size());
        for(auto const& t_id : tweet_ids)
        {
            ret.emplace_back(m_tweets.find(t_id)->second);
        }
    }
    return ret;
}
