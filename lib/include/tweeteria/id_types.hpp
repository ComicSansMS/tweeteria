
#ifndef TWEETERIA_INCLUDE_GUARD_ID_TYPES_HPP
#define TWEETERIA_INCLUDE_GUARD_ID_TYPES_HPP

#include <cstdint>
#include <functional>

namespace tweeteria
{
template<typename Id_T, typename Tag_T>
struct WrappedId {
    typedef Id_T IdType;
    Id_T id;

    WrappedId() = default;

    explicit WrappedId(IdType arg_id)
        :id (arg_id)
    {}

    WrappedId(WrappedId const&) = default;
    WrappedId& operator=(WrappedId const&) = default;
    WrappedId(WrappedId&&) = default;
    WrappedId& operator=(WrappedId&&) = default;
};

template<typename Id_T, typename Tag_T>
inline bool operator==(WrappedId<Id_T, Tag_T> const& lhs, WrappedId<Id_T, Tag_T> const& rhs)
{
    return lhs.id == rhs.id;
}

template<typename Id_T, typename Tag_T>
inline bool operator!=(WrappedId<Id_T, Tag_T> const& lhs, WrappedId<Id_T, Tag_T> const& rhs)
{
    return lhs.id != rhs.id;
}

template<typename Id_T, typename Tag_T>
inline bool operator<(WrappedId<Id_T, Tag_T> const& lhs, WrappedId<Id_T, Tag_T> const& rhs)
{
    return lhs.id < rhs.id;
}

template<typename Id_T, typename Tag_T>
inline bool operator>(WrappedId<Id_T, Tag_T> const& lhs, WrappedId<Id_T, Tag_T> const& rhs)
{
    return lhs.id > rhs.id;
}

template<typename Id_T, typename Tag_T>
inline bool operator<=(WrappedId<Id_T, Tag_T> const& lhs, WrappedId<Id_T, Tag_T> const& rhs)
{
    return lhs.id <= rhs.id;
}

template<typename Id_T, typename Tag_T>
inline bool operator>=(WrappedId<Id_T, Tag_T> const& lhs, WrappedId<Id_T, Tag_T> const& rhs)
{
    return lhs.id >= rhs.id;
}
}

namespace std
{
template<typename Id_T, typename Tag_T> struct hash<tweeteria::WrappedId<Id_T, Tag_T>>
{
    typedef tweeteria::WrappedId<Id_T, Tag_T> argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        return std::hash<typename argument_type::IdType>{}(s.id);
    }
};
}

namespace tweeteria
{
namespace id_tags
{
struct UserId_Tag {};
struct TweetId_Tag {};
struct CursorId_Tag {};
struct MediaId_Tag {};
}
typedef WrappedId<std::uint64_t, id_tags::UserId_Tag> UserId;
typedef WrappedId<std::uint64_t, id_tags::TweetId_Tag> TweetId;
typedef WrappedId<std::int64_t, id_tags::CursorId_Tag> CursorId;
typedef WrappedId<std::uint64_t, id_tags::MediaId_Tag> MediaId;
}
#endif
