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

#pragma once

#include <concepts>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

namespace infinity {
// using String = std::string;

template <typename T>
concept POD = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

template <typename T>
inline int32_t GetSizeInBytes(const T &) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    return sizeof(T);
}

template <>
inline int32_t GetSizeInBytes(const std::string &value) {
    return sizeof(int32_t) + value.length();
}

template <typename T>
inline T ReadBuf(char *const buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    return value;
}

template <typename T>
inline T ReadBufAdv(char *&buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    buf += sizeof(T);
    return value;
}

template <>
inline std::string ReadBuf<std::string>(char *const buf) {
    int32_t size = ReadBuf<int32_t>(buf);
    std::string str(buf + sizeof(int32_t), size);
    return str;
}

template <>
inline std::string ReadBufAdv<std::string>(char *&buf) {
    int32_t size = ReadBufAdv<int32_t>(buf);
    std::string str(buf, size);
    buf += size;
    return str;
}

template <typename T>
inline void WriteBuf(char *const buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
}

template <typename T>
inline void WriteBufAdv(char *&buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
    buf += sizeof(T);
}

template <>
inline void WriteBuf<std::string>(char *const buf, const std::string &value) {
    int32_t len = value.length();
    WriteBuf(buf, len);
    memcpy(buf + len, value.c_str(), len);
}

template <>
inline void WriteBufAdv<std::string>(char *&buf, const std::string &value) {
    int32_t len = value.length();
    WriteBufAdv(buf, len);
    memcpy(buf, value.c_str(), len);
    buf += len;
}

inline void WriteBufCharsAdv(char *&buf, const char *data, size_t bytes) {
    memcpy(buf, data, bytes);
    buf += bytes;
}

} // namespace infinity