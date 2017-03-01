
#ifndef TWEETERIA_INCLUDE_GUARD_TWEET_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEET_HPP

#include <rapidjson/fwd.h>

#include <cstdint>
#include <string>

namespace tweeteria
{
struct Tweet {
    std::uint64_t id;
    std::string text;
    std::uint64_t user_id;

    static Tweet fromJSon(rapidjson::Value const& val);
};
}

#endif
