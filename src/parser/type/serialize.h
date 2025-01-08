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

template <typename T>
inline T ReadBuf(const char *buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T value;
    std::memcpy(&value, buf, sizeof(T));
    return value;
}

template <typename T>
inline T ReadBufAdv(const char *&buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T value;
    std::memcpy(&value, buf, sizeof(T));
    buf += sizeof(T);
    return value;
}

template <>
inline std::string ReadBuf<std::string>(const char *buf) {
    int32_t size = ReadBuf<int32_t>(buf);
    std::string str(buf + sizeof(int32_t), size);
    return str;
}

template <>
inline std::tuple<> ReadBuf<std::tuple<>>(const char *buf) {
    return {};
}

template <>
inline std::tuple<> ReadBufAdv<std::tuple<>>(const char *&buf) {
    return {};
}

template <>
inline std::string ReadBufAdv<std::string>(const char *&buf) {
    int32_t size = ReadBufAdv<int32_t>(buf);
    std::string str(buf, size);
    buf += size;
    return str;
}

template <typename T>
const T *ReadBufVecAdv(const char *&buf, size_t size) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *data = reinterpret_cast<T *>(const_cast<char *>(buf));
    buf += sizeof(T) * size;
    return data;
}

template <typename T>
inline void WriteBuf(char *const buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    std::memcpy(buf, &value, sizeof(T));
}

template <typename T>
inline void WriteBufAdv(char *&buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    std::memcpy(buf, &value, sizeof(T));
    buf += sizeof(T);
}

template <>
inline void WriteBuf<std::string>(char *const buf, const std::string &value) {
    int32_t len = value.length();
    WriteBuf(buf, len);
    memcpy(buf + sizeof(len), value.c_str(), len);
}

template <>
inline void WriteBufAdv<std::string>(char *&buf, const std::string &value) {
    int32_t len = value.length();
    WriteBufAdv(buf, len);
    memcpy(buf, value.c_str(), len);
    buf += len;
}

template <>
inline void WriteBuf<std::tuple<>>(char *const buf, const std::tuple<> &) {}

template <>
inline void WriteBufAdv<std::tuple<>>(char *&buf, const std::tuple<> &) {}

template <typename T>
inline void WriteBufVecAdv(char *&buf, const T *data, size_t size) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    memcpy(buf, data, sizeof(T) * size);
    buf += sizeof(T) * size;
}

template <typename T>
void GetSizeInBytesAligned(char *&start) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    auto ptr = reinterpret_cast<uintptr_t>(start);
    size_t t_align = std::alignment_of_v<T>;
    ptr = (ptr + t_align - 1) & ~(t_align - 1);
    start = reinterpret_cast<char *>(ptr);
    start += sizeof(T);
}

template <typename T>
void GetSizeInBytesVecAligned(char *&start, size_t size) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    auto ptr = reinterpret_cast<uintptr_t>(start);
    size_t t_align = std::alignment_of_v<T>;
    ptr = (ptr + t_align - 1) & ~(t_align - 1);
    start = reinterpret_cast<char *>(ptr);
    start += sizeof(T) * size;
}

template <typename T>
T ReadBufAdvAligned(const char *&buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    auto ptr = reinterpret_cast<uintptr_t>(buf);
    size_t t_align = std::alignment_of_v<T>;
    ptr = (ptr + t_align - 1) & ~(t_align - 1);
    buf = reinterpret_cast<const char *>(ptr);
    return ReadBufAdv<T>(buf);
}

template <typename T>
const T *ReadBufVecAdvAligned(const char *&buf, size_t size) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    auto ptr = reinterpret_cast<uintptr_t>(buf);
    size_t t_align = std::alignment_of_v<T>;
    ptr = (ptr + t_align - 1) & ~(t_align - 1);
    buf = reinterpret_cast<const char *>(ptr);
    return ReadBufVecAdv<T>(buf, size);
}

template <typename T>
void WriteBufAdvAligned(char *&buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    auto ptr = reinterpret_cast<uintptr_t>(buf);
    size_t t_align = std::alignment_of_v<T>;
    ptr = (ptr + t_align - 1) & ~(t_align - 1);
    buf = reinterpret_cast<char *>(ptr);
    WriteBuf(buf, value);
    buf += sizeof(T);
}

template <typename T>
void WriteBufVecAdvAligned(char *&buf, const T *data, size_t size) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    auto ptr = reinterpret_cast<uintptr_t>(buf);
    size_t t_align = std::alignment_of_v<T>;
    ptr = (ptr + t_align - 1) & ~(t_align - 1);
    buf = reinterpret_cast<char *>(ptr);
    WriteBufVecAdv(buf, data, size);
}

} // namespace infinity