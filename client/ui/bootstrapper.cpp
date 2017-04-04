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

#include <api_credentials.hpp>

#include <tweeteria/tweeteria.hpp>

#include <cpprest/http_msg.h>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

#include <fstream>

struct Bootstrapper::Pimpl {
    pplx::cancellation_token_source cts;

    tweeteria::OAuthCredentials oauth_creds;

    pplx::task_completion_event<std::string> pin_retrieval_event;

    std::shared_ptr<tweeteria::Tweeteria> tweeteria;

    std::shared_ptr<tweeteria::User> user;

    Pimpl() = default;
};

Bootstrapper::Bootstrapper(QObject* parent)
    :QObject(parent), m_pimpl(std::make_unique<Pimpl>()), m_proxyConfig(), m_connectivityTestGenerationCount(0)
{
    {
        char const* proxy_cfg_filename = "tweeteria.proxy";
        std::ifstream fin_proxy_cfg(proxy_cfg_filename, std::ios_base::binary);
        if(fin_proxy_cfg) {
            auto const proxy_cfg = tweeteria::ProxyConfig::deserialize(fin_proxy_cfg);
            if(!fin_proxy_cfg) {
                GHULBUS_LOG(Error, "Error while reading proxy config from " << proxy_cfg_filename << ".");
            } else {
                m_proxyConfig = proxy_cfg;
            }
        }
    }
    {
        char const* credentials_filename = "tweeteria.cred";
        std::ifstream fin_credentials(credentials_filename, std::ios_base::binary);
        if(fin_credentials) {
            auto credentials = tweeteria::OAuthCredentials::deserialize(fin_credentials);
            if(!fin_credentials) {
                GHULBUS_LOG(Error, "Error while reading credentials from " << credentials_filename << ".");
            } else {
                m_pimpl->oauth_creds = credentials;
            }
        }
    }

    connect(this, &Bootstrapper::connectivityCheckSucceeded,
            this, &Bootstrapper::checkCredentials, Qt::QueuedConnection);

    connect(this, &Bootstrapper::oauthCredentialsUpdated,
            this, &Bootstrapper::checkCredentials, Qt::QueuedConnection);
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

void Bootstrapper::checkCredentials()
{
    if(m_pimpl->oauth_creds.access_token.empty() || m_pimpl->oauth_creds.token_secret.empty()) {
        tweeteria::Tweeteria::performOAuthAuthentication(
            APICredentials::consumer_key(), APICredentials::consumer_secret(),
            [this](std::string const& url) -> pplx::task<std::string> {
                emit oauthAuthorizationUrlReady(QString::fromStdString(url));
                return pplx::create_task(m_pimpl->pin_retrieval_event);
            }, m_proxyConfig).then([this](pplx::task<tweeteria::OAuthCredentials> const& task) {
                try {
                    auto const credentials = task.get();
                    GHULBUS_LOG(Info, "Successfully retrieved OAuth credentials.");
                    m_pimpl->oauth_creds = credentials;
                    emit oauthCredentialsUpdated();
                } catch(std::exception& e) {
                    // todo: retry on error
                    GHULBUS_LOG(Info, "Error retrieving credentials: " << e.what());
                }
            });
    } else {
        m_pimpl->tweeteria = std::make_shared<tweeteria::Tweeteria>(m_pimpl->oauth_creds);
        m_pimpl->tweeteria->verifyCredentials().then([this](pplx::task<tweeteria::VerificationResult> const& res) {
            try {
                auto const verification_result = res.get();
                m_pimpl->user = verification_result.user;
                if(verification_result.is_verified) {
                    emit credentialsVerified(*verification_result.user);
                }
            } catch(std::exception& e) {
                GHULBUS_LOG(Info, "Error while trying to verify user credentials: " << e.what());
            }
        });
    }
}

std::shared_ptr<tweeteria::Tweeteria> Bootstrapper::getTweeteria() const
{
    return m_pimpl->tweeteria;
}

std::shared_ptr<tweeteria::User> Bootstrapper::getVerifiedUser() const
{
    return m_pimpl->user;
}

void Bootstrapper::onOAuthPinEntered(QString const& pin_str)
{
    m_pimpl->pin_retrieval_event.set(pin_str.toStdString());
}

void Bootstrapper::onProxyConfigurationChange(tweeteria::ProxyConfig new_proxy_config)
{
    m_pimpl->cts.cancel();
    m_pimpl->cts = pplx::cancellation_token_source();
    m_proxyConfig = std::move(new_proxy_config);
    checkConnectivity();
}
