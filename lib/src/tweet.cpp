
#include <tweeteria/tweet.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

namespace tweeteria
{
namespace {
template<typename T>
T getNullable(rapidjson::Value const& v)
{
    if(v.IsNull()) { return T{}; }
    return v.Get<T>();
}
}

Tweet Tweet::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Tweet."); }
    Tweet ret;
    ret.created_at = val["created_at"].GetString();
    ret.entities = Entities::fromJSON(val["entities"]);
    ret.favorite_count = getNullable<std::int32_t>(val["favorite_count"]);
    ret.favorited = getNullable<bool>(val["favorited"]);
    ret.id = TweetId(val["id"].GetUint64());

    ret.in_reply_to_screen_name = getNullable<std::string>(val["in_reply_to_screen_name"]);
    ret.in_reply_to_status_id = TweetId(getNullable<std::uint64_t>(val["in_reply_to_status_id"]));
    ret.in_reply_to_user_id = UserId(getNullable<std::uint64_t>(val["in_reply_to_user_id"]));

    auto it_quoted_status_id = val.FindMember("quoted_status_id");
    if(it_quoted_status_id != val.MemberEnd()) {
        ret.quoted_status_id = TweetId(it_quoted_status_id->value.GetUint64());
        ret.quoted_status = std::make_shared<Tweet>(Tweet::fromJSON(val["quoted_status"]));
    } else {
        ret.quoted_status_id = TweetId(0);
        ret.quoted_status = nullptr;
    }

    ret.retweet_count = val["retweet_count"].GetInt();
    ret.retweeted = val["retweeted"].GetBool();
    
    auto it_retweeted_status = val.FindMember("retweeted_status");
    if(it_retweeted_status != val.MemberEnd()) {
        ret.retweeted_status = std::make_shared<Tweet>(Tweet::fromJSON(it_retweeted_status->value));
    } else {
        ret.retweeted_status = nullptr;
    }

    ret.text = val["text"].GetString();
    ret.user_id = UserId(val["user"]["id"].GetUint64());
    return ret;
}
}
