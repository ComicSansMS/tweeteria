
#ifndef TWEETERIA_INCLUDE_GUARD_STRING_UTIL_HPP
#define TWEETERIA_INCLUDE_GUARD_STRING_UTIL_HPP

#include <string>

namespace tweeteria
{
std::wstring convertUtf8ToUtf16(std::string const& utf8_str);
std::string convertUtf16ToUtf8(std::wstring const& utf16_str);
}

#endif
