
#ifndef TWEETERIA_INCLUDE_GUARD_TWEETERIA_HPP
#define TWEETERIA_INCLUDE_GUARD_TWEETERIA_HPP

#include <tweeteria/cursor.hpp>
#include <tweeteria/error.hpp>
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

    pplx::task<std::vector<User>> getUsers(std::vector<UserId> const& user_ids);
};

std::vector<User> json_test();
std::vector<Tweet> json_test_tweets();
}


#endif
