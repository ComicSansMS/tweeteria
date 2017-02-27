
#ifndef TWEETERIA_INCLUDE_GUARD_USER_HPP
#define TWEETERIA_INCLUDE_GUARD_USER_HPP

#include <cstdint>
#include <string>

namespace tweeteria
{
struct User {
    std::uint64_t id;
    std::string name;
    std::string screen_name;
    std::string description;
    std::uint32_t followers_count;
    std::uint32_t friends_count;
    std::uint32_t favourites_count;
    std::string profile_image_url_https;
};
}


#endif
