
#include <tweeteria/image_util.hpp>
#include <tweeteria/exceptions.hpp>

#include <catch.hpp>

TEST_CASE("Test getProfileImageUrlsFromBaseUrl()")
{
    char const* http_normal = "http://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3_normal.png";
    char const* https_normal = "https://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3_normal.png";
    char const* http_bigger = "http://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3_bigger.png";
    char const* https_bigger = "https://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3_bigger.png";
    char const* http_mini = "http://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3_mini.png";
    char const* https_mini = "https://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3_mini.png";
    char const* http_original = "http://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3.png";
    char const* https_original = "https://pbs.twimg.com/profile_images/2284174872/7df3h38zabcvjylnyfe3.png";

    SECTION("Valid Url breakdown HTTP")
    {
        for(char const* url : { http_normal, http_bigger, http_mini, http_original })
        {
            auto const profile_images = tweeteria::getProfileImageUrlsFromBaseUrl(url);
            CHECK(profile_images.normal   == http_normal);
            CHECK(profile_images.bigger   == http_bigger);
            CHECK(profile_images.mini     == http_mini);
            CHECK(profile_images.original == http_original);
        }
    }

    SECTION("Valid Url breakdown HTTPS")
    {
        for(char const* url : { https_normal, https_bigger, https_mini, https_original })
        {
            auto const profile_images = tweeteria::getProfileImageUrlsFromBaseUrl(url);
            CHECK(profile_images.normal   == https_normal);
            CHECK(profile_images.bigger   == https_bigger);
            CHECK(profile_images.mini     == https_mini);
            CHECK(profile_images.original == https_original);
        }
    }

    SECTION("Urls without file extensions are also valid")
    {
        auto const chopExt = [](std::string const& str) {
            return str.substr(0, str.size() - 4);
        };
        for(char const* url : { http_normal, http_bigger, http_mini, http_original })
        {
            auto const profile_images = tweeteria::getProfileImageUrlsFromBaseUrl(chopExt(url));
            CHECK(profile_images.normal   == chopExt(http_normal));
            CHECK(profile_images.bigger   == chopExt(http_bigger));
            CHECK(profile_images.mini     == chopExt(http_mini));
            CHECK(profile_images.original == chopExt(http_original));
        }
    }

    SECTION("Invalid URL should throw")
    {
        CHECK_THROWS_AS(tweeteria::getProfileImageUrlsFromBaseUrl(""), tweeteria::InvalidArgument);
        CHECK_THROWS_AS(tweeteria::getProfileImageUrlsFromBaseUrl("foo"), tweeteria::InvalidArgument);
    }
}
