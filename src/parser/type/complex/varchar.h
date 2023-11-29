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
#include <cstring>
#include <string>

namespace infinity {

class FixHeapManager;

// varchar related constants
constexpr uint64_t VARCHAR_PREFIX_LENGTH = 5;
constexpr uint64_t VARCHAR_INLINE_LENGTH = 13;
constexpr uint64_t VARCHAR_LENGTH_LIMIT = 8 * 1024 * 1024; // 23 bit or 8MB

#pragma pack(1)
struct ValueVarchar {
    char prefix_[VARCHAR_PREFIX_LENGTH]{};
    char* ptr_{nullptr};
};

struct InlineVarchar {
    char data_[VARCHAR_INLINE_LENGTH]{};
};

struct VectorVarchar {
    char prefix_[VARCHAR_PREFIX_LENGTH]{};
    uint32_t chunk_id_{0};
    uint32_t chunk_offset_{0};
};

struct Varchar {
//public:
//    static constexpr size_t PREFIX_LENGTH = 6;
//    static constexpr size_t INLINE_LENGTH = 14;
//    static constexpr size_t LEN_LIMIT = 65536; // 16 bit

public:
    explicit
    Varchar(bool is_value = true, uint64_t length = 0): is_value_(is_value), length_(length) {
        memset(this->short_.data_, 0, VARCHAR_INLINE_LENGTH);
        if(is_value) {
            if(length == 0) {
                return ;
            } else {
                this->value_.ptr_ = new char[this->length_]{0};
            }
        } else {
            this->vector_.chunk_id_ = 0;
            this->vector_.chunk_offset_ = 0;
        }
    }

//    Varchar(bool is_value): is_value_(is_value), length_(0) {
//        memset(this->short_.data_, 0, VARCHAR_INLINE_LENGTH);
//    }

//    explicit Varchar(const std::string &str);
//
//    explicit Varchar(const char *ptr);
//
//    explicit Varchar(const char *ptr, size_t len);
//
    ~Varchar();

    Varchar(const Varchar &other);

    Varchar(Varchar &&other) noexcept = delete;

    Varchar &operator=(const Varchar &other);

    Varchar &operator=(Varchar &&other) noexcept;

public:
    bool operator==(const Varchar &other) const;

    bool operator>=(const Varchar &other) const = delete;

    bool operator>(const Varchar &other) const = delete;

    bool operator<=(const Varchar &other) const = delete;

    bool operator<(const Varchar &other) const = delete;
//
//    [[nodiscard]] inline char *GetDataPtr() const {
//        if (IsInlined()) {
//            return (char *)(prefix);
//        } else {
//            return this->ptr;
//        }
//    }
//
//    inline size_t GetDataLen() const { return this->length; }
//
public:
    [[nodiscard]] inline bool IsValue() const {
        return is_value_;
    }

    inline void SetValue() {
        is_value_ = true;
    }

    inline void SetColumnVector() {
        is_value_ = false;
    }

    void DeepCopy(const Varchar &other);

    void InitAsValue(const std::string &str);

    void InitAsValue(const char *ptr, bool is_move);

    void InitAsValue(const char *ptr, size_t len, bool is_move);

//    void InitializeAsEmptyStr();
//
    [[nodiscard]] inline bool IsInlined() const { return length_ <= VARCHAR_INLINE_LENGTH; }
//
    void Reset(bool clean_memory = true);

    [[nodiscard]] std::string ToString() const;


    uint64_t is_value_:1 {0};
    uint64_t length_:23 {0};
    union {
        ValueVarchar value_;
        InlineVarchar short_;
        VectorVarchar vector_;
    };
};




} // namespace infinity
