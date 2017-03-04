
#include <tweeteria/tweeteria.hpp>

#include <tweeteria/exceptions.hpp>
#include <tweeteria/string_util.hpp>

#include <cpprest/http_client.h>
#include <cpprest/oauth1.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <exception>
#include <iostream>
#include <istream>
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
                return VerificationResult{ false, Errors::fromJSon(d) };
            });
        } else {
            return pplx::task_from_exception<VerificationResult>(
                std::make_exception_ptr(APIProtocolViolation("Unexpected http return code for verify.")));
        }
    });
}


std::ostream& operator<<(std::ostream& os, User const& u)
{
    os << "@" << u.screen_name << " (" <<  u.name << ")\n" << u.description
       << "\nFollowers: " << u.followers_count << " Following: " << u.friends_count << " Favourites: " << u.favourites_count;
    return os;
}

std::vector<User> json_test()
{
    std::ifstream fin("friends.txt");
    std::string str;
    std::getline(fin, str);
    rapidjson::Document d;
    d.Parse(str.data(), str.length());
    bool is_obj = d.IsObject();
    std::vector<User> users;
    for(auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        std::cout << (it->name.GetString()) << std::endl;
        if(it->name == "users") {
            auto& user_arr = it->value.GetArray();
            for(rapidjson::SizeType i=0; i<user_arr.Size(); ++i) {
                users.push_back(User::fromJSon(user_arr[i]));
            }
        }
    }
    for(auto const& u : users) {
        std::cout << "\n\n-------------------------------------------------\n";
        std::cout << u << std::endl;
    }
    return users;
}

std::vector<Tweet> json_test_tweets()
{
    std::ifstream fin("timeline.txt");
    std::string str;
    std::getline(fin, str);
    rapidjson::Document d;
    d.Parse(str.data(), str.length());
    std::vector<Tweet> tweets;
    for(rapidjson::SizeType i=0; i<d.Size(); ++i) {
        tweets.push_back(Tweet::fromJSon(d[i]));
    }
    return tweets;
}
}
