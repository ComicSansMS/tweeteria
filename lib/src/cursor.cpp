
#include <tweeteria/cursor.hpp>

#include <tweeteria/exceptions.hpp>

#include <rapidjson/document.h>

namespace tweeteria
{
Cursor Cursor::fromJSON(rapidjson::Value const& val)
{
    if(!val.IsObject()) { throw InvalidJSONFormat("Unexpected JSon format for Cursor."); }
    Cursor ret;
    ret.next_cursor = CursorId(val["next_cursor"].GetInt64());
    ret.previous_cursor = CursorId(val["previous_cursor"].GetInt64());
    return ret;
}
}
