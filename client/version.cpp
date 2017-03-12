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

#include <version.hpp>

#define TWEETERIA_CLIENT_VERSION_MAJOR 0
#define TWEETERIA_CLIENT_VERSION_MINOR 9
#define TWEETERIA_CLIENT_VERSION_PATCH 0
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

namespace TweeteriaClientVersion
{
int majorVersion()
{
    return TWEETERIA_CLIENT_VERSION_MAJOR;
}

int minorVersion()
{
    return TWEETERIA_CLIENT_VERSION_MINOR;
}

int patchVersion()
{
    return TWEETERIA_CLIENT_VERSION_PATCH;
}

int version()
{
    return TWEETERIA_CLIENT_VERSION_MAJOR * 10000 + TWEETERIA_CLIENT_VERSION_MINOR * 100 + TWEETERIA_CLIENT_VERSION_PATCH;
}

char const* versionString()
{
    return TO_STRING(TWEETERIA_CLIENT_VERSION_MAJOR) "." TO_STRING(TWEETERIA_CLIENT_VERSION_MINOR) "." TO_STRING(TWEETERIA_CLIENT_VERSION_PATCH);
}
}

