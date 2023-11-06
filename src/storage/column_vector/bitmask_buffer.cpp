//
// Created by JinHai on 2022/11/6.
//

module;

import stl;
import global_resource_usage;

import infinity_exception;

module bitmask_buffer;

namespace infinity {

SharedPtr<BitmaskBuffer> BitmaskBuffer::Make(SizeT count) {
    SharedPtr<BitmaskBuffer> buffer_ptr = MakeShared<BitmaskBuffer>();
    buffer_ptr->Initialize(count);
    return buffer_ptr;
}

SharedPtr<BitmaskBuffer> BitmaskBuffer::Make(const u64 *input_ptr, SizeT count) {
    SharedPtr<BitmaskBuffer> buffer_ptr = MakeShared<BitmaskBuffer>();
    buffer_ptr->Initialize(input_ptr, count);
    return buffer_ptr;
}

BitmaskBuffer::BitmaskBuffer() { GlobalResourceUsage::IncrObjectCount(); }

BitmaskBuffer::~BitmaskBuffer() { GlobalResourceUsage::DecrObjectCount(); }

void BitmaskBuffer::Initialize(SizeT count) {
    Assert<TypeException>((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    SizeT unit_count = UnitCount(count); // u64 array length
    data_ptr_ = MakeUnique<u64[]>(unit_count);
    for (SizeT i = 0; i < unit_count; ++i) {
        data_ptr_[i] = UNIT_MAX;
    }
}

void BitmaskBuffer::Initialize(const u64 *input_ptr, SizeT count) {
    Assert<TypeException>((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    SizeT unit_count = UnitCount(count); // u64 array length
    data_ptr_ = MakeUnique<u64[]>(unit_count);

    void *target_ptr = data_ptr_.get();
    void *source_ptr = (void *)input_ptr;
    Memcpy(target_ptr, source_ptr, unit_count * sizeof(u64));
}

} // namespace infinity