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
    std::vector<UserInfoCallback> outstanding_cbs;
    {
        std::lock_guard<std::mutex> lk(m_mtx);
        m_users[user.id] = user;
        auto awaiters = m_userInfoCallbacks.find(user.id);
        if(awaiters != end(m_userInfoCallbacks)) {
            outstanding_cbs = std::move(awaiters->second);
            m_userInfoCallbacks.erase(awaiters);
        }
    }
    for(auto const& cb : outstanding_cbs) {
        cb(user);
    }
}

void DataModel::updateTweet(tweeteria::Tweet const& tweet)
{
    std::lock_guard<std::mutex> lk(m_mtx);
    m_tweets[tweet.id] = tweet;
}

std::vector<tweeteria::UserId> DataModel::updateUserTimeline(tweeteria::UserId user_id, std::vector<tweeteria::Tweet> const& new_tweets)
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

    // merge timelines
    std::vector<tweeteria::TweetId>& existing = it_timeline->second;
    auto it_existing = begin(existing);
    auto it_new = begin(new_tweets);

    auto insert_tweet = [this, &it_existing, &existing, &missing_authors](tweeteria::Tweet const& t) {
        it_existing = existing.insert(it_existing, t.id);
        ++it_existing;
        m_tweets[t.id] = t;
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
    };

    for(;;)
    {
        if(it_new == end(new_tweets)) { break; }
        if(it_existing == end(existing)) {
            existing.reserve(existing.size() + (end(new_tweets) - it_new));
            it_existing = end(existing);    // iterators invalidated by reserve
            for(; it_new != end(new_tweets); ++it_new) {
                insert_tweet(*it_new);
            }
            break;
        }
        if(it_new->id >= *it_existing) {
            if(it_new->id != *it_existing) {
                insert_tweet(*it_new);
            }
            ++it_new;
        } else {
            ++it_existing;
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
    if(it == end(m_users)) {
        return boost::none;
    }
    return it->second;
}

std::vector<tweeteria::TweetId> DataModel::getUserTimeline(tweeteria::UserId user_id) const
{
    std::vector<tweeteria::TweetId> ret;
    auto it = m_userTimelines.find(user_id);
    if(it != end(m_userTimelines)) {
        ret = it->second;
    }
    return ret;
}

boost::optional<tweeteria::Tweet> DataModel::getTweet(tweeteria::TweetId tweet_id) const
{
    std::lock_guard<std::mutex> lk(m_mtx);
    auto it = m_tweets.find(tweet_id);
    if(it == end(m_tweets)) {
        return boost::none;
    }
    return it->second;
}

void DataModel::awaitUserInfo(tweeteria::UserId user_to_wait_for, UserInfoCallback const& cb)
{
    std::unique_lock<std::mutex> lk(m_mtx);
    auto user_it = m_users.find(user_to_wait_for);
    if(user_it != end(m_users)) {
        // user info already arrived;
        tweeteria::User const user = user_it->second;
        lk.unlock();
        cb(user);
        return;
    }
    m_userInfoCallbacks[user_to_wait_for].emplace_back(cb);
}
