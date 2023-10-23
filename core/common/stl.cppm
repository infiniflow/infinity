
module;

#include "ctpl_stl.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

    template <typename T>
    using Queue = std::queue<T>;

    template <typename S, typename T>
    using Map = std::map<S, T>;

    template <typename T>
    using Set = std::set<T>;

    template <typename S, typename T>
    using HashMap = std::unordered_map<S, T>;

    template <typename S>
    using HashSet = std::unordered_set<S>;

    template <typename T>
    using Optional = std::optional<T>;

    using StdOfStream = std::ofstream;

    // String

    using String = std::basic_string<char>;

    using StringView = std::string_view;

    inline bool IsEqual(const String &s1, const String &s2) { return s1 == s2; }

    inline bool IsEqual(const String &s1, const char *s2) { return s1 == s2; }

    inline String TrimPath(const String &path) {
        const auto pos = path.find("/src/");
        if (pos == String::npos)
            return path;
        return path.substr(pos + 1);
    }

    void ToUpper(String & str) { std::transform(str.begin(), str.end(), str.begin(), ::toupper); }
    int ToUpper(int c) { return ::toupper(c); }

    void ToLower(String & str) { std::transform(str.begin(), str.end(), str.begin(), ::tolower); }
    int ToLower(int c) { return ::tolower(c); }

    inline void StringToLower(String & str) {
        std::transform(str.begin(), str.end(), str.begin(), [](const auto c) { return std::tolower(c); });
    }

    const char *FromChars(const char *first, const char *last, unsigned long &value) {
        auto res = std::from_chars(first, last, value);
        if (res.ec == std::errc()) {
            return res.ptr;
        } else {
            return nullptr;
        }
    }

    template<typename T>
    inline const T& Min(const T& a, const T& b) {
        return std::min(a, b);
    }

    template<typename T>
    inline const T& Max(const T& a, const T& b) {
    return std::max(a, b);
    }

    // ToStr()

    _GLIBCXX_NODISCARD
    inline String ToStr(int __val)
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
    inline String ToStr(unsigned __val)
#if _GLIBCXX_USE_CXX11_ABI && (__CHAR_BIT__ * __SIZEOF_INT__) <= 32
        noexcept // any 32-bit value fits in the SSO buffer
#endif
    {
        String __str(std::__detail::__to_chars_len(__val), '\0');
        std::__detail::__to_chars_10_impl(&__str[0], __str.size(), __val);
        return __str;
    }

    _GLIBCXX_NODISCARD
    inline String ToStr(long __val)
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
    inline String ToStr(unsigned long __val)
#if _GLIBCXX_USE_CXX11_ABI && (__CHAR_BIT__ * __SIZEOF_LONG__) <= 32
        noexcept // any 32-bit value fits in the SSO buffer
#endif
    {
        String __str(std::__detail::__to_chars_len(__val), '\0');
        std::__detail::__to_chars_10_impl(&__str[0], __str.size(), __val);
        return __str;
    }

    _GLIBCXX_NODISCARD
    inline String ToStr(long long __val) {
        const bool __neg = __val < 0;
        const unsigned long long __uval = __neg ? (unsigned long long)~__val + 1ull : __val;
        const auto __len = std::__detail::__to_chars_len(__uval);
        String __str(__neg + __len, '-');
        std::__detail::__to_chars_10_impl(&__str[__neg], __len, __uval);
        return __str;
    }

    _GLIBCXX_NODISCARD
    inline String ToStr(unsigned long long __val) {
        String __str(std::__detail::__to_chars_len(__val), '\0');
        std::__detail::__to_chars_10_impl(&__str[0], __str.size(), __val);
        return __str;
    }

