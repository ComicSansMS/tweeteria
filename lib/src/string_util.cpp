
#include <tweeteria/user.hpp>

#include <codecvt>
#include <locale>

namespace tweeteria
{
std::wstring convertUtf8ToUtf16(std::string const& utf8_str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8_str);
}

std::string convertUtf16ToUtf8(std::wstring const& utf16_str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(utf16_str);
}
}
