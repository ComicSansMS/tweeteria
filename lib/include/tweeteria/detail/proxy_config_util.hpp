
#ifndef TWEETERIA_INCLUDE_GUARD_DETAIL_PROXY_CONFIG_UTIL_HPP
#define TWEETERIA_INCLUDE_GUARD_DETAIL_PROXY_CONFIG_UTIL_HPP

namespace web
{
class web_proxy;
}

namespace tweeteria
{
struct ProxyConfig;
namespace detail
{
web::web_proxy constructProxyFromConfig(tweeteria::ProxyConfig const& cfg);
}
}

#endif
