
#ifndef TWEETERIA_INCLUDE_GUARD_PROXY_CONFIG_HPP
#define TWEETERIA_INCLUDE_GUARD_PROXY_CONFIG_HPP

#include <cstdint>
#include <string>

namespace tweeteria
{
struct ProxyConfig
{
    enum class Mode {
        None = 0,
        Auto,
        System,
        Manual
    };
    Mode mode;
    std::string proxy_url;
    std::uint16_t proxy_port;
    std::string proxy_login_user;
    std::string proxy_login_password;

    ProxyConfig()
        :mode(Mode::None)
    {}
};
}

#endif
