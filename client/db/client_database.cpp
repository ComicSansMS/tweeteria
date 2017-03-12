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

#include <db/client_database.hpp>

#include <db/table_layouts.hpp>

#include <db/tables/friends.hpp>
#include <db/tables/sqlite_master.hpp>
#include <db/tables/tweeteria_client_properties.hpp>

#include <exceptions.hpp>
#include <version.hpp>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

#include <gbBase/Assert.hpp>
#include <gbBase/Exception.hpp>
#include <gbBase/Log.hpp>

#include <sstream>

namespace
{
inline bool constexpr sqlpp11_debug()
{
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}
}

struct ClientDatabase::Pimpl
{
    sqlpp::sqlite3::connection db;
    ClientDatabase::Pimpl(sqlpp::sqlite3::connection_config const& conf);
};

ClientDatabase::Pimpl::Pimpl(sqlpp::sqlite3::connection_config const& conf)
    :db(conf)
{
}

ClientDatabase::ClientDatabase(std::unique_ptr<Pimpl>&& pimpl)
    :m_pimpl(std::move(pimpl))
{}

ClientDatabase::ClientDatabase(std::string const& db_filename)
    :m_pimpl(nullptr)
{
    GHULBUS_LOG(Debug, "Opening database at " << db_filename << ".");
    sqlpp::sqlite3::connection_config conf;
    conf.debug = sqlpp11_debug();
    conf.flags = SQLITE_OPEN_READWRITE;
    conf.path_to_database = db_filename;

    m_pimpl = std::make_unique<Pimpl>(conf);
    auto& db = m_pimpl->db;

    auto const master_tab = tables::SqliteMaster{};
    if(db(select(master_tab.name)
        .from(master_tab)
        .where((master_tab.name == "tweeteria_client_properties") && (master_tab.type == "table"))).empty())
    {
        GHULBUS_THROW(Exceptions::DatabaseError(), "Database file does not contain a tweeteria client properties table.");
    }

    auto const prop_tab = tables::TweeteriaClientProperties{};
    for(auto const& r : db(select(prop_tab.value).from(prop_tab).where(prop_tab.id == "version")))
    {
        std::string const version = r.value;
        GHULBUS_LOG(Trace, "Database version " << version);
        if(version != std::string(TweeteriaClientVersion::versionString()))
        {
            GHULBUS_THROW(Exceptions::DatabaseError(), "Unsupported database version " + version + ".");
        }
    }
}

ClientDatabase::~ClientDatabase()
{
    // needed for pimpl
}

ClientDatabase ClientDatabase::createNewDatabase(std::string const& db_filename)
{
    GHULBUS_LOG(Info, "Creating new database at " << db_filename << ".");

    sqlpp::sqlite3::connection_config conf;
    conf.debug = sqlpp11_debug();
    conf.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    conf.path_to_database = db_filename;

    auto pimpl = std::make_unique<Pimpl>(conf);
    auto& db = pimpl->db;

    // create tables
    db.execute(table_layouts::tweeteria_client_properties());
    db.execute(table_layouts::friends());

    auto const prop_tab = tables::TweeteriaClientProperties{};
    db(insert_into(prop_tab).set(prop_tab.id    = "version",
                                 prop_tab.value = TweeteriaClientVersion::versionString()));

    GHULBUS_LOG(Info, " Successfully established database at " << db_filename << ".");
    return ClientDatabase(std::move(pimpl));
}

void ClientDatabase::updateUserRead(tweeteria::UserId user, tweeteria::TweetId tweet_read)
{
    auto& db = m_pimpl->db;
    auto const tab = tables::Friends{};

    auto const result = db(select(tab.lastReadId).from(tab).where(tab.userId == user.id));
    if(!result.empty())
    {
        if(tweeteria::TweetId(result.front().lastReadId) >= tweet_read) {
            // already up to date; nothing more to do
            return;
        }
        GHULBUS_LOG(Trace, "Updating last read entry for user " << user.id << " to " << tweet_read.id << ".");
        db(update(tab).set(tab.lastReadId = tweet_read.id).where(tab.userId == user.id));
    } else {
        GHULBUS_LOG(Trace, "Creating last read entry for user " << user.id << " to " << tweet_read.id << ".");
        db(insert_into(tab).set(tab.userId = user.id, tab.lastReadId = tweet_read.id));
    }
}

tweeteria::TweetId ClientDatabase::getLastReadForUser(tweeteria::UserId user)
{
    auto& db = m_pimpl->db;
    auto const tab = tables::Friends{};

    auto const result = db(select(tab.lastReadId).from(tab).where(tab.userId == user.id));
    if(!result.empty()) {
        return tweeteria::TweetId(result.front().lastReadId);
    }
    return tweeteria::TweetId(0);
}
