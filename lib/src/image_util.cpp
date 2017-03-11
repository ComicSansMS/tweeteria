
#include <tweeteria/image_util.hpp>

#include <tweeteria/exceptions.hpp>

#include <algorithm>

namespace tweeteria
{

ProfileImageUrl::ProfileImageUrl(bool is_https, std::string const& base_url, std::string const& extension)
{
    normal =   std::string("http") + (is_https ? "s" : "") + "://" + base_url + "_normal" + extension;
    bigger =   std::string("http") + (is_https ? "s" : "") + "://" + base_url + "_bigger" + extension;
    mini =     std::string("http") + (is_https ? "s" : "") + "://" + base_url + "_mini" + extension;
    original = std::string("http") + (is_https ? "s" : "") + "://" + base_url + extension;
}

ProfileImageUrl getProfileImageUrlsFromBaseUrl(std::string const& profile_image_base_url)
{
    bool is_https;
    std::size_t base_url_start;
    if(profile_image_base_url.compare(0, 7, "http://") == 0) {
        is_https = false;
        base_url_start = 7;
    } else if(profile_image_base_url.compare(0, 8, "https://") == 0) {
        is_https = true;
        base_url_start = 8;
    } else {
        throw InvalidArgument("Not a valid image url.");
    }

    auto extension_start = profile_image_base_url.rfind('.');
    std::string extension;
    if(extension_start != std::string::npos) {
        if(profile_image_base_url.find('/', extension_start) != std::string::npos) {
            // no extension
            extension_start = std::string::npos;
        } else {
            extension = profile_image_base_url.substr(extension_start);
        }
    }

    auto const normal_pos = static_cast<int>(profile_image_base_url.rfind("_normal"));
    auto const bigger_pos = static_cast<int>(profile_image_base_url.rfind("_bigger"));
    auto const mini_pos = static_cast<int>(profile_image_base_url.rfind("_mini"));
    auto const suffix_pos = std::max(std::max(normal_pos, bigger_pos), mini_pos);

    std::string base_url;
    if(suffix_pos != std::string::npos) {
        base_url = profile_image_base_url.substr(base_url_start, suffix_pos - base_url_start);
    } else {
        if(extension_start != std::string::npos) {
            base_url = profile_image_base_url.substr(base_url_start, extension_start - base_url_start);
        } else {
            base_url = profile_image_base_url.substr(base_url_start);
        }
    }
    ProfileImageUrl ret(is_https, base_url, extension);

    return ret;
}

}
