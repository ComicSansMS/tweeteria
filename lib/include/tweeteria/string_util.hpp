
#ifndef TWEETERIA_INCLUDE_GUARD_STRING_UTIL_HPP
#define TWEETERIA_INCLUDE_GUARD_STRING_UTIL_HPP

#include <string>

namespace tweeteria
{
std::wstring convertUtf8ToUtf16(std::string const& utf8_str);
std::string convertUtf16ToUtf8(std::wstring const& utf16_str);
std::size_t lengthUtf8CodePoints(char const* utf8_str);
std::size_t lengthUtf8CodePoints(char const* utf8_str, std::size_t length_code_units);
std::size_t lengthUtf8CodePoints(std::string const& utf8_str);

template<class ForwardIt>
inline std::size_t lengthUtf8CodePoints(ForwardIt first, ForwardIt last)
{
    auto it = first;
    std::size_t count = 0;
    while(it != last) {
        if(((*it) & 0x80) == 0x00) {
            // single byte code point
            ++count;
            ++it;
        } else if(((*it) & 0xE0) == 0xC0) {
            // two byte code point
            if(((*(++it)) & 0xC0) != 0x80) { return 0xFFFFFFFF; }
            ++count;
            ++it;
        } else if(((*it) & 0xF0) == 0xE0) {
            // three byte code point
            if(((*(++it)) & 0xC0) != 0x80) { return 0xFFFFFFFF; }
            if(((*(++it)) & 0xC0) != 0x80) { return 0xFFFFFFFF; }
            ++count;
            ++it;
        } else if(((*it) & 0xF8) == 0xF0) {
            // four byte code point
            if(((*(++it)) & 0xC0) != 0x80) { return 0xFFFFFFFF; }
            if(((*(++it)) & 0xC0) != 0x80) { return 0xFFFFFFFF; }
            if(((*(++it)) & 0xC0) != 0x80) { return 0xFFFFFFFF; }
            ++count;
            ++it;
        } else if(((*it) & 0xC0) == 0x80) {
            // middle of a code point
            return 0xFFFFFFFF;
        } else {
            // invalid code unit
            return 0xFFFFFFFF;
        }
    }
    return count;
}
}

#endif
