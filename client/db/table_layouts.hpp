/*
 * Tweeteria - A minimalistic tweet reader.
 * Copyright (C) 2017  Andreas Weis (der_ghulbus@ghulbus-inc.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_DB_TABLE_LAYOUTS_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_DB_TABLE_LAYOUTS_HPP

namespace table_layouts
{
inline namespace v10000
{
/** A key/value store for saving generic properties.
 */
inline constexpr char const* tweeteria_client_properties()
{
    return R"(
        CREATE TABLE tweeteria_client_properties(
            id    TEXT      PRIMARY KEY,
            value TEXT
        );)";
}

/** Information about friends (ie. people you are following)
 */
inline constexpr char const* friends()
{
    return R"(
        CREATE TABLE friends(
            user_id         INTEGER   PRIMARY KEY,
            last_read_id    INTEGER
        );)";
}
}
}
#endif
