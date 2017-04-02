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

#include <ui/opening_dialog.hpp>

#include <tweeteria/tweeteria.hpp>

#include <cpprest/http_msg.h>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>


struct Bootstrapper::Pimpl {
    pplx::cancellation_token_source cts;

    tweeteria::OAuthCredentials oauth_creds;

    Pimpl() = default;
};

Bootstrapper::Bootstrapper(QObject* parent)
    :QObject(parent), m_pimpl(std::make_unique<Pimpl>()), m_proxyConfig(), m_connectivityTestGenerationCount(0)
{
}

// needed for pimpl:
Bootstrapper::~Bootstrapper() = default;

void Bootstrapper::checkConnectivity()
{
    ++m_connectivityTestGenerationCount;
    auto const gen_count = m_connectivityTestGenerationCount;
    emit connectivityCheckStarted(gen_count);
    tweeteria::Tweeteria::checkConnectivity(m_proxyConfig, m_pimpl->cts.get_token())
        .then([this, gen_count](pplx::task<void> const& result)
        {
            try {
                result.get();
                emit connectivityCheckSucceeded(gen_count);
            } catch(web::http::http_exception& e) {
                emit connectivityCheckFailed(gen_count, QString(e.what()));
            } catch(pplx::task_canceled&) {
                emit connectivityCheckFailed(gen_count, "Canceled.");
            } catch(std::exception& e) {
                emit connectivityCheckFailed(gen_count, QString("Unexpected error: ") + e.what());
            } catch(...) {
                emit connectivityCheckFailed(gen_count, "Unexpected error.");
            }
        });
}

void Bootstrapper::startOAuth()
{

}

void Bootstrapper::onProxyConfigurationChange(tweeteria::ProxyConfig new_proxy_config)
{
    m_pimpl->cts.cancel();
    m_pimpl->cts = pplx::cancellation_token_source();
    m_proxyConfig = std::move(new_proxy_config);
    checkConnectivity();
}
