
#ifndef TWEETERIA_INCLUDE_GUARD_TWEET_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEET_HPP

#include <tweeteria/id_types.hpp>
#include <tweeteria/entities.hpp>

#include <rapidjson/fwd.h>

#include <cstdint>
#include <memory>
#include <string>

namespace tweeteria
{
struct User;

struct Tweet {
    std::string created_at;
    Entities entities;
    ExtendedEntities extended_entities;
    std::int32_t favorite_count;
    bool favorited;
    TweetId id;

    std::string in_reply_to_screen_name;
    TweetId in_reply_to_status_id;
    UserId in_reply_to_user_id;

    TweetId quoted_status_id;
    std::shared_ptr<Tweet> quoted_status;

    std::int32_t retweet_count;
    bool retweeted;

    std::shared_ptr<Tweet> retweeted_status;

    std::string text;
    Indices display_text_range;
    UserId user_id;

    std::string raw_json;

    static Tweet fromJSON(rapidjson::Value const& val);

    std::string getUrl(User const& author) const;
    std::string getPrettyJSON() const;
    std::string getDisplayText() const;
};
}

#endif
