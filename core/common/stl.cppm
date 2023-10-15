
module;

#include "ctpl_stl.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

export module stl;

namespace infinity {

export {

// Containers

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template <typename T, std::size_t N>
using Array = std::array<T, N>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using List = std::list<T>;

template <typename S, typename T>
using Map = std::map<S, T>;

template <typename T>
using Set = std::set<T>;

template <typename S, typename T>
using HashMap = std::unordered_map<S, T>;

template <typename S>
using HashSet = std::unordered_set<S>;

// String

using String = std::basic_string<char>;

// ToString()

_GLIBCXX_NODISCARD
inline String ToString(int __val)
#if _GLIBCXX_USE_CXX11_ABI && (__CHAR_BIT__ * __SIZEOF_INT__) <= 32
noexcept // any 32-bit value fits in the SSO buffer
#endif
{
    const bool __neg = __val < 0;
    const unsigned __uval = __neg ? (unsigned)~__val + 1u : __val;
    const auto __len = std::__detail::__to_chars_len(__uval);
    String __str(__neg + __len, '-');
    std::__detail::__to_chars_10_impl(&__str[__neg], __len, __uval);
    return __str;
}

_GLIBCXX_NODISCARD
inline String ToString(unsigned __val)
#if _GLIBCXX_USE_CXX11_ABI && (__CHAR_BIT__ * __SIZEOF_INT__) <= 32
noexcept // any 32-bit value fits in the SSO buffer
#endif
{
    String __str(std::__detail::__to_chars_len(__val), '\0');
    std::__detail::__to_chars_10_impl(&__str[0], __str.size(), __val);
    return __str;
}

_GLIBCXX_NODISCARD
inline String ToString(long __val)
#if _GLIBCXX_USE_CXX11_ABI && (__CHAR_BIT__ * __SIZEOF_LONG__) <= 32
noexcept // any 32-bit value fits in the SSO buffer
#endif
{
    const bool __neg = __val < 0;
    const unsigned long __uval = __neg ? (unsigned long)~__val + 1ul : __val;
    const auto __len = std::__detail::__to_chars_len(__uval);
    String __str(__neg + __len, '-');
    std::__detail::__to_chars_10_impl(&__str[__neg], __len, __uval);
    return __str;
}

_GLIBCXX_NODISCARD
inline String ToString(unsigned long __val)
#if _GLIBCXX_USE_CXX11_ABI && (__CHAR_BIT__ * __SIZEOF_LONG__) <= 32
noexcept // any 32-bit value fits in the SSO buffer
#endif
{
    String __str(std::__detail::__to_chars_len(__val), '\0');
    std::__detail::__to_chars_10_impl(&__str[0], __str.size(), __val);
    return __str;
}

_GLIBCXX_NODISCARD
inline String ToString(long long __val) {
    const bool __neg = __val < 0;
    const unsigned long long __uval = __neg ? (unsigned long long)~__val + 1ull : __val;
    const auto __len = std::__detail::__to_chars_len(__uval);
    String __str(__neg + __len, '-');
    std::__detail::__to_chars_10_impl(&__str[__neg], __len, __uval);
    return __str;
}

_GLIBCXX_NODISCARD
inline String ToString(unsigned long long __val) {
    String __str(std::__detail::__to_chars_len(__val), '\0');
    std::__detail::__to_chars_10_impl(&__str[0], __str.size(), __val);
    return __str;
}

#if _GLIBCXX_USE_C99_STDIO
// NB: (v)snprintf vs sprintf.

_GLIBCXX_NODISCARD
inline String ToString(float __val) {
    const int __n = __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<String>(&std::vsnprintf, __n, "%f", __val);
}

_GLIBCXX_NODISCARD
inline String ToString(double __val) {
    const int __n = __gnu_cxx::__numeric_traits<double>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<String>(&std::vsnprintf, __n, "%f", __val);
}

_GLIBCXX_NODISCARD
inline String ToString(long double __val) {
    const int __n = __gnu_cxx::__numeric_traits<long double>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<String>(&std::vsnprintf, __n, "%Lf", __val);
}
#endif // _GLIBCXX_USE_C99_STDIO

// Primitives

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using idx_t = u64;

using f32 = float;
using f64 = double;

using offset_t = int64_t;

using ptr_t = char *;
using const_ptr_t = const char *;
using char_t = char;
using SizeT = size_t;

using TxnTimeStamp = u64;

// Concurrency

using RWMutex = std::shared_mutex;
using ThreadPool = ctpl::thread_pool;

using Thread = std::thread;

using au64 = std::atomic_uint64_t;
using aptr = std::atomic_uintptr_t;
using atomic_bool = std::atomic_bool;

constexpr u64 u64_max = std::numeric_limits<u64>::max();
constexpr u64 i64_max = std::numeric_limits<i64>::max();
constexpr u64 u32_max = std::numeric_limits<u32>::max();
constexpr u64 i32_max = std::numeric_limits<i32>::max();
constexpr u64 i16_max = std::numeric_limits<i16>::max();
constexpr u64 u16_max = std::numeric_limits<u16>::max();

// Smart ptr

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// DB Type

using ColumnID = u32;

}

} // namespace infinity
