
#include <tweeteria/tweet.hpp>

#include <tweeteria/exceptions.hpp>
#include <tweeteria/string_util.hpp>
#include <tweeteria/user.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <stdexcept>

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
    auto it_extended_entities = val.FindMember("extended_entities");
    if(it_extended_entities != val.MemberEnd()) {
        ret.extended_entities = ExtendedEntities::fromJSON(it_extended_entities->value);
    }
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

    ret.text = val["full_text"].Get<std::string>();
    ret.display_text_range = Indices::fromJSON(val["display_text_range"]);
    ret.user_id = UserId(val["user"]["id"].GetUint64());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    val.Accept(writer);
    ret.raw_json = std::string(buffer.GetString(), buffer.GetSize());

    return ret;
}

std::string Tweet::getUrl(User const& author) const
{
    if(author.id != user_id) { throw InvalidArgument("Provided User is not the author of this tweet."); }
    return std::string("https://twitter.com/") + author.screen_name + "/status/" + std::to_string(id.id);
}

std::string Tweet::getPrettyJSON() const
{
    rapidjson::Document d;
    d.Parse(raw_json);
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetSize());
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
    for(auto const& sym : entities.symbols) {
        replacements.emplace_back(
            sym.indices[0],
            sym.indices[1],
            sym.text
        );
    }
    for(auto const& um : entities.user_mentions) {
        replacements.emplace_back(
            um.indices[0],
            um.indices[1],
            std::string("<font color=\"#1DA1F2\">@<a href=\"https://twitter.com/") + um.screen_name + "\"><span style=\"color:#1DA1F2;\">" + um.screen_name + "</span></a></font>"
        );
    }
    for(auto const& url : entities.urls) {
        replacements.emplace_back(
            url.indices[0],
            url.indices[1],
            std::string("<a href=\"") + url.expanded_url + "\"><span style=\"color:#1DA1F2;\">" + url.display_url + "</span></a>"
        );
    }
    for(auto const& media : entities.media) {
        replacements.emplace_back(
            media.indices[0],
            media.indices[1],
            ""
        );
    }

    std::sort(begin(replacements), end(replacements));

    std::string ret;
    std::size_t src_i = 0;
    std::size_t src_end = text.length();
    auto replacements_it = replacements.cbegin();
    auto const base_it = text.cbegin();
    std::size_t utf8_correction = 0;
    while(src_i != src_end)
    {
        if(replacements_it != end(replacements)) {
            auto const start = base_it + src_i;
            auto it = advanceUtf8CodePoints(start, text.cend(), (replacements_it->startIndex + utf8_correction) - src_i);
            ret += std::string(base_it + src_i, it);
            std::size_t const len_code_units = it - (base_it + src_i);
            std::size_t const len_code_points = lengthUtf8CodePoints(base_it + src_i, it);
            utf8_correction += len_code_units - len_code_points;

            ret += replacements_it->replacement;
            auto const repl_code_point_it = advanceUtf8CodePoints(it, text.cend(), replacements_it->endIndex - replacements_it->startIndex);
            auto const repl_code_unit_it = it + (replacements_it->endIndex - replacements_it->startIndex);
            utf8_correction += repl_code_point_it - repl_code_unit_it;
            src_i = replacements_it->endIndex + utf8_correction;
            ++replacements_it;
        } else {
            ret += std::string(base_it + src_i, base_it + src_end);
            src_i = src_end;
        }
    }

    std::size_t it_newline;
    while((it_newline = ret.find('\n')) != std::string::npos) {
        ret.replace(it_newline, 1, "<br />");
    }

    return ret;
}
}
