
#include <tweeteria/proxy_config.hpp>

#include <tweeteria/exceptions.hpp>
#include <tweeteria/string_util.hpp>

#include <ostream>
#include <istream>
#include <type_traits>

namespace tweeteria
{
ProxyConfig::ProxyConfig()
    :mode(Mode::None)
{}

void ProxyConfig::serialize(std::ostream& os) const
{
    serialize_string("TWT100", os);
    os.write(reinterpret_cast<char const*>(&mode), sizeof(std::underlying_type<Mode>::type));
    if(mode == Mode::Manual) {
        serialize_string(proxy_url, os);
        os.write(reinterpret_cast<char const*>(&proxy_port), sizeof(proxy_port));
        serialize_string(proxy_login_user, os);
        serialize_string(proxy_login_password, os);
    }
}

/* static */
ProxyConfig ProxyConfig::deserialize(std::istream& is)
{
    auto const header = deserialize_string(is);
    if(header != "TWT100") { throw tweeteria::IOError("Invalid OAuth format."); }
    ProxyConfig ret;
    is.read(reinterpret_cast<char*>(&ret.mode), sizeof(std::underlying_type<Mode>::type));
    if(ret.mode == Mode::Manual) {
        ret.proxy_url = deserialize_string(is);
        is.read(reinterpret_cast<char*>(&ret.proxy_port), sizeof(ret.proxy_port));
        ret.proxy_login_user = deserialize_string(is);
        ret.proxy_login_password = deserialize_string(is);
    }
    return ret;
}
}
