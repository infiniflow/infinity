//
// Created by JinHai on 2022/10/27.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace infinity {

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template <class T, typename... Args>
inline SharedPtr<T> MakeShared(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template <class T, typename... Args>
inline UniquePtr<T> MakeUnique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}


template<typename T>
using Vector = std::vector<T>;

using String = std::string;

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

using ptr_t = char *;
using const_ptr_t = const char *;
using char_t = char;
}
