// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "ctpl_stl.h"
#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <experimental/source_location>
#include <filesystem>
#include <forward_list>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

export module stl;

export namespace std {

using std::experimental::source_location;
// using std::stringstream;
using std::forward;
using std::move;

using std::swap;

using std::max;
using std::min;

using std::to_string;
using std::from_chars;
using std::errc;

using std::stoi;
using std::strtol;
using std::strtof;
using std::strtod;

using std::memcpy;
using std::memset;
using std::memcmp;
using std::strlen;

using std::time;
using std::printf;

using std::is_same;
using std::fill;
using std::lower_bound;

using std::condition_variable;
using std::lock_guard;
using std::memory_order;
using std::memory_order_acq_rel;
using std::memory_order_acquire;
using std::memory_order_consume;
using std::memory_order_relaxed;
using std::memory_order_release;
using std::memory_order_seq_cst;
using std::mutex;
using std::shared_lock;
using std::shared_mutex;
using std::unique_lock;

using std::forward_list;
using std::isalpha;
using std::isalnum;
using std::pow;
using std::log2;
using std::sqrt;
using std::floor;
using std::nearbyint;
using std::isnan;
using std::isinf;
using std::fmod;
using std::fabs;
using std::fill_n;
using std::transform;
using std::sort;
using std::make_heap;
using std::pop_heap;
using std::reverse;
using std::remove_if;
using std::find;

namespace ranges {

using std::ranges::for_each;
using std::ranges::equal;

}

using std::decay_t;
using std::function;
using std::numeric_limits;

namespace chrono {
using std::chrono::duration;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using std::chrono::operator>;
using std::chrono::operator>=;
using std::chrono::operator<;
using std::chrono::operator<=;
using std::chrono::operator==;

using std::chrono::operator+;
using std::chrono::operator-;

using std::chrono::minutes;
using std::chrono::weeks;
using std::chrono::years;

using std::chrono::steady_clock;
using std::chrono::time_point;
} // namespace chrono

using std::cout;
using std::cerr;
using std::endl;

using std::ostream;
using std::ofstream;
using std::ifstream;
using std::ios;

using std::align;

using std::ptrdiff_t;

using std::static_pointer_cast;
using std::dynamic_pointer_cast;

namespace filesystem {
    using std::filesystem::file_size;
}

using std::mt19937;
using std::uniform_real_distribution;
using std::iota;

using std::exception;
using std::unordered_set;

using std::back_inserter;
using std::hash;

using std::string_view;
using std::streamsize;

using std::variant;
using std::holds_alternative;
using std::get;
using std::visit;

using std::is_integral_v;
using std::is_floating_point_v;

using std::monostate;

} // namespace std

namespace infinity {

export {

    // Containers

    template <typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    template <typename... T>
    using Tuple = std::tuple<T...>;

    template <typename T, std::size_t N>
    using Array = std::array<T, N>;

    template <typename T, typename Allocator = std::allocator<T>>
    using Vector = std::vector<T, Allocator>;

    template <typename T>
    using Deque = std::deque<T>;

    template <typename T>
    using List = std::list<T>;

    template <typename T>
    using Queue = std::queue<T>;

    template <typename S, typename T>
    using Map = std::map<S, T>;

    template <typename S, typename T>
    using MultiMap = std::multimap<S, T>;

    template <typename T>
    using Set = std::set<T>;

    template<typename T>
    using Hash = std::hash<T>;

    template <typename S, typename T, typename H = std::hash<S>, typename Eq = std::equal_to<S>>
    using HashMap = std::unordered_map<S, T, H, Eq>;

    template <typename S, typename T, typename H = std::hash<S>>
    using MultiHashMap = std::unordered_multimap<S, T, H>;

    template <typename S>
    using HashSet = std::unordered_set<S>;

    template <typename T>
    using MaxHeap = std::priority_queue<T>;

    template <typename T, typename C>
    using Heap = std::priority_queue<T, std::vector<T>, C>;

    template <typename T>
    using Optional = std::optional<T>;
    constexpr std::nullopt_t None = std::nullopt;

    using NoneType = std::nullopt_t;

    using StdOfStream = std::ofstream;

    // String

    using String = std::basic_string<char>;

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

    template<class BidirIteratorType>
    BidirIteratorType Prev(BidirIteratorType it, typename std::iterator_traits<BidirIteratorType>::difference_type n = 1) {
        std::advance(it, -n);
        return it;
    }

    template<class BidirIteratorType>
    BidirIteratorType Next(BidirIteratorType it, typename std::iterator_traits<BidirIteratorType>::difference_type n = 1) {
        std::advance(it, n);
        return it;
    }

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
    using SizeT = u64;

    // Transactions
    using TxnTimeStamp = uint64_t;
    using TransactionID = uint64_t;

    // Entry
    using SegmentID = uint32_t;
    using BlockID = uint16_t;
    using ColumnID = uint64_t;

    // Related to entry
    using BlockOffset = uint16_t;
    using SegmentOffset = uint32_t;

    // Concurrency
    using ThreadPool = ctpl::thread_pool;

    using Thread = std::thread;

    using atomic_u32 = std::atomic_uint32_t;
    using atomic_u64 = std::atomic_uint64_t;
    using ai64 = std::atomic_int64_t;
    using aptr = std::atomic_uintptr_t;
    using atomic_bool = std::atomic_bool;
    using atomic_flag = std::atomic_flag;

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

    template <typename T, typename... Args>
    inline UniquePtr<T> MakeUniqueForOverwrite(Args && ...args) {
        return std::make_unique_for_overwrite<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename U>
    inline constexpr Pair<T, U> MakePair(T && first, U && second) {
        return std::make_pair<T, U>(std::forward<T>(first), std::forward<U>(second));
    }

    template<typename T>
    inline constexpr Optional<T> MakeOptional(T && value) {
        return std::make_optional<T>(std::forward<T>(value));
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


    // IsStandLayout
    template <typename T>
    concept IsStandLayout = std::is_standard_layout_v<T>;

    // Stringstream
    using IStringStream = std::istringstream;
    using OStringStream = std::ostringstream;

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

    template <typename II, typename OI>
    OI Copy(II first, II last, OI d_first) {
        return std::copy(first, last, d_first);
    }
}

export template <typename T1, typename T2>
struct CompareByFirst {
    using P = std::pair<T1, T2>;
    bool operator()(const P &lhs, const P &rhs) const { return lhs.first < rhs.first; }
};

export template <typename T1, typename T2>
struct CompareByFirstReverse {
    using P = std::pair<T1, T2>;
    bool operator()(const P &lhs, const P &rhs) const { return lhs.first > rhs.first; }
};

} // namespace infinity
