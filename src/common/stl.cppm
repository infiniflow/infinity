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
#include <bit>
#include <bitset>
#include <cassert>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <forward_list>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <ranges>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

export module infinity_core:stl;

export namespace std {

using std::source_location;

using std::monostate;
using std::nullptr_t;

// using std::stringstream;
using std::exchange;
using std::forward;
using std::move;
using std::swap;

using std::max;
using std::min;

using std::errc;
using std::error_code;
using std::from_chars;
using std::to_string;

using std::stoi;
using std::stol;
using std::stoll;
using std::stoull;
using std::strtod;
using std::strtof;
using std::strtol;

using std::bit_cast;
using std::memcmp;
using std::memcpy;
using std::memset;
using std::strcmp;
using std::strlen;

using std::fprintf;
using std::printf;
using std::sprintf;
using std::time;

using std::fill;
using std::is_same;
using std::lower_bound;
using std::upper_bound;

using std::atomic_flag;
using std::binary_semaphore;
using std::condition_variable;
using std::counting_semaphore;
using std::lock_guard;
using std::memory_order;
using std::memory_order_acq_rel;
using std::memory_order_acquire;
using std::memory_order_consume;
using std::memory_order_relaxed;
using std::memory_order_release;
using std::memory_order_seq_cst;
using std::mutex;
using std::scoped_lock;
using std::shared_lock;
using std::shared_mutex;
using std::unique_lock;

using std::adopt_lock;
using std::defer_lock;
using std::try_to_lock;

using std::accumulate;
using std::binary_search;
using std::bitset;
using std::ceil;
using std::copy_n;
using std::fabs;
using std::fill_n;
using std::find;
using std::fixed;
using std::floor;
using std::fmod;
using std::forward_list;
using std::isalnum;
using std::isalpha;
using std::isinf;
using std::isnan;
using std::log2;
using std::make_heap;
using std::max_element;
using std::min_element;
using std::nearbyint;
using std::partial_sort;
using std::pop_heap;
using std::pow;
using std::push_heap;
using std::reduce;
using std::remove_if;
using std::reverse;
using std::setprecision;
using std::sort;
using std::sqrt;
using std::stable_sort;
using std::tie;
using std::transform;
using std::unique;

using std::string;
using std::stringstream;

namespace ranges {

using std::ranges::equal;
using std::ranges::for_each;

} // namespace ranges

using std::decay_t;
using std::function;
using std::numeric_limits;

namespace chrono {
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

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

using std::chrono::ceil;
using std::chrono::days;

using std::chrono::day;
using std::chrono::month;
using std::chrono::year;

using std::chrono::sys_days;
using std::chrono::system_clock;
using std::chrono::year_month_day;

using std::chrono::high_resolution_clock;
} // namespace chrono

using std::cerr;
using std::cout;
using std::endl;
using std::format;

using std::hex;
using std::setfill;
using std::setw;

using std::fstream;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::ostream;

using std::endian;

using std::align;

using std::ptrdiff_t;

using std::dynamic_pointer_cast;
using std::static_pointer_cast;

namespace filesystem {
using std::filesystem::absolute;
using std::filesystem::canonical;
using std::filesystem::copy;
using std::filesystem::copy_file;
using std::filesystem::copy_options;
using std::filesystem::create_directories;
using std::filesystem::directory_iterator;
using std::filesystem::directory_options;
using std::filesystem::exists;
using std::filesystem::file_size;
using std::filesystem::filesystem_error;
using std::filesystem::path;
using std::filesystem::read_symlink;
using std::filesystem::relative;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::rename;
using std::filesystem::resize_file;

using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::is_symlink;
using std::filesystem::recursive_directory_iterator;
} // namespace filesystem

namespace this_thread {
using std::this_thread::sleep_for;
}

using std::iota;
using std::mt19937;
using std::random_device;
using std::uniform_int_distribution;
using std::uniform_real_distribution;

using std::array;
using std::exception;
using std::unordered_set;

using std::back_inserter;
using std::distance;
using std::hash;

using std::streamsize;
using std::string_view;

using std::get;
using std::holds_alternative;
using std::variant;
using std::variant_size_v;
using std::visit;

using std::common_type_t;
using std::conditional_t;
using std::derived_from;
using std::invocable;
using std::is_floating_point_v;
using std::is_integral_v;
using std::remove_pointer_t;
using std::remove_reference_t;
using std::underlying_type_t;
using std::unsigned_integral;

using std::function;
using std::monostate;
using std::thread;

using std::is_same_v;
using std::priority_queue;

using std::begin;
using std::end;

using std::asctime;
using std::localtime;
using std::time_t;

using std::stod;
using std::stof;
using std::strtoull;

using std::construct_at;

using std::set;

using std::all_of;
using std::any_of;
using std::none_of;

using std::memset;
using std::strncmp;

// using std::literals;
using std::localtime;
using std::put_time;

using std::views::keys;
using std::views::values;

using std::greater;
using std::less;

using std::make_move_iterator;
} // namespace std

