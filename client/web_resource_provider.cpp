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
#include <web_resource_provider.hpp>

#include <tweeteria/proxy_config.hpp>
#include <tweeteria/string_util.hpp>

#include <tweeteria/detail/proxy_config_util.hpp>

#include <gbBase/Assert.hpp>

#include <cpprest/http_client.h>
#include <cpprest/uri.h>

#include <unordered_map>

struct WebResourceProvider::Pimpl
{
    std::unordered_map<utility::string_t, web::http::client::http_client> connections;
    std::unordered_map<std::string, std::vector<unsigned char>> cache;
    std::vector<pplx::task<void>> tasks;
    tweeteria::ProxyConfig proxy_config;
};

WebResourceProvider::WebResourceProvider(tweeteria::ProxyConfig const& proxy_config)
    :m_pimpl(new WebResourceProvider::Pimpl())
{
    m_pimpl->proxy_config = proxy_config;
}

WebResourceProvider::~WebResourceProvider()
{
    // needed for pimpl
}

void WebResourceProvider::retrieve(std::string const& url, std::function<void(std::vector<unsigned char> const&)> retrieval_cb, CachingPolicy caching_policy)
{
    web::uri uri(
#ifdef _UTF16_STRINGS
        tweeteria::convertUtf8ToUtf16(url)
#else
        url
#endif
    );
    auto const host = uri.authority();
    auto const host_str = host.to_string();
    auto it = m_pimpl->connections.find(host_str);
    if(it == end(m_pimpl->connections))
    {
        auto const proxy = tweeteria::detail::constructProxyFromConfig(m_pimpl->proxy_config);
        web::http::client::http_client_config config;
        config.set_proxy(proxy);
        auto ret = m_pimpl->connections.emplace(std::make_pair(host_str, web::http::client::http_client(host, config)));
        GHULBUS_ASSERT(ret.second);
        it = ret.first;
    }
    auto& http_client = it->second;

    m_pimpl->tasks.emplace_back(
        http_client.request(web::http::methods::GET, uri.resource().to_string()).then([](web::http::http_response response)
        {
            return response.extract_vector();
        }).then([retrieval_cb](std::vector<unsigned char> data)
        {
            retrieval_cb(data);
        }));
}
