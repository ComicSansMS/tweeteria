
#include <tweeteria/image_util.hpp>

#include <tweeteria/exceptions.hpp>

#include <regex>

namespace tweeteria
{

ProfileImageUrl::ProfileImageUrl(bool is_https, std::string const& base_url, std::string const& extension)
{
    normal =   std::string("http") + (is_https ? "s" : "") + "://" + base_url + "_normal." + extension;
    bigger =   std::string("http") + (is_https ? "s" : "") + "://" + base_url + "_bigger." + extension;
    mini =     std::string("http") + (is_https ? "s" : "") + "://" + base_url + "_mini." + extension;
    original = std::string("http") + (is_https ? "s" : "") + "://" + base_url + "." + extension;
}

ProfileImageUrl getProfileImageUrlsFromBaseUrl(std::string const& profile_image_base_url)
{
    std::regex r(R"regx(^http(s?)://(\S*)\.(\S*)$)regx");
    std::smatch smatch;
    if(!std::regex_match(profile_image_base_url, smatch, r)) {
        throw InvalidArgument("Not a valid image url.");
    }
    std::regex r_modifier(R"regx((_normal|_bigger|_mini)$)regx");
    auto base_url = std::regex_replace(smatch[2].str(), r_modifier, "");
    ProfileImageUrl ret((smatch[1].length() == 1), base_url, smatch[3].str());
    return ret;
}

}
