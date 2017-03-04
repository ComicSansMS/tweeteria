
#include <tweeteria/tweet.hpp>

#include <rapidjson/document.h>

namespace tweeteria
{
Tweet Tweet::fromJSon(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw std::runtime_error(""); }
    Tweet ret;
    ret.id = TweetId(val["id"].GetUint64());
    ret.text = val["text"].GetString();
    ret.user_id = UserId(val["user"]["id"].GetUint64());
    return ret;
}
}
