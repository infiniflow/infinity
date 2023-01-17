//
// Created by JinHai on 2022/11/6.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "heap_chunk.h"

namespace infinity {

enum class VectorBufferType {
    kInvalid,
    kStandard,
    kHeap
};

class VectorBuffer {
public:
    static SharedPtr<VectorBuffer>
    Make(size_t data_type_size, size_t capacity, VectorBufferType buffer_type);

public:
    explicit VectorBuffer() {
        GlobalResourceUsage::IncrObjectCount();
    }

    ~VectorBuffer() {
        GlobalResourceUsage::DecrObjectCount();
    }

    void
    Initialize(size_t type_size, size_t capacity);

    void
    ResetToInit();

    void
    Copy(ptr_t input, size_t size);

    [[nodiscard]] ptr_t
    GetData() const { return data_.get(); }

public:
    bool initialized_ {false};
    UniquePtr<char[]> data_ {nullptr};
    size_t data_size_ {0};
    size_t capacity_ {0};
    VectorBufferType buffer_type_{VectorBufferType::kInvalid};

    UniquePtr<StringHeapMgr> heap_mgr_{nullptr};
};

}
