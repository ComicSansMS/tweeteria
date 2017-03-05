
#include <tweeteria/tweeteria.hpp>

#include <tweeteria/exceptions.hpp>
#include <tweeteria/string_util.hpp>

#include <cpprest/http_client.h>
#include <cpprest/oauth1.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <istream>
#include <iterator>
#include <memory>
#include <ostream>
#include <fstream>
#include <string>

namespace 
{
namespace Endpoints
{
utility::string_t oauth_temp_endpoint()
{
    return utility::string_t(U("https://api.twitter.com/oauth/request_token"));
}
utility::string_t oauth_auth_endpoint()
{
    return utility::string_t(U("https://api.twitter.com/oauth/authenticate"));
}
utility::string_t oauth_token_endpoint()
{
    return utility::string_t(U("https://api.twitter.com/oauth/access_token"));
}
utility::string_t oauth_callback_uri()
{
    return utility::string_t(U("oob"));
}
utility::string_t twitter_api_endpoint()
{
    return utility::string_t(U("https://api.twitter.com/1.1/"));
}
} // namespace Endpoints

utility::string_t toUtilString(std::string const& str)
{
#ifdef _UTF16_STRINGS
    return tweeteria::convertUtf8ToUtf16(str);
#else
    return str;
#endif
}

std::string fromUtilString(utility::string_t const& util_str)
{
#ifdef _UTF16_STRINGS
    return tweeteria::convertUtf16ToUtf8(util_str);
#else
    return str;
#endif
}

std::shared_ptr<web::http::oauth1::experimental::oauth1_config> createOAuthConfig(tweeteria::OAuthCredentials const& credentials)
{
    auto ret = std::make_shared<web::http::oauth1::experimental::oauth1_config>(
        toUtilString(credentials.consumer_key), toUtilString(credentials.consumer_secret),
        Endpoints::oauth_temp_endpoint(), Endpoints::oauth_auth_endpoint(),
        Endpoints::oauth_token_endpoint(), Endpoints::oauth_callback_uri(),
        web::http::oauth1::experimental::oauth1_methods::hmac_sha1);
    web::http::oauth1::experimental::oauth1_token oauth_token(
        toUtilString(credentials.access_token), toUtilString(credentials.token_secret));
    if(!oauth_token.is_valid_access_token()) { throw tweeteria::InvalidArgument("Invalid OAuth token."); }
    ret->set_token(oauth_token);
    return ret;
}

void serialize_string(std::string const& str, std::ostream& os)
{
    std::uint64_t const string_size = str.length();
    os.write(reinterpret_cast<char const*>(&string_size), sizeof(string_size));
    os.write(str.c_str(), string_size);
    if(!os) { throw tweeteria::IOError("Error while serializing string."); }
}

std::string deserialize_string(std::istream& is)
{
    std::uint64_t string_size;
    is.read(reinterpret_cast<char*>(&string_size), sizeof(string_size));
    std::vector<char> buffer;
    buffer.resize(string_size, '0');
    is.read(buffer.data(), buffer.size());
    if(!is) { throw tweeteria::IOError("Error while deserializing string."); }
    return std::string(begin(buffer), end(buffer));
}
} // anonymous namespace

namespace tweeteria
{

void OAuthCredentials::serialize(std::ostream& os)
{
    serialize_string("TWT100", os);
    serialize_string(consumer_key, os);
    serialize_string(consumer_secret, os);
    serialize_string(access_token, os);
    serialize_string(token_secret, os);
}

/* static */
OAuthCredentials OAuthCredentials::deserialize(std::istream& is)
{
    auto const header = deserialize_string(is);
    if(header != "TWT100") { throw tweeteria::IOError("Invalid OAuth format."); }
    OAuthCredentials ret;
    ret.consumer_key = deserialize_string(is);
    ret.consumer_secret = deserialize_string(is);
    ret.access_token = deserialize_string(is);
    ret.token_secret = deserialize_string(is);
    return ret;
}

struct Tweeteria::Pimpl
{
    std::shared_ptr<web::http::oauth1::experimental::oauth1_config> oauth_config;
    std::shared_ptr<web::http::oauth1::details::oauth1_handler> oauth_handler;
    web::http::client::http_client http_client;

    Pimpl(OAuthCredentials const& credentials)
        :oauth_handler(std::make_shared<web::http::oauth1::details::oauth1_handler>(createOAuthConfig(credentials))),
         http_client(Endpoints::twitter_api_endpoint())
    {
        http_client.add_handler(oauth_handler);
    }

    pplx::task<std::tuple<Cursor, std::vector<UserId>>> getFriendsIds(
        bool use_id, UserId user_id, std::string const& user_name, CursorId cursor_id);

    pplx::task<std::tuple<Cursor, std::vector<User>>> getFriendsList(
        bool use_id, UserId user_id, std::string const& user_name, CursorId cursor_id);
};

Tweeteria::Tweeteria(OAuthCredentials const& credentials)
    :m_pimpl(std::make_unique<Pimpl>(credentials))
{
}

Tweeteria::~Tweeteria()
{
    // required for pimpl
}

pplx::task<VerificationResult> Tweeteria::verifyCredentials()
{
    web::http::uri_builder request_uri(U("/account/verify_credentials.json"));
    request_uri.append_query(U("skip_status"), U("1"));
    web::http::http_request request(web::http::methods::GET);
    request.set_request_uri(request_uri.to_uri());

    return m_pimpl->http_client.request(request).then([](web::http::http_response response)
    {
        auto const status_code = response.status_code();
        if(status_code == web::http::status_codes::OK) {
            return pplx::task_from_result<VerificationResult>(VerificationResult{ true, Errors{} });
        } else if(status_code == web::http::status_codes::Unauthorized) {
            return response.extract_utf8string().then([status_code](std::string body)
            {
                rapidjson::Document d;
                d.Parse(body);
                return VerificationResult{ false, Errors::fromJSON(d) };
            });
        } else {
            throw APIProtocolViolation("Unexpected http return code for account/verify_credentials.");
        }
    });
}

MultiPageResult<std::vector<UserId>> Tweeteria::getMyFriendsIds()
{
    return MultiPageResult<std::vector<UserId>>([this](CursorId cursor_id) {
        return getFriendsIds(UserId(0), cursor_id);
    });
}

MultiPageResult<std::vector<UserId>> Tweeteria::getFriendsIds(UserId user_id)
{
    return MultiPageResult<std::vector<UserId>>([this, user_id](CursorId cursor_id) {
        return getFriendsIds(user_id, cursor_id);
    });
}

MultiPageResult<std::vector<UserId>> Tweeteria::getFriendsIds(std::string const& user_name)
{
    return MultiPageResult<std::vector<UserId>>([this, user_name](CursorId cursor_id) {
        return getFriendsIds(user_name, cursor_id);
    });
}

pplx::task<std::tuple<Cursor, std::vector<UserId>>> Tweeteria::getFriendsIds(std::string const& user_name, CursorId cursor_id)
{
    return m_pimpl->getFriendsIds(false, UserId(), user_name, cursor_id);
}

pplx::task<std::tuple<Cursor, std::vector<UserId>>> Tweeteria::getFriendsIds(UserId user_id, CursorId cursor_id)
{
    return m_pimpl->getFriendsIds(true, user_id, std::string(), cursor_id);
}

pplx::task<std::tuple<Cursor, std::vector<UserId>>> Tweeteria::Pimpl::getFriendsIds(
    bool use_id, UserId user_id, std::string const& user_name, CursorId cursor_id)
{
    web::http::uri_builder request_uri(U("/friends/ids.json"));
    if(use_id) {
        if(user_id.id != 0) {
            request_uri.append_query(U("user_id"), user_id.id);
        }
    } else {
        if(!user_name.empty()) {
            request_uri.append_query(U("screen_name"), toUtilString(user_name));
        }
    }
    request_uri.append_query(U("cursor"), cursor_id.id);
    web::http::http_request request(web::http::methods::GET);
    request.set_request_uri(request_uri.to_uri());

    return http_client.request(request).then([](web::http::http_response response) {
        if(response.status_code() != web::http::status_codes::OK) {
            throw APIProtocolViolation("Unexpected http return code for friends/ids.");
        }
        return response.extract_utf8string();
    }).then([](std::string body) {
        rapidjson::Document d;
        d.Parse(body);
        std::vector<UserId> ret;
        auto const& ids = d["ids"];
        ret.reserve(ids.Size());
        std::transform(ids.Begin(), ids.End(), std::back_inserter(ret),
                       [](rapidjson::Value const& v) { return UserId(v.GetUint64()); });
        auto const cursor = Cursor::fromJSON(d);
        return std::make_tuple(cursor, ret);
    });
}

MultiPageResult<std::vector<User>> Tweeteria::getMyFriendsList()
{
    return MultiPageResult<std::vector<User>>([this](CursorId cursor_id) {
        return m_pimpl->getFriendsList(true, UserId(0), std::string(), cursor_id);
    });
}

pplx::task<std::tuple<Cursor, std::vector<User>>> Tweeteria::Pimpl::getFriendsList(
    bool use_id, UserId user_id, std::string const& user_name, CursorId cursor_id)
{
    web::http::uri_builder request_uri(U("/friends/list.json"));
    if(use_id) {
        if(user_id.id != 0) {
            request_uri.append_query(U("user_id"), user_id.id);
        }
    } else {
        if(!user_name.empty()) {
            request_uri.append_query(U("screen_name"), toUtilString(user_name));
        }
    }
    request_uri.append_query(U("cursor"), cursor_id.id);
    web::http::http_request request(web::http::methods::GET);
    request.set_request_uri(request_uri.to_uri());

    return http_client.request(request).then([](web::http::http_response response) {
        if(response.status_code() != web::http::status_codes::OK) {
            throw APIProtocolViolation("Unexpected http return code for friends/list.");
        }
        return response.extract_utf8string();
    }).then([](std::string body) {
        rapidjson::Document d;
        d.Parse(body);
        std::vector<User> ret;
        auto const& users = d["users"];
        ret.reserve(users.Size());
        std::transform(users.Begin(), users.End(), std::back_inserter(ret), User::fromJSON);
        auto const cursor = Cursor::fromJSON(d);
        return std::make_tuple(cursor, ret);
    });
}

pplx::task<std::vector<User>> Tweeteria::getUsers(std::vector<UserId> const& user_ids)
{
    if(user_ids.empty()) { return pplx::task_from_result(std::vector<User>()); }
    if(user_ids.size() > 100) { throw InvalidArgument("Not more than 100 ids per request allowed."); }

    std::string csv_list_acc;
    bool is_first = true;
    for(auto const& id : user_ids) {
        if(is_first) {
            is_first = false;
        } else {
            csv_list_acc.append("%2C");
        }
        csv_list_acc.append(std::to_string(id.id));
    }
    auto const csv_list = toUtilString(csv_list_acc);
    web::http::uri_builder request_uri(U("/users/lookup.json"));
    request_uri.append_query(U("user_id"), csv_list, false);
    web::http::http_request request(web::http::methods::GET);
    request.set_request_uri(request_uri.to_uri());
    return m_pimpl->http_client.request(request).then([](web::http::http_response response)
    {
        if(response.status_code() != web::http::status_codes::OK) {
            throw APIProtocolViolation("Unexpected http return code for friends/ids.");
        }
        return response.extract_utf8string();
    }).then([](std::string body) {
        rapidjson::Document d;
        d.Parse(body);
        std::vector<User> ret;
        ret.reserve(d.Size());
        std::transform(d.Begin(), d.End(), std::back_inserter(ret), User::fromJSON);
        return ret;
    });
}

pplx::task<std::vector<Tweet>> Tweeteria::getUserTimeline(UserId user_id)
{
    web::http::uri_builder request_uri(U("/statuses/user_timeline.json"));
    if(user_id.id != 0) {
        request_uri.append_query(U("user_id"), user_id.id);
    }
    request_uri.append_query(U("trim_user"), U("1"));
    request_uri.append_query(U("exclude_replies"), U("1"));
    web::http::http_request request(web::http::methods::GET);
    request.set_request_uri(request_uri.to_uri());

    return m_pimpl->http_client.request(request).then([](web::http::http_response response) {
        if(response.status_code() != web::http::status_codes::OK) {
            throw APIProtocolViolation("Unexpected http return code for friends/list.");
        }
        return response.extract_utf8string();
    }).then([](std::string body) {
        rapidjson::Document d;
        d.Parse(body);
        std::vector<Tweet> ret;
        ret.reserve(d.Size());
        std::transform(d.Begin(), d.End(), std::back_inserter(ret), Tweet::fromJSON);
        return ret;
    });
}

std::ostream& operator<<(std::ostream& os, User const& u)
{
    os << "@" << u.screen_name << " (" <<  u.name << ")\n" << u.description
       << "\nFollowers: " << u.followers_count << " Following: " << u.friends_count << " Favourites: " << u.favourites_count;
    return os;
}
}
