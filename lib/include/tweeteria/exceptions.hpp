
#ifndef TWEETERIA_INCLUDE_GUARD_EXCEPTIONS_HPP
#define TWEETERIA_INCLUDE_GUARD_EXCEPTIONS_HPP

#include <stdexcept>

namespace tweeteria
{
class InvalidArgument : public std::invalid_argument
{
public:
    InvalidArgument(char const* what) noexcept
        :std::invalid_argument(what)
    {}
};
}

#endif
