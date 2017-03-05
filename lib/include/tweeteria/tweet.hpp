
#ifndef TWEETERIA_INCLUDE_GUARD_TWEET_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEET_HPP

#include <tweeteria/id_types.hpp>
#include <tweeteria/entities.hpp>

#include <rapidjson/fwd.h>

#include <cstdint>
#include <string>

namespace tweeteria
{
struct Tweet {
    std::string created_at;
    Entities entities;
    TweetId id;
    std::string text;
    UserId user_id;

    static Tweet fromJSON(rapidjson::Value const& val);
};
}

#endif
