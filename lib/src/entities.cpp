
#include <tweeteria/entities.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

#include <algorithm>
#include <iterator>

namespace tweeteria
{
Indices Indices::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsArray()) { throw InvalidJSONFormat("Unexpected JSon format for Indices."); }
    Indices ret;
    ret.index[0] = val[0].GetUint();
    ret.index[1] = val[1].GetUint();
    return ret;
}

Hashtag Hashtag::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Hashtag."); }
    Hashtag ret;
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.text = val["text"].GetString();
    return ret;
}

Symbol Symbol::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Symbol."); }
    Symbol ret;
    ret.text = val["text"].GetString();
    ret.indices = Indices::fromJSON(val["indices"]);
    return ret;
}

Size Size::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Size."); }
    Size ret;
    ret.h = val["h"].GetInt();
    std::string const resize_str = val["resize"].GetString();
    if(resize_str == "fit") {
        ret.resize = Size::fit;
    } else if(resize_str == "crop") {
        ret.resize = Size::crop;
    } else {
        throw InvalidJSONFormat("Unexpected JSon format for Size.resize field.");
    }
    ret.w = val["w"].GetInt();
    return ret;
}

Sizes Sizes::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Sizes."); }
    Sizes ret;
    ret.thumb = Size::fromJSON(val["thumb"]);
    ret.large = Size::fromJSON(val["large"]);
    ret.medium = Size::fromJSON(val["medium"]);
    ret.small = Size::fromJSON(val["small"]);
    return ret;
}

Media Media::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Media."); }
    Media ret;
    ret.display_url = val["display_url"].GetString();
    ret.expanded_url = val["expanded_url"].GetString();
    ret.id = MediaId(val["id"].GetUint64());
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.media_url_https = val["media_url_https"].GetString();
    ret.sizes = Sizes::fromJSON(val["sizes"]);
    auto it_source_status_id = val.FindMember("source_status_id");
    ret.source_status_id = TweetId((it_source_status_id != val.MemberEnd()) ? val["source_status_id"].GetUint64() : 0);
    ret.type = val["type"].GetString();
    ret.url = val["url"].GetString();
    return ret;
}

Url Url::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Url."); }
    Url ret;
    ret.display_url = val["display_url"].GetString();
    ret.expanded_url = val["expanded_url"].GetString();
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.url = val["url"].GetString();
    return ret;
}

UserMention UserMention::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for UserMention."); }
    UserMention ret;
    ret.id = UserId(val["id"].GetUint64());
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.name = val["name"].GetString();
    ret.screen_name = val["screen_name"].GetString();
    return ret;
}

namespace {
template<typename T>
inline void entitiesFromJSON(rapidjson::Value const& val, char const* field, std::vector<T>& entities)
{
    if(!val.HasMember(field)) { return; }
    auto const& val_field = val[field];
    entities.reserve(val_field.Size());
    std::transform(val_field.Begin(), val_field.End(), std::back_inserter(entities), T::fromJSON);
}
}

Entities Entities::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Entities."); }
    Entities ret;
    entitiesFromJSON(val, "hashtags", ret.hashtags);
    entitiesFromJSON(val, "symbols", ret.symbols);
    entitiesFromJSON(val, "media", ret.media);
    entitiesFromJSON(val, "urls", ret.urls);
    entitiesFromJSON(val, "user_mentions", ret.user_mentions);
    return ret;
}

UserEntities UserEntities::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for User Entities."); }
    UserEntities ret;

    if(val.HasMember("url")) {
        auto const& field = val["url"];
        auto const& entities = field["urls"];
        ret.url.reserve(entities.Size());
        std::transform(entities.Begin(), entities.End(), std::back_inserter(ret.url), Url::fromJSON);
    }

    if(val.HasMember("description")) {
        auto const& field = val["description"];
        auto const& entities = field["urls"];
        ret.description.reserve(entities.Size());
        std::transform(entities.Begin(), entities.End(), std::back_inserter(ret.description), Url::fromJSON);
    }
    return ret;
}
}
