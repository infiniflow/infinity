//
// Created by JinHai on 2022/11/6.
//

#include "bitmask_buffer.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

SharedPtr<BitmaskBuffer> BitmaskBuffer::Make(size_t count) {
    SharedPtr<BitmaskBuffer> buffer_ptr = MakeShared<BitmaskBuffer>();
    buffer_ptr->Initialize(count);
    return buffer_ptr;
}

SharedPtr<BitmaskBuffer> BitmaskBuffer::Make(const u64 *input_ptr, size_t count) {
    SharedPtr<BitmaskBuffer> buffer_ptr = MakeShared<BitmaskBuffer>();
    buffer_ptr->Initialize(input_ptr, count);
    return buffer_ptr;
}

BitmaskBuffer::BitmaskBuffer() { GlobalResourceUsage::IncrObjectCount(); }

BitmaskBuffer::~BitmaskBuffer() { GlobalResourceUsage::DecrObjectCount(); }

void BitmaskBuffer::Initialize(size_t count) {
    TypeAssert((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    size_t unit_count = UnitCount(count); // u64 array length
    data_ptr_ = MakeUnique<u64[]>(unit_count);
    for (size_t i = 0; i < unit_count; ++i) {
        data_ptr_[i] = UNIT_MAX;
    }
}

void BitmaskBuffer::Initialize(const u64 *input_ptr, size_t count) {
    TypeAssert((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    size_t unit_count = UnitCount(count); // u64 array length
    data_ptr_ = MakeUnique<u64[]>(unit_count);

    void *target_ptr = data_ptr_.get();
    void *source_ptr = (void *)input_ptr;
    memcpy(target_ptr, source_ptr, unit_count * sizeof(u64));
}

} // namespace infinity