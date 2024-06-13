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

export module emvb_shared_vec;
import stl;

namespace infinity {

// append only
// reallocation will not affect the existing shared pointers for reading old data
export template <typename T>
class EMVBSharedVec {
    mutable std::shared_mutex rw_mutex_; // mutex for read-write operations
    SharedPtr<T[]> data_;
    u32 capacity_ = 0;
    u32 size_ = 0;

    void ReserveUnderLock(u32 new_capacity);
    void ReserveUnderLockForNewSize(u32 new_size) {
        if (new_size <= capacity_) {
            return;
        }
        const u32 new_capacity = new_size + (new_size / 2);
        ReserveUnderLock(new_capacity);
    }

public:
    [[nodiscard]] Pair<SharedPtr<T[]>, u32> GetData() const {
        std::shared_lock lock(rw_mutex_);
        return {data_, size_};
    }
    void Reserve(u32 new_capacity) {
        std::unique_lock lock(rw_mutex_);
        ReserveUnderLock(new_capacity);
    }
    void PushBack(auto &&val) {
        std::unique_lock lock(rw_mutex_);
        ReserveUnderLockForNewSize(size_ + 1);
        data_[size_++] = std::forward<decltype(val)>(val);
    }
    void PushBackIfDifferentFromLast(auto &&val) {
        std::unique_lock lock(rw_mutex_);
        if (size_ > 0 and data_[size_ - 1] == val) {
            return;
        }
        ReserveUnderLockForNewSize(size_ + 1);
        data_[size_++] = std::forward<decltype(val)>(val);
    }
    void Pushback(auto IterBegin, auto IterEnd) {
        std::unique_lock lock(rw_mutex_);
        const u32 append_size = std::distance(IterBegin, IterEnd);
        const u32 new_size = size_ + append_size;
        ReserveUnderLockForNewSize(new_size);
        std::copy_n(IterBegin, append_size, data_.get() + size_);
        size_ = new_size;
    }
};

} // namespace infinity
