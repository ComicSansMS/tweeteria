
#include <tweeteria/user.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

namespace tweeteria
{
User User::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for User."); }
    User ret;
    ret.id = UserId(val["id"].GetUint64());
    ret.name = val["name"].GetString();
    ret.screen_name = val["screen_name"].GetString();
    ret.description = val["description"].GetString();
    ret.entities = UserEntities::fromJSON(val["entities"]);
    ret.followers_count = val["followers_count"].GetUint();
    ret.friends_count = val["friends_count"].GetUint();
    ret.favourites_count = val["favourites_count"].GetUint();
    ret.profile_image_url_https = val["profile_image_url_https"].GetString();
    return ret;
}
}
