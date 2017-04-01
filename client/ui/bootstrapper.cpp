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
#include <ui/bootstrapper.hpp>

#include <tweeteria/tweeteria.hpp>

#include <cpprest/http_msg.h>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

Bootstrapper::Bootstrapper(QObject* parent)
    :QObject(parent)
{
}

void Bootstrapper::checkConnectivity()
{
    tweeteria::checkConnectivity().then([this](pplx::task<void> const& result) {
        try {
            result.get();
            emit connectivityCheckSucceeded();
        } catch(web::http::http_exception& e) {
            emit connectivityCheckFailed(QString(e.what()));
        } catch(...) {
            emit connectivityCheckFailed("Unexpected error.");
        }
    });
}
