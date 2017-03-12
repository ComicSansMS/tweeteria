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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_DB_TABLES_SQLITE_MASTER_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_DB_TABLES_SQLITE_MASTER_HPP

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace tables
{
  namespace SqliteMaster_
  {
    struct Type
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "type";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T type;
            T& operator()() { return type; }
            const T& operator()() const { return type; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct Name
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct TblName
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "tbl_name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T tblName;
            T& operator()() { return tblName; }
            const T& operator()() const { return tblName; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct Rootpage
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "rootpage";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T rootpage;
            T& operator()() { return rootpage; }
            const T& operator()() const { return rootpage; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Sql
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "sql";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T sql;
            T& operator()() { return sql; }
            const T& operator()() const { return sql; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  }

  struct SqliteMaster: sqlpp::table_t<SqliteMaster,
               SqliteMaster_::Type,
               SqliteMaster_::Name,
               SqliteMaster_::TblName,
               SqliteMaster_::Rootpage,
               SqliteMaster_::Sql>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "sqlite_master";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T sqliteMaster;
        T& operator()() { return sqliteMaster; }
        const T& operator()() const { return sqliteMaster; }
      };
    };
  };
} // namespace tables
#endif
