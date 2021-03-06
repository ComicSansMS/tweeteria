
#include <tweeteria/user.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace tweeteria
{
User User::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for User."); }
    User ret;
    ret.id = UserId(val["id"].GetUint64());
    ret.name = val["name"].Get<std::string>();
    ret.screen_name = val["screen_name"].Get<std::string>();
    ret.description = val["description"].Get<std::string>();
    ret.entities = UserEntities::fromJSON(val["entities"]);
    ret.followers_count = val["followers_count"].GetUint();
    ret.friends_count = val["friends_count"].GetUint();
    ret.favourites_count = val["favourites_count"].GetUint();
    ret.profile_image_url_https = val["profile_image_url_https"].Get<std::string>();

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    val.Accept(writer);
    ret.raw_json = std::string(buffer.GetString(), buffer.GetSize());

    return ret;
}

std::string User::getPrettyJSON() const
{
    rapidjson::Document d;
    d.Parse(raw_json);
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetSize());
}
}
