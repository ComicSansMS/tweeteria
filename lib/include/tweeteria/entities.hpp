
#ifndef TWEETERIA_INCLUDE_GUARD_ENTITIES_HPP
#define TWEETERIA_INCLUDE_GUARD_ENTITIES_HPP

#include <rapidjson/document.h>

#include <array>
#include <cstdint>
#include <string>

namespace tweeteria
{
struct Indices
{
    std::array<int, 2> indices;
};

struct Url {
    std::string display_url;
    std::string expanded_url;
    Indices indices;
    std::string url;
};

Url fromJSon(rapidjson::Value const& val);
}

#endif
