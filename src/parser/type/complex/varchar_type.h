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

#include <cstdint>
#include <string>

namespace infinity {

struct VarcharType {
public:
    static constexpr size_t PREFIX_LENGTH = 6;
    static constexpr size_t INLINE_LENGTH = 14;
    static constexpr size_t LEN_LIMIT = 65536; // 16 bit

public:
    VarcharType() {
        length = 0;
        ptr = nullptr;
    }

    explicit VarcharType(const std::string &str);

    explicit VarcharType(const char *ptr);

    explicit VarcharType(const char *ptr, size_t len);

    ~VarcharType();

    VarcharType(const VarcharType &other);

    VarcharType(VarcharType &&other) noexcept;

    VarcharType &operator=(const VarcharType &other);

    VarcharType &operator=(VarcharType &&other) noexcept;

public:
    bool operator==(const VarcharType &other) const;

    bool operator>=(const VarcharType &other) const;

    bool operator>(const VarcharType &other) const;

    bool operator<=(const VarcharType &other) const;

    bool operator<(const VarcharType &other) const;

    [[nodiscard]] inline char *GetDataPtr() const {
        if (IsInlined()) {
            return (char *)(prefix);
        } else {
            return this->ptr;
        }
    }

    inline size_t GetDataLen() const { return this->length; }

public:
    void DeepCopy(const VarcharType &other);

    void Initialize(const std::string &str);

    void Initialize(const char *ptr);

    void Initialize(const char *ptr, size_t len);

    void InitializeAsEmptyStr();

    [[nodiscard]] inline bool IsInlined() const { return length <= INLINE_LENGTH; }

    void Reset(bool clean_memory = true);

    [[nodiscard]] std::string ToString() const;

    uint16_t length{0};           // 65535 will be the limitation.
    char prefix[PREFIX_LENGTH]{}; // prefix of the varchar
    // If length <= 14, ptr will be used as prefix.
    char *ptr{nullptr}; // pointer to the varchar value buffer.
};

} // namespace infinity