namespace infinity {

export {

    // Containers
    //
    // template <typename T>
    // using Queue = std::queue<T>;
    //
    // template <typename T>
    // using Hash = std::hash<T>;
    //
    // template <typename T>
    // struct EqualTo {
    //     bool operator()(const T &left, const T &right) const { return left == right; }
    // };
    //
    // template <typename S, typename T, typename H = std::hash<S>>
    // using MultiHashMap = std::unordered_multimap<S, T, H>;
    //
    // template <typename T>
    // using MaxHeap = std::priority_queue<T>;

    using NoneType = std::nullopt_t;

    // std::string

    inline bool IsEqual(const std::string &s1, const std::string &s2) { return s1 == s2; }

    inline bool IsEqual(const std::string &s1, const char *s2) { return s1 == s2; }

    inline std::string TrimPath(const std::string &path) {
        const auto pos = path.find("/src/");
        if (pos == std::string::npos)
            return path;
        return path.substr(pos + 1);
    }

    inline std::string TrimString(const std::string &s) {
        int len = s.length();
        int i = 0;

        while (i < len && isspace(s[i])) {
            i++;
        }

        while (len > i && isspace(s[len - 1])) {
            len--;
        }

        if (i == len) {
            return "";
        }

        std::string ss = s.substr(i, len - i);
        return ss;
    }

    std::vector<std::string> SplitStrByComma(std::string str) {
        std::vector<std::string> tokens;
        for (const auto &token : str | std::views::split(',')) {
            tokens.emplace_back(token.begin(), token.end());
        }

        for (auto &s : tokens) {
            s = TrimString(s);
        }

        return tokens;
    }

    void ToUpper(std::string & str) { std::transform(str.begin(), str.end(), str.begin(), ::toupper); }

    int ToUpper(int c) { return ::toupper(c); }

    void ToLower(std::string & str) { std::transform(str.begin(), str.end(), str.begin(), ::tolower); }

    int ToLower(int c) { return ::tolower(c); }

    inline void StringToLower(std::string & str) {
        std::transform(str.begin(), str.end(), str.begin(), [](const auto c) { return std::tolower(c); });
    }

    template <class BidirIteratorType>
    BidirIteratorType Prev(BidirIteratorType it, typename std::iterator_traits<BidirIteratorType>::difference_type n = 1) {
        std::advance(it, -n);
        return it;
    }

    template <class BidirIteratorType>
    BidirIteratorType Next(BidirIteratorType it, typename std::iterator_traits<BidirIteratorType>::difference_type n = 1) {
        std::advance(it, n);
        return it;
    }

    using offset_t = int64_t;

    using char_t = char;
    using uintptr_t = std::uintptr_t;

    // Concurrency
    using ThreadPool = ctpl::thread_pool;

    using atomic_u32 = std::atomic_uint32_t;
    using atomic_u64 = std::atomic_uint64_t;
    using ai64 = std::atomic_int64_t;
    using aptr = std::atomic_uintptr_t;
    using atomic_bool = std::atomic_bool;

    using std::atomic_compare_exchange_strong;
    using std::atomic_store;

    // Smart ptr

    template <typename T, typename U>
    inline constexpr auto MakePair(T && first, U && second) {
        return std::make_pair(std::forward<T>(first), std::forward<U>(second));
    }

    template <typename T>
    inline constexpr std::optional<T> MakeOptional(T && value) {
        return std::make_optional<T>(std::forward<T>(value));
    }

    template <typename T>
    using TimePoint = std::chrono::time_point<T, std::chrono::nanoseconds>;

    using Seconds = std::chrono::seconds;

    inline std::chrono::nanoseconds ElapsedFromStart(const TimePoint<std::chrono::high_resolution_clock> &end,
                                                     const TimePoint<std::chrono::high_resolution_clock> &start) {
        return end - start;
    }

    template <typename T>
    T ChronoCast(const std::chrono::nanoseconds &nano_seconds) {
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

    inline std::vector<std::string> GetFilesFromDir(const std::string &path) {
        std::vector<std::string> result;
        for (auto &i : std::filesystem::directory_iterator(path)) {
            result.emplace_back(i.path().string());
        }
        return result;
    }

    // typeid
    //    using TypeID = std::typeid();

    // std::function
    //    template<typename R, typename... Ts>
    //    using std::function = std::function<R, Ts>;

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
