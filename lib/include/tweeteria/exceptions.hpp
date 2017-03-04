
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

class IOError : public std::runtime_error
{
public:
    IOError(char const* what) noexcept
        :std::runtime_error(what)
    {}
};

class APIProtocolViolation: public std::runtime_error
{
public:
    APIProtocolViolation(char const* what) noexcept
        :std::runtime_error(what)
    {}
};

class InvalidJSONFormat: public std::runtime_error
{
public:
    InvalidJSONFormat(char const* what) noexcept
        :std::runtime_error(what)
    {}
};
}

#endif
