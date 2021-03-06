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
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_WEB_RESOURCE_PROVIDER_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_WEB_RESOURCE_PROVIDER_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tweeteria {
struct ProxyConfig;
}

enum class CachingPolicy {
    DoCache,
    NoCaching
};

class WebResourceProvider {
private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
public:
    WebResourceProvider(tweeteria::ProxyConfig const& proxy_config);
    ~WebResourceProvider();

    WebResourceProvider(WebResourceProvider const&) = delete;
    WebResourceProvider& operator=(WebResourceProvider const&) = delete;

    void retrieve(std::string const& url, std::function<void(std::vector<unsigned char> const&)> retrieval_cb, CachingPolicy caching_policy);
};

#endif
