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

export module infinity_core:vector_with_lock;

import std.compat;

namespace infinity {

export template <typename ValueType>
class VectorWithLock {
private:
    std::shared_mutex mutex_;
    std::vector<ValueType> vec_;

public:
    VectorWithLock() = default;
    VectorWithLock(size_t count) : vec_(count) {}
    VectorWithLock(size_t count, const ValueType &value) : vec_(count, value) {}

    ~VectorWithLock() = default;

    void Resize(size_t new_size) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        vec_.resize(new_size);
    }

    ValueType Get(size_t i) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        if (i >= vec_.size())
            return std::numeric_limits<ValueType>::max();
        return vec_[i];
    }

    ValueType Sum() {
        ValueType sum = 0;
        std::shared_lock<std::shared_mutex> lock(mutex_);
        for (size_t i = 0; i < vec_.size(); ++i)
            sum += vec_[i];
        return sum;
    }

    void Set(size_t i, const ValueType &value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        vec_[i] = value;
    }

    void SetBatch(size_t begin_idx, const std::vector<ValueType> &values) {
        size_t required_size = begin_idx + values.size();
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (vec_.size() < required_size)
            vec_.resize(required_size);
        for (size_t i = 0; i < values.size(); ++i)
            vec_[begin_idx + i] = values[i];
    }

    std::vector<ValueType> &UnsafeVec() { return vec_; }

    void Clear() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        vec_.clear();
    }
};
} // namespace infinity
