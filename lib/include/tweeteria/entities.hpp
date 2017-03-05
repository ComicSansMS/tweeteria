
#ifndef TWEETERIA_INCLUDE_GUARD_ENTITIES_HPP
#define TWEETERIA_INCLUDE_GUARD_ENTITIES_HPP

#include <tweeteria/id_types.hpp>

#include <rapidjson/fwd.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace tweeteria
{
struct Indices
{
    std::array<std::int32_t, 2> index;

    inline std::int32_t& operator[](std::size_t i) {
        return index[i];
    }

    inline std::int32_t const& operator[](std::size_t i) const {
        return index[i];
    }

    static Indices fromJSON(rapidjson::Value const& val);
};

struct Hashtag {
    Indices indices;
    std::string text;

    static Hashtag fromJSON(rapidjson::Value const& val);
};

struct Symbol {
    std::string text;
    Indices indices;

    static Symbol fromJSON(rapidjson::Value const& val);
};

struct Size {
    std::int32_t h;
    enum Resize {
        fit,
        crop
    } resize;
    std::int32_t w;

    static Size fromJSON(rapidjson::Value const& val);
};

struct Sizes {
    Size thumb;
    Size large;
    Size medium;
    Size small;

    static Sizes fromJSON(rapidjson::Value const& val);
};

struct Media
{
    std::string display_url;
    std::string expanded_url;
    MediaId id;
    Indices indices;
    std::string media_url_https;
    Sizes sizes;
    TweetId source_status_id;
    std::string type;
    std::string url;

    static Media fromJSON(rapidjson::Value const& val);
};

struct Url {
    std::string display_url;
    std::string expanded_url;
    Indices indices;
    std::string url;

    static Url fromJSON(rapidjson::Value const& val);
};

struct UserMention {
    UserId id;
    Indices indices;
    std::string name;
    std::string screen_name;

    static UserMention fromJSON(rapidjson::Value const& val);
};


struct Entities {
    std::vector<Hashtag> hashtags;
    std::vector<Symbol> symbols;
    std::vector<Media> media;
    std::vector<Url> urls;
    std::vector<UserMention> user_mentions;
    // @todo extended_entities

    static Entities fromJSON(rapidjson::Value const& val);
};

struct UserEntities {
    std::vector<Url> url;
    std::vector<Url> description;

    static UserEntities fromJSON(rapidjson::Value const& val);
};
}

#endif
