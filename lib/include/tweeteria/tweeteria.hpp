
#ifndef TWEETERIA_INCLUDE_GUARD_TWEETERIA_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEETERIA_HPP

#include <tweeteria/error.hpp>
#include <tweeteria/tweet.hpp>
#include <tweeteria/user.hpp>

#include <pplx/pplxtasks.h>

#include <iosfwd>
#include <functional>
#include <memory>
#include <vector>

namespace tweeteria
{
struct OAuthCredentials
{
    std::string consumer_key;
    std::string consumer_secret;
    std::string access_token;
    std::string token_secret;

    void serialize(std::ostream& os);
    static OAuthCredentials deserialize(std::istream& is);
};

struct VerificationResult
{
    bool is_verified;
    Errors errors;
};

class Tweeteria
{
public:
    enum class CredentialValidity
    {
        Valid,
        Invalid
    };
private:
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
public:
    Tweeteria(OAuthCredentials const& credentials);
    ~Tweeteria();

    Tweeteria(Tweeteria const&) = delete;
    Tweeteria& operator=(Tweeteria const&) = delete;

    pplx::task<VerificationResult> verifyCredentials();
};

std::vector<User> json_test();
std::vector<Tweet> json_test_tweets();
}


#endif
