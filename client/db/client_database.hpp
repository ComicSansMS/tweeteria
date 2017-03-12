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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_DB_CLIENT_DATABASE_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_DB_CLIENT_DATABASE_HPP

#include <tweeteria/tweeteria.hpp>

#include <memory>
#include <string>

class ClientDatabase {
private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
private:
    ClientDatabase(std::unique_ptr<Pimpl>&& pimpl);
public:
    ClientDatabase(std::string const& db_filename);
    ~ClientDatabase();

    ClientDatabase(ClientDatabase const&) = delete;
    ClientDatabase& operator=(ClientDatabase const&) = delete;

    ClientDatabase(ClientDatabase&&) = default;
    ClientDatabase& operator=(ClientDatabase&&) = default;

    static ClientDatabase createNewDatabase(std::string const& db_filename);

    void updateUserRead(tweeteria::UserId user, tweeteria::TweetId tweet_read);

    tweeteria::TweetId getLastReadForUser(tweeteria::UserId user);
};

#endif
