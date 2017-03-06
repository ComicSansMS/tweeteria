
#include <tweeteria/tweet.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

#include <algorithm>

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
    ret.created_at = val["created_at"].Get<std::string>();
    ret.entities = Entities::fromJSON(val["entities"]);
    ret.favorite_count = getNullable<std::int32_t>(val["favorite_count"]);
    ret.favorited = getNullable<bool>(val["favorited"]);
    ret.id = TweetId(val["id"].GetUint64());

    ret.in_reply_to_screen_name = getNullable<std::string>(val["in_reply_to_screen_name"]);
    ret.in_reply_to_status_id = TweetId(getNullable<std::uint64_t>(val["in_reply_to_status_id"]));
    ret.in_reply_to_user_id = UserId(getNullable<std::uint64_t>(val["in_reply_to_user_id"]));

    auto const it_quoted_status_id = val.FindMember("quoted_status_id");
    ret.quoted_status_id = TweetId((it_quoted_status_id != val.MemberEnd()) ? (it_quoted_status_id->value.GetUint64()) : 0);
    auto const it_quoted_status = val.FindMember("quoted_status");
    ret.quoted_status = ((it_quoted_status != val.MemberEnd()) ? std::make_shared<Tweet>(Tweet::fromJSON(val["quoted_status"])) : nullptr);

    ret.retweet_count = val["retweet_count"].GetInt();
    ret.retweeted = val["retweeted"].GetBool();
    
    auto it_retweeted_status = val.FindMember("retweeted_status");
    if(it_retweeted_status != val.MemberEnd()) {
        ret.retweeted_status = std::make_shared<Tweet>(Tweet::fromJSON(it_retweeted_status->value));
    } else {
        ret.retweeted_status = nullptr;
    }

    ret.text = val["text"].Get<std::string>();
    ret.user_id = UserId(val["user"]["id"].GetUint64());
    return ret;
}

struct Replacement {
    std::int32_t startIndex;
    std::int32_t endIndex;
    std::string replacement;

    Replacement(std::int32_t start, std::int32_t end, std::string const& str)
        :startIndex(start), endIndex(end), replacement(str)
    {}

    Replacement(std::int32_t start, std::int32_t end, std::string&& str)
        :startIndex(start), endIndex(end), replacement(std::move(str))
    {}
};

inline bool operator<(Replacement const& lhs, Replacement const& rhs) {
    return lhs.startIndex < rhs.startIndex;
}

std::string Tweet::getDisplayText() const
{
    std::vector<Replacement> replacements;
    if(retweeted_status) {
        replacements.emplace_back(0, 2, std::string(""));
    }
    for(auto const& ht : entities.hashtags) {
        replacements.emplace_back(
            ht.indices[0],
            ht.indices[1],
            std::string("<font color=\"#1DA1F2\">#<a href=\"https://twitter.com/hashtag/") + ht.text + "?src=hash\"><span style=\"color:#1DA1F2;\">" + ht.text + "</span></a></font>"
        );
    }
    for(auto const& um : entities.user_mentions) {
        replacements.emplace_back(
            um.indices[0],
            um.indices[1],
            std::string("<font color=\"#1DA1F2\">@<a href=\"https://twitter.com/") + um.screen_name + "\"><span style=\"color:#1DA1F2;\">" + um.screen_name + "</span></a></font>"
        );
    }

    std::sort(begin(replacements), end(replacements));

    std::string ret;
    std::size_t src_i = 0;
    std::size_t src_end = text.length();
    auto replacements_it = replacements.cbegin();
    auto const base_it = text.cbegin();
    while(src_i != src_end)
    {
        if(replacements_it != end(replacements)) {
            ret += std::string(base_it + src_i, base_it + replacements_it->startIndex);
            ret += replacements_it->replacement;
            src_i = replacements_it->endIndex;
            ++replacements_it;
        } else {
            ret += std::string(base_it + src_i, base_it + src_end);
            src_i = src_end;
        }
    }

    return ret;
}
}
