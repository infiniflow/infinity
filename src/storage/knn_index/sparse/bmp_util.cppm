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

export module bmp_util;

import stl;
import statement_common;

namespace infinity {

export using BMPBlockID = i32;
export using BMPBlockOffset = u8;

export using BMPDocID = u32;

export enum class BMPCompressType : i8 {
    kRaw = 0,
    kCompressed = 1,
    kInvalid = 2,
};

export enum class BMPOwnMem : i8 {
    kTrue = 0,
    kFalse = 1,
};

export template <typename T, BMPOwnMem OwnMem>
class VecPtr {};

export template <typename T>
class VecPtr<T, BMPOwnMem::kTrue> {
public:
    VecPtr() = default;
    VecPtr(Vector<T> ptr) : vec_(std::move(ptr)) {}

    T &operator[](SizeT idx) { return vec_[idx]; }
    const T &operator[](SizeT idx) const { return vec_[idx]; }

    T *data() { return vec_.data(); }
    const T *data() const { return vec_.data(); }
    SizeT size() const { return vec_.size(); }

    template<typename U>
    void push_back(U&& val) { vec_.push_back(std::forward<U>(val)); }

    Vector<T> exchange(Vector<T> vec) { return std::exchange(vec_, std::move(vec)); }

private:
    Vector<T> vec_;
};

export template <typename T>
class VecPtr<T, BMPOwnMem::kFalse> {
public:
    VecPtr() = default;
    VecPtr(const T *ptr, SizeT size) : ptr_(ptr), size_(size) {}

    const T &operator[](SizeT idx) const { return ptr_[idx]; }

    SizeT size() const { return size_; }

private:
    const T *ptr_ = nullptr;
    SizeT size_ = 0;
};

export BMPCompressType BMPCompressTypeFromString(const String &compress_type_str) {
    if (IsEqual(compress_type_str, "raww")) {
        return BMPCompressType::kRaw;
    } else if (IsEqual(compress_type_str, "compress")) {
        return BMPCompressType::kCompressed;
    } else {
        return BMPCompressType::kInvalid;
    }
}

export String BMPCompressTypeToString(BMPCompressType compress_type) {
    switch (compress_type) {
        case BMPCompressType::kRaw: {
            return "raw";
        }
        case BMPCompressType::kCompressed: {
            return "compress";
        }
        case BMPCompressType::kInvalid: {
            return "Invalid";
        }
    }
}
export struct BmpSearchOptions {
    f32 alpha_ = 1.0;
    f32 beta_ = 1.0;
    bool use_tail_ = true;
    bool use_lock_ = true;
};

export struct BMPOptimizeOptions {
    i32 topk_ = 0;
    bool bp_reorder_ = false;
};

export class BMPUtil {
public:
    static BmpSearchOptions ParseBmpSearchOptions(const Vector<UniquePtr<InitParameter>> &opt_params);

    static Optional<BMPOptimizeOptions> ParseBMPOptimizeOptions(const Vector<UniquePtr<InitParameter>> &opt_params);
};

} // namespace infinity