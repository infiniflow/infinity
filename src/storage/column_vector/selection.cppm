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

export module infinity_core:selection;

import :infinity_exception;
import :default_values;

import global_resource_usage;

namespace infinity {

struct SelectionData {
    explicit SelectionData(size_t count) : capacity_(count) {
        if (count > std::numeric_limits<u16>::max()) {
            UnrecoverableError("Too large size for selection data.");
        }
        data_ = std::make_unique<u16[]>(count);
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("SelectionData");
#endif
    }

    ~SelectionData() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("SelectionData");
#endif
    }

    std::unique_ptr<u16[]> data_{};
    size_t capacity_{};
};

export class Selection {
public:
    Selection() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("Selection");
#endif
    }

    ~Selection() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("Selection");
#endif
    }

    void Initialize(size_t count = DEFAULT_VECTOR_SIZE) {
        storage_ = std::make_shared<SelectionData>(count);
        selection_vector = storage_->data_.get();
    }

    inline void Set(size_t selection_idx, size_t row_idx) {
        if (selection_vector == nullptr) {
            UnrecoverableError("Selection container isn't initialized");
        }
        if (selection_idx >= storage_->capacity_) {
            UnrecoverableError("Exceed the selection vector capacity.");
        }
        selection_vector[selection_idx] = row_idx;
    }

    inline void Append(size_t row_idx) {
        Set(latest_selection_idx_, row_idx);
        ++latest_selection_idx_;
    }

    inline size_t Get(size_t idx) const {
        if (selection_vector == nullptr) {
            return idx;
        }
        if (idx >= latest_selection_idx_) {
            UnrecoverableError("Exceed the last row of the selection vector.");
        }
        return selection_vector[idx];
    }

    inline u16 &operator[](size_t idx) const {
        if (idx >= latest_selection_idx_) {
            UnrecoverableError("Exceed the last row of the selection vector.");
        }
        return selection_vector[idx];
    }

    inline size_t Capacity() const {
        if (selection_vector == nullptr) {
            UnrecoverableError("Selection container isn't initialized");
        }
        return storage_->capacity_;
    }

    inline size_t Size() const {
        if (selection_vector == nullptr) {
            UnrecoverableError("Selection container isn't initialized");
        }
        return latest_selection_idx_;
    }

    void Reset() {
        storage_.reset();
        latest_selection_idx_ = 0;
        selection_vector = nullptr;
    }

private:
    size_t latest_selection_idx_{};
    u16 *selection_vector{};
    std::shared_ptr<SelectionData> storage_{};
};

} // namespace infinity
