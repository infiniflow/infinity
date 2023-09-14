//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include "ctpl_stl.h"

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <cstring>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <thread>
#include <shared_mutex>

namespace infinity {

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Vector = std::vector<T>;

template<typename T>
using List = std::list<T>;

template<typename T>
using Set = std::set<T>;

template<typename S, typename T>
using HashMap = std::unordered_map<S, T>;

template<typename S>
using HashSet = std::unordered_set<S>;

using RWMutex = std::shared_mutex;

using ThreadPool = ctpl::thread_pool;

using String = std::string;
using StringView = std::string_view;

using Byte = std::byte;

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

using au64 = std::atomic_uint64_t;
using aptr = std::atomic_uintptr_t;

using ptr_t = char *;
using const_ptr_t = const char *;
using char_t = char;
using SizeT = size_t;

using Thread = std::thread;

using TxnTimeStamp = u64;

struct RowID {
    RowID(u32 blk, u32 off) : block(blk), offset(off) {}
    u32 block {};
    u32 offset {};
};

using ColumnID = u32;

}
