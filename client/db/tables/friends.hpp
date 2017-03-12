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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_DB_TABLES_FRIENDS_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_DB_TABLES_FRIENDS_HPP

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace tables
{
  namespace Friends_
  {
    struct UserId
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "user_id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T userId;
            T& operator()() { return userId; }
            const T& operator()() const { return userId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct LastReadId
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "last_read_id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lastReadId;
            T& operator()() { return lastReadId; }
            const T& operator()() const { return lastReadId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  }

  struct Friends: sqlpp::table_t<Friends,
               Friends_::UserId,
               Friends_::LastReadId>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "friends";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T friends;
        T& operator()() { return friends; }
        const T& operator()() const { return friends; }
      };
    };
  };
} // namespace tables
#endif
