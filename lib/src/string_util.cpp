
#include <tweeteria/string_util.hpp>

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

namespace {
class ZeroTermIterator
{
    friend bool operator!=(ZeroTermIterator lhs, ZeroTermIterator rhs);
private:
    char const* it_;
public:
    typedef typename std::iterator_traits<char const*>::difference_type difference_type;
    ZeroTermIterator()
        :it_(nullptr)
    {}
    ZeroTermIterator(char const* it)
        :it_(it)
    {}
    ZeroTermIterator operator++() {
        return ZeroTermIterator(++it_);
    }
    char operator*() {
        return *it_;
    }
};

bool operator!=(ZeroTermIterator lhs, ZeroTermIterator /*rhs*/)
{
    return (*lhs.it_) != '\0';
}
}

std::size_t lengthUtf8CodePoints(char const* utf8_str)
{
    return lengthUtf8CodePoints(ZeroTermIterator{utf8_str}, ZeroTermIterator{});
}

std::size_t lengthUtf8CodePoints(char const* utf8_str, std::size_t length_code_units)
{
    return lengthUtf8CodePoints(utf8_str, utf8_str + length_code_units);
}

std::size_t lengthUtf8CodePoints(std::string const& utf8_str)
{
    return lengthUtf8CodePoints(begin(utf8_str), end(utf8_str));
}
}
