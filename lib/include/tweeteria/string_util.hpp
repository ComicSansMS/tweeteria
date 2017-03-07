
#ifndef TWEETERIA_INCLUDE_GUARD_STRING_UTIL_HPP
#define TWEETERIA_INCLUDE_GUARD_STRING_UTIL_HPP

#include <tweeteria/exceptions.hpp>

#include <string>

namespace tweeteria
{
std::wstring convertUtf8ToUtf16(std::string const& utf8_str);
std::string convertUtf16ToUtf8(std::wstring const& utf16_str);
std::size_t lengthUtf8CodePoints(char const* utf8_str);
std::size_t lengthUtf8CodePoints(char const* utf8_str, std::size_t length_code_units);
std::size_t lengthUtf8CodePoints(std::string const& utf8_str);

template<class ForwardIt>
inline ForwardIt avanceUtf8CodePoints(ForwardIt first, ForwardIt last, std::size_t n_code_points)
{
    auto it = first;
    for(auto count = 0; count < n_code_points; ++count) {
        if(((*it) & 0x80) == 0x00) {
            // single byte code point
            ++it;
        } else if(((*it) & 0xE0) == 0xC0) {
            // two byte code point
            ++it;
            ++it;
        } else if(((*it) & 0xF0) == 0xE0) {
            // three byte code point
            ++it;
            ++it;
            ++it;
        } else if(((*it) & 0xF8) == 0xF0) {
            // four byte code point
            ++it;
            ++it;
            ++it;
            ++it;
        } else if(((*it) & 0xC0) == 0x80) {
            // middle of a code point
            throw InvalidArgument("Malformed UTF8: Orphaned code point tail.");
        } else {
            // invalid code unit
            throw InvalidArgument("Malformed UTF8: Invalid code unit.");
        }
    }
    return it;
}

template<class ForwardIt>
inline std::size_t lengthUtf8CodePoints(ForwardIt first, ForwardIt last)
{
    auto it = first;
    std::size_t count = 0;
    auto advance_and_check = [last](ForwardIt& it) {
        ++it;
        if(it == last) {
            throw InvalidArgument("UTF8 string truncated in the middle of a code point.");
        }
        if(((*it) & 0xC0) != 0x80) {
            throw InvalidArgument("Malformed UTF8: Code point too short.");
        }
    };
    while(it != last) {
        if(((*it) & 0x80) == 0x00) {
            // single byte code point
            ++count;
            ++it;
        } else if(((*it) & 0xE0) == 0xC0) {
            // two byte code point
            advance_and_check(it);
            ++count;
            ++it;
        } else if(((*it) & 0xF0) == 0xE0) {
            // three byte code point
            advance_and_check(it);
            advance_and_check(it);
            ++count;
            ++it;
        } else if(((*it) & 0xF8) == 0xF0) {
            // four byte code point
            advance_and_check(it);
            advance_and_check(it);
            advance_and_check(it);
            ++count;
            ++it;
        } else if(((*it) & 0xC0) == 0x80) {
            // middle of a code point
            throw InvalidArgument("Malformed UTF8: Orphaned code point tail.");
        } else {
            // invalid code unit
            throw InvalidArgument("Malformed UTF8: Invalid code unit.");
        }
    }
    return count;
}
}

#endif
