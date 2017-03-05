
#ifndef TWEETERIA_INCLUDE_GUARD_USER_HPP
#define TWEETERIA_INCLUDE_GUARD_USER_HPP

#include <tweeteria/id_types.hpp>
#include <tweeteria/entities.hpp>

#include <rapidjson/fwd.h>

#include <cstdint>
#include <string>

namespace tweeteria
{
struct User {
    UserId id;
    std::string name;
    std::string screen_name;
    std::string description;
    UserEntities entities;
    std::uint32_t followers_count;
    std::uint32_t friends_count;
    std::uint32_t favourites_count;
    std::string profile_image_url_https;

    static User fromJSON(rapidjson::Value const& val);
};
}

#endif
