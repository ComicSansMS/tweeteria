
#ifndef TWEETERIA_INCLUDE_GUARD_CURSOR_HPP
#define TWEETERIA_INCLUDE_GUARD_CURSOR_HPP

#include <tweeteria/id_types.hpp>

#include <rapidjson/fwd.h>

namespace tweeteria
{
struct Cursor
{
    CursorId next_cursor;
    CursorId previous_cursor;

    static Cursor fromJSON(rapidjson::Value const& val);
};
}

#endif