#if _GLIBCXX_USE_C99_STDIO
    // NB: (v)snprintf vs sprintf.

    _GLIBCXX_NODISCARD
    inline String ToStr(float __val) {
        const int __n = __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 20;
        return __gnu_cxx::__to_xstring<String>(&std::vsnprintf, __n, "%f", __val);
    }

    _GLIBCXX_NODISCARD
    inline String ToStr(double __val) {
        const int __n = __gnu_cxx::__numeric_traits<double>::__max_exponent10 + 20;
        return __gnu_cxx::__to_xstring<String>(&std::vsnprintf, __n, "%f", __val);
    }

    _GLIBCXX_NODISCARD
    inline String ToStr(long double __val) {
        const int __n = __gnu_cxx::__numeric_traits<long double>::__max_exponent10 + 20;
        return __gnu_cxx::__to_xstring<String>(&std::vsnprintf, __n, "%Lf", __val);
    }
#endif // _GLIBCXX_USE_C99_STDIO

    // stoi
    inline int StrToInt(const std::string &str, size_t *idx = 0, int base = 10) { return std::stoi(str, idx, base); }

    // StrToL
    inline long StrToL(const char *__restrict nptr, char **__restrict endptr, int base) { return std::strtol(nptr, endptr, base); }

    // StrToF
    inline float StrToF(const char *__restrict nptr, char **__restrict endptr) { return std::strtof(nptr, endptr); }

    // StrToD
    inline double StrToD(const char *__restrict nptr, char **__restrict endptr) { return std::strtod(nptr, endptr); }

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
    using ai64 = std::atomic_int64_t;
    using aptr = std::atomic_uintptr_t;
    using atomic_bool = std::atomic_bool;

    constexpr u64 u64_min = std::numeric_limits<u64>::min();
    constexpr i64 i64_min = std::numeric_limits<i64>::min();
    constexpr u32 u32_min = std::numeric_limits<u32>::min();
    constexpr i32 i32_min = std::numeric_limits<i32>::min();
    constexpr i16 i16_min = std::numeric_limits<i16>::min();
    constexpr u16 u16_min = std::numeric_limits<u16>::min();
    constexpr i8 i8_min = std::numeric_limits<i8>::min();
    constexpr u8 u8_min = std::numeric_limits<u8>::min();

    constexpr u64 u64_max = std::numeric_limits<u64>::max();
    constexpr i64 i64_max = std::numeric_limits<i64>::max();
    constexpr u32 u32_max = std::numeric_limits<u32>::max();
    constexpr i32 i32_max = std::numeric_limits<i32>::max();
    constexpr i16 i16_max = std::numeric_limits<i16>::max();
    constexpr u16 u16_max = std::numeric_limits<u16>::max();
    constexpr i8 i8_max = std::numeric_limits<i8>::max();
    constexpr u8 u8_max = std::numeric_limits<u8>::max();

    constexpr f32 f32_inf = std::numeric_limits<f32>::infinity();
    constexpr f32 f32_min = std::numeric_limits<f32>::min();
    constexpr f32 f32_max = std::numeric_limits<f32>::max();
    constexpr f64 f64_inf = std::numeric_limits<f64>::infinity();
    constexpr f64 f64_min = std::numeric_limits<f64>::min();
    constexpr f64 f64_max = std::numeric_limits<f64>::max();

    constexpr u64 u64_inf = std::numeric_limits<u64>::infinity();
    constexpr i64 i64_inf = std::numeric_limits<i64>::infinity();
    constexpr u32 u32_inf = std::numeric_limits<u32>::infinity();
    constexpr i32 i32_inf = std::numeric_limits<i32>::infinity();
    constexpr i16 i16_inf = std::numeric_limits<i16>::infinity();
    constexpr u16 u16_inf = std::numeric_limits<u16>::infinity();
    constexpr i8 i8_inf = std::numeric_limits<i8>::infinity();
    constexpr u8 u8_inf = std::numeric_limits<u8>::infinity();

    constexpr ptr_t ptr_inf = std::numeric_limits<ptr_t>::infinity();
    constexpr u64 *u64_ptr_inf = std::numeric_limits<u64 *>::infinity();

    template <typename T>
    using Atomic = std::atomic<T>;

    // Smart ptr

    template <typename T>
    using SharedPtr = std::shared_ptr<T>;

    template <typename T, typename... Args>
    inline SharedPtr<T> MakeShared(Args && ...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using UniquePtr = std::unique_ptr<T>;

    template <typename T, typename... Args>
    inline UniquePtr<T> MakeUnique(Args && ...args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    // DB Type

    using ColumnID = u32;

    // Exception

    using StlException = std::exception;

    // Move
    template <typename _Tp>
    _GLIBCXX_NODISCARD constexpr typename std::remove_reference<_Tp>::type &&Move(_Tp && __t) noexcept {
        return static_cast<typename std::remove_reference<_Tp>::type &&>(__t);
    }

    // Forward
    template <typename _Tp>
    _GLIBCXX_NODISCARD constexpr _Tp &&Forward(typename std::remove_reference<_Tp>::type & __t) noexcept {
        return static_cast<_Tp &&>(__t);
    }

    // Chrono
    using Clock = std::chrono::high_resolution_clock;

    template <typename T>
    using TimePoint = std::chrono::time_point<T, std::chrono::nanoseconds>;

    using NanoSeconds = std::chrono::nanoseconds;
    using MicroSeconds = std::chrono::microseconds;
    using MilliSeconds = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;

    inline NanoSeconds ElapsedFromStart(const TimePoint<Clock> &end, const TimePoint<Clock> &start) { return end - start; }

    template <typename T>
    T ChronoCast(const NanoSeconds &nano_seconds) {
        return std::chrono::duration_cast<T>(nano_seconds);
    }

    // Memcpy
    void *Memcpy(void *__restrict dest, const void *__restrict src, size_t n) { return memcpy(dest, src, n); }
    void *Memset(void *__restrict dest, int value, size_t n) { return memset(dest, value, n); }

    // Memcmp
    int Memcmp(const void *__restrict s1, const void *__restrict s2, size_t n) { return memcmp(s1, s2, n); }

    // IsStandLayout
    template <typename T>
    concept IsStandLayout = std::is_standard_layout_v<T>;

    template <typename T>
    concept IsTrivial = std::is_trivial_v<T>;

    // Mutex
    template <typename T>
    using SharedLock = std::shared_lock<T>;

    template <typename T>
    using UniqueLock = std::unique_lock<T>;

    template <typename T>
    using LockGuard = std::lock_guard<T>;

    constexpr std::memory_order MemoryOrderRelax = std::memory_order::relaxed;

    using CondVar = std::condition_variable;

    // Stringstream
    using StringStream = std::basic_stringstream<char>;

    // Endl;
    template <typename _CharT, typename _Traits>
    inline std::basic_ostream<_CharT, _Traits> &Endl(std::basic_ostream<_CharT, _Traits> & __os) {
        return std::flush(__os.put(__os.widen('\n')));
    }

    // Dir
    using Path = std::filesystem::path;
    using DirEntry = std::filesystem::directory_entry;

    inline Vector<String> GetFilesFromDir(const String &path) {
        Vector<String> result;
        for (auto &i : std::filesystem::directory_iterator(path)) {
            result.emplace_back(i.path().string());
        }
        return result;
    }

    // typeid
    //    using TypeID = std::typeid();

    // std::function
    template <typename T>
    using StdFunction = std::function<T>;

    // SharedPtr
    template <typename T>
    using EnableSharedFromThis = std::enable_shared_from_this<T>;

    // static ptr cast
    template<typename _Tp, typename _Tp1, std::_Lock_policy _Lp>
    inline std::__shared_ptr<_Tp, _Lp>
    StaticPointCast(const std::__shared_ptr<_Tp1, _Lp>& __r) noexcept
    {
        using _Sp = std::__shared_ptr<_Tp, _Lp>;
        return _Sp(__r, static_cast<typename _Sp::element_type*>(__r.get()));
    }

    using Mutex = std::mutex;

    float HugeValf() {
        return HUGE_VALF;
    }
}

} // namespace infinity
