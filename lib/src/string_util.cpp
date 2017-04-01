
#include <tweeteria/string_util.hpp>

#include <codecvt>
#include <istream>
#include <locale>
#include <ostream>
#include <vector>

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
    friend bool operator==(ZeroTermIterator lhs, ZeroTermIterator rhs);
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

bool operator==(ZeroTermIterator lhs, ZeroTermIterator /*rhs*/)
{
    return (*lhs.it_) == '\0';
}

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

void serialize_string(std::string const& str, std::ostream& os)
{
    std::uint64_t const string_size = str.length();
    os.write(reinterpret_cast<char const*>(&string_size), sizeof(string_size));
    os.write(str.c_str(), string_size);
    if(!os) { throw tweeteria::IOError("Error while serializing string."); }
}

std::string deserialize_string(std::istream& is)
{
    std::uint64_t string_size;
    is.read(reinterpret_cast<char*>(&string_size), sizeof(string_size));
    std::vector<char> buffer;
    buffer.resize(string_size, '0');
    is.read(buffer.data(), buffer.size());
    if(!is) { throw tweeteria::IOError("Error while deserializing string."); }
    return std::string(begin(buffer), end(buffer));
}
}
