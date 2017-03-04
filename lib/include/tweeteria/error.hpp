
#ifndef TWEETERIA_INCLUDE_GUARD_ERROR_HPP
#define TWEETERIA_INCLUDE_GUARD_ERROR_HPP

#include <rapidjson/fwd.h>

#include <cstdint>
#include <string>
#include <vector>

namespace tweeteria
{
struct Error
{
    std::uint32_t code;
    std::string message;

    static Error fromJSon(rapidjson::Value const& val);
};

struct Errors
{
    std::vector<Error> errors;

    static Errors fromJSon(rapidjson::Value const& val);
};
}

#endif
