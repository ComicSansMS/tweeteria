
#ifndef TWEETERIA_INCLUDE_GUARD_TWEETERIA_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEETERIA_HPP

#include <tweeteria/cursor.hpp>
#include <tweeteria/error.hpp>
#include <tweeteria/exceptions.hpp>
#include <tweeteria/id_types.hpp>
#include <tweeteria/tweet.hpp>
#include <tweeteria/user.hpp>

#include <pplx/pplxtasks.h>

#include <iosfwd>
#include <functional>
#include <memory>
#include <vector>
#include <tuple>

namespace tweeteria
{
struct ProxyConfig;

struct OAuthCredentials
{
    std::string consumer_key;
    std::string consumer_secret;
    std::string access_token;
    std::string token_secret;

    void serialize(std::ostream& os) const;
    static OAuthCredentials deserialize(std::istream& is);
};

struct VerificationResult
{
    bool is_verified;
    Errors errors;
    std::shared_ptr<User> user;
};

template<typename T>
class MultiPageResult
{
public:
    typedef std::function<pplx::task<std::tuple<Cursor, T>>(CursorId)> RetrievalFunc;
private:
    RetrievalFunc m_retrieve;
    Cursor m_cursor;
public:
    explicit MultiPageResult(RetrievalFunc retrieval_func)
        :m_retrieve(retrieval_func)
    {
        if(!m_retrieve) { throw InvalidArgument("Empty retrieval function."); }
        m_cursor.next_cursor = CursorId(-1);
        m_cursor.previous_cursor = CursorId(-1);
    }

    bool done() const
    {
        return (m_cursor.next_cursor == CursorId(0));
    }

    pplx::task<T> nextPage()
    {
        return retrieveFromCursor(m_cursor.next_cursor);
    }

    pplx::task<T> previousPage()
    {
        return retrieveFromCursor(m_cursor.previous_cursor);
    }

private:
    pplx::task<T> retrieveFromCursor(CursorId cursor)
    {
        return m_retrieve(cursor).then([this](std::tuple<Cursor, T> res)
        {
            m_cursor = std::get<Cursor>(res);
            return std::get<T>(std::move(res));
        });
    }
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

    MultiPageResult<std::vector<UserId>> getMyFriendsIds();
    MultiPageResult<std::vector<UserId>> getFriendsIds(UserId user_id);
    MultiPageResult<std::vector<UserId>> getFriendsIds(std::string const& user_name);
    pplx::task<std::tuple<Cursor, std::vector<UserId>>> getFriendsIds(std::string const& user_name, CursorId cursor_id);
    pplx::task<std::tuple<Cursor, std::vector<UserId>>> getFriendsIds(UserId user_id, CursorId cursor_id);

    MultiPageResult<std::vector<User>> getMyFriendsList();

    pplx::task<std::vector<Tweet>> getUserTimeline(UserId user_id, TweetId max_id = TweetId(0));

    pplx::task<std::vector<User>> getUsers(std::vector<UserId> const& user_ids);

    pplx::task<std::vector<Tweet>> getTweets(std::vector<TweetId> const& tweet_ids);

public:
    static pplx::task<void> checkConnectivity(ProxyConfig const& proxy_config,
                                              pplx::cancellation_token const& token = pplx::cancellation_token::none());

    typedef std::function<pplx::task<std::string>(std::string const&)> OAuthAuthenticationCallback;
    static pplx::task<OAuthCredentials> performOAuthAuthentication(ProxyConfig const& proxy_config,
                                                                   std::string const& consumer_key,
                                                                   std::string const& consumer_secret,
                                                                   OAuthAuthenticationCallback const& authenticate_cb);
};
}


#endif
