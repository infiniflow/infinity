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

export module infinity_core:data_store_util;

import std;
import std.compat;

namespace infinity {

export template <typename T, bool OwnMem>
class ArrayPtr {
public:
    ArrayPtr() = default;
    ArrayPtr(std::unique_ptr<T[]> ptr) : ptr_(std::move(ptr)) {}

    T &operator[](size_t idx) { return ptr_[idx]; }
    const T &operator[](size_t idx) const { return ptr_[idx]; }

    T *get() const { return ptr_.get(); }

    std::unique_ptr<T[]> exchange(std::unique_ptr<T[]> ptr) { return std::exchange(ptr_, std::move(ptr)); }

private:
    std::unique_ptr<T[]> ptr_;
};

export template <typename T>
class ArrayPtr<T, false> {
public:
    ArrayPtr() = default;
    ArrayPtr(const T *ptr) : ptr_(ptr) {}

    const T &operator[](size_t idx) const { return ptr_[idx]; }

    const T *get() const { return ptr_; }

private:
    const T *ptr_ = nullptr;
};

export template <bool OwnMem>
class PPtr {
public:
    PPtr() = default;
    void set(char *ptr) { ptr_ = ptr; }
    char *get() const { return ptr_; }

private:
    char *ptr_;
};

export template <>
class PPtr<false> {
public:
    PPtr() = default;
    void set(const char *ptr) { ptr_ = ptr; }
    const char *get() const { return ptr_; }

private:
    const char *ptr_ = nullptr;
};

} // namespace infinity
