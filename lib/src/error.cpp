
#include <tweeteria/error.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

#include <algorithm>
#include <iterator>

namespace tweeteria
{
Error Error::fromJSon(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Error."); }
    Error ret;
    ret.code = val["code"].GetUint();
    ret.message = val["message"].GetString();
    return ret;
}

Errors Errors::fromJSon(rapidjson::Value const& val)
{
    if(!val.IsObject() || !val["errors"].IsArray()) {
        throw InvalidJSONFormat("Unexpected JSon format for Errors.");
    }
    auto const& errors = val["errors"];
    Errors ret;
    ret.errors.reserve(errors.Size());
    std::transform(errors.Begin(), errors.End(), std::back_inserter(ret.errors), Error::fromJSon);
    return ret;
}
}
