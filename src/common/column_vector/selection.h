//
// Created by JinHai on 2022/11/6.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/default_values.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

struct SelectionData {
    explicit
    SelectionData(SizeT count) : capacity_(count) {
        ExecutorAssert(count <= std::numeric_limits<u16>::max(), "Too large size for selection data.")
        data_ = MakeUnique<u16[]>(count);
        GlobalResourceUsage::IncrObjectCount();
    }

    ~SelectionData() {
        GlobalResourceUsage::DecrObjectCount();
    }

    UniquePtr<u16[]> data_{};
    SizeT capacity_{};
};

class Selection {
public:
    Selection() {
        GlobalResourceUsage::IncrObjectCount();
    }

    ~Selection() {
        GlobalResourceUsage::DecrObjectCount();
    }

    void
    Initialize(SizeT count = DEFAULT_VECTOR_SIZE) {
        storage_ = MakeShared<SelectionData>(count);
        selection_vector = storage_->data_.get();
    }

    inline void
    Set(SizeT selection_idx, SizeT row_idx) {
        ExecutorAssert(selection_vector != nullptr, "Selection container isn't initialized")
        ExecutorAssert(selection_idx < storage_->capacity_, "Exceed the selection vector capacity.")
        selection_vector[selection_idx] = row_idx;
    }

    inline void
    Append(SizeT row_idx) {
        Set(latest_selection_idx_, row_idx);
        ++ latest_selection_idx_;
    }

    [[nodiscard]] inline SizeT
    Get(SizeT idx) const {
        if(selection_vector == nullptr) {
            return idx;
        }
        ExecutorAssert(idx < latest_selection_idx_,
                       "Exceed the last row of the selection vector.")
        return selection_vector[idx];
    }

    inline u16&
    operator[](SizeT idx) const {
        ExecutorAssert(idx < latest_selection_idx_,
                       "Exceed the last row of the selection vector.")
        return selection_vector[idx];
    }

    [[nodiscard]] inline SizeT
    Capacity() const {
        ExecutorAssert(selection_vector != nullptr, "Selection container isn't initialized")
        return storage_->capacity_;
    }

    [[nodiscard]] inline SizeT
    Size() const {
        ExecutorAssert(selection_vector != nullptr, "Selection container isn't initialized")
        return latest_selection_idx_;
    }

    void
    Reset() {
        storage_.reset();
        latest_selection_idx_ = 0;
        selection_vector = nullptr;
    }

private:
    SizeT latest_selection_idx_{};
    u16* selection_vector{};
    SharedPtr<SelectionData> storage_{};
};

}


