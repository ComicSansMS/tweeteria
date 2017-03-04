
#ifndef TWEETERIA_INCLUDE_GUARD_TWEET_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEET_HPP

#include <tweeteria/id_types.hpp>

#include <rapidjson/fwd.h>

#include <cstdint>
#include <string>

namespace tweeteria
{
struct Tweet {
    TweetId id;
    std::string text;
    UserId user_id;

    static Tweet fromJSon(rapidjson::Value const& val);
};
}

#endif
