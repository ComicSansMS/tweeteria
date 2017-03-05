
#include <tweeteria/tweet.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

namespace tweeteria
{
Tweet Tweet::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Tweet."); }
    Tweet ret;
    ret.created_at = val["created_at"].GetString();
    ret.entities = Entities::fromJSON(val["entities"]);
    ret.id = TweetId(val["id"].GetUint64());
    ret.text = val["text"].GetString();
    ret.user_id = UserId(val["user"]["id"].GetUint64());
    return ret;
}
}
