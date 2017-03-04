
#ifndef TWEETERIA_INCLUDE_GUARD_IMAGE_UTIL_HPP
#define TWEETERIA_INCLUDE_GUARD_IMAGE_UTIL_HPP

#include <string>

namespace tweeteria
{
class ProfileImageUrl
{
public:
    ProfileImageUrl(bool is_https, std::string const& base_url, std::string const& extension);
    std::string normal;
    std::string bigger;
    std::string mini;
    std::string original;
};

ProfileImageUrl getProfileImageUrlsFromBaseUrl(std::string const& profile_image_base_url);
}

#endif
