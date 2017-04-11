
#include <tweeteria/detail/proxy_config_util.hpp>

#include <tweeteria/proxy_config.hpp>
#include <tweeteria/string_util.hpp>
#include <cpprest/http_client.h>

namespace
{

utility::string_t toUtilString(std::string const& str)
{
#ifdef _UTF16_STRINGS
    return tweeteria::convertUtf8ToUtf16(str);
#else
    return str;
#endif
}

}

namespace tweeteria
{
namespace detail
{

web::web_proxy constructProxyFromConfig(tweeteria::ProxyConfig const& cfg)
{
    if(cfg.mode == tweeteria::ProxyConfig::Mode::Manual) {
        web::uri_builder proxy_uri;
        proxy_uri.set_host(toUtilString(cfg.proxy_url));
        if(cfg.proxy_port != 0) {
            proxy_uri.set_port(cfg.proxy_port);
        }
        web::web_proxy ret(proxy_uri.to_uri());
        if(!cfg.proxy_login_user.empty() || !cfg.proxy_login_password.empty()) {
            web::credentials cred(toUtilString(cfg.proxy_login_user), toUtilString(cfg.proxy_login_password));
            ret.set_credentials(cred);
        }
        return ret;
    } else if(cfg.mode == tweeteria::ProxyConfig::Mode::Auto) {
        return web::web_proxy(web::web_proxy::web_proxy_mode::use_auto_discovery);
    } else if(cfg.mode == tweeteria::ProxyConfig::Mode::System) {
        return web::web_proxy(web::web_proxy::web_proxy_mode::use_default);
    } else {
        return web::web_proxy(web::web_proxy::web_proxy_mode::disabled);
    }
}

}
}
