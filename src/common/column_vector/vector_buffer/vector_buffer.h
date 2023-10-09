//
// Created by JinHai on 2022/11/6.
//

#pragma once

#include "common/types/data_type.h"
#include "heap_chunk.h"

namespace infinity {

enum class VectorBufferType { kInvalid, kStandard, kHeap };

class VectorBuffer {
public:
    static SharedPtr<VectorBuffer> Make(SizeT data_type_size, SizeT capacity, VectorBufferType buffer_type);

public:
    explicit VectorBuffer() { GlobalResourceUsage::IncrObjectCount(); }

    ~VectorBuffer() { GlobalResourceUsage::DecrObjectCount(); }

    void Initialize(SizeT type_size, SizeT capacity);

    void ResetToInit();

    void Copy(ptr_t input, SizeT size);

    [[nodiscard]] ptr_t GetData() const { return data_.get(); }

public:
    bool initialized_{false};
    UniquePtr<char[]> data_{nullptr};
    SizeT data_size_{0};
    SizeT capacity_{0};
    VectorBufferType buffer_type_{VectorBufferType::kInvalid};

    UniquePtr<StringHeapMgr> heap_mgr_{nullptr};
};

} // namespace infinity
