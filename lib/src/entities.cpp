
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
    ret.text = val["text"].Get<std::string>();
    return ret;
}

Symbol Symbol::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Symbol."); }
    Symbol ret;
    ret.text = val["text"].Get<std::string>();
    ret.indices = Indices::fromJSON(val["indices"]);
    return ret;
}

Size Size::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Size."); }
    Size ret;
    ret.h = val["h"].GetInt();
    std::string const resize_str = val["resize"].Get<std::string>();
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
    ret.display_url = val["display_url"].Get<std::string>();
    ret.expanded_url = val["expanded_url"].Get<std::string>();
    ret.id = MediaId(val["id"].GetUint64());
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.media_url_https = val["media_url_https"].Get<std::string>();
    ret.sizes = Sizes::fromJSON(val["sizes"]);
    auto it_source_status_id = val.FindMember("source_status_id");
    ret.source_status_id = TweetId((it_source_status_id != val.MemberEnd()) ? it_source_status_id->value.GetUint64() : 0);
    ret.type = val["type"].Get<std::string>();
    ret.url = val["url"].Get<std::string>();
    return ret;
}

Variant Variant::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Variant."); }
    Variant ret;
    auto it_bitrate = val.FindMember("bitrate");
    ret.bitrate = (it_bitrate != val.MemberEnd()) ? it_bitrate->value.GetInt() : -1;
    ret.content_type = val["content_type"].Get<std::string>();
    ret.url = val["url"].Get<std::string>();
    return ret;
}

VideoInfo VideoInfo::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for VideoInfo."); }
    VideoInfo ret;
    ret.aspect_ratio[0] = val["aspect_ratio"].GetArray()[0].GetInt();
    ret.aspect_ratio[1] = val["aspect_ratio"].GetArray()[1].GetInt();
    auto it_duration_millis = val.FindMember("duration_millis");
    ret.duration_millis = (it_duration_millis != val.MemberEnd()) ? it_duration_millis->value.GetInt() : -1;
    auto it_variants = val.FindMember("variants");
    if(it_variants != val.MemberEnd()) {
        std::transform(it_variants->value.Begin(), it_variants->value.End(), std::back_inserter(ret.variants), Variant::fromJSON);
    }
    return ret;
}

ExtendedMedia ExtendedMedia::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for ExtendedMedia."); }
    ExtendedMedia ret;
    ret.display_url = val["display_url"].Get<std::string>();
    ret.expanded_url = val["expanded_url"].Get<std::string>();
    ret.id = MediaId(val["id"].GetUint64());
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.media_url_https = val["media_url_https"].Get<std::string>();
    ret.sizes = Sizes::fromJSON(val["sizes"]);
    auto it_source_status_id = val.FindMember("source_status_id");
    ret.source_status_id = TweetId((it_source_status_id != val.MemberEnd()) ? it_source_status_id->value.GetUint64() : 0);
    ret.type = val["type"].Get<std::string>();
    ret.url = val["url"].Get<std::string>();
    auto it_video_info = val.FindMember("video_info");
    if(it_video_info != val.MemberEnd()) {
        ret.video_info = VideoInfo::fromJSON(val["video_info"]);
    }
    return ret;
}

Url Url::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Url."); }
    Url ret;
    // the docs claim that display_url and expanded_url are always there,
    // but apparently they are absent or null sometimes in practice.
    // we fall back to the value of url in that case.
    if(val.HasMember("display_url") && val["display_url"].IsString()) {
        ret.display_url = val["display_url"].Get<std::string>();
    } else {
        ret.display_url = val["url"].Get<std::string>();
    }
    if(val.HasMember("expanded_url") && val["expanded_url"].IsString()) {
        ret.expanded_url = val["expanded_url"].Get<std::string>();
    } else {
        ret.expanded_url = val["url"].Get<std::string>();
    }
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.url = val["url"].Get<std::string>();
    return ret;
}

UserMention UserMention::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for UserMention."); }
    UserMention ret;
    ret.id = UserId(val["id"].GetUint64());
    ret.indices = Indices::fromJSON(val["indices"]);
    ret.name = val["name"].Get<std::string>();
    ret.screen_name = val["screen_name"].Get<std::string>();
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

ExtendedEntities ExtendedEntities::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for ExtendedEntities."); }
    ExtendedEntities ret;
    entitiesFromJSON(val, "media", ret.media);

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
