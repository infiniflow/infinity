//
// Created by JinHai on 2022/10/30.
//

#include "blob_type.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

BlobType::BlobType(const BlobType& other) {
    this->Copy(other.ptr, other.size);
}

BlobType::BlobType(BlobType&& other) noexcept {
    Move(other.ptr, other.size);
    other.ptr = nullptr;
    other.size = 0;
}

BlobType& BlobType::operator=(const BlobType& other) {
    if(this == &other) return *this;
    this->Copy(other.ptr, other.size);
    return *this;
}

BlobType& BlobType::operator=(BlobType&& other) noexcept {
    if(this == &other) return *this;
    this->Move(other.ptr, other.size);
    other.ptr = nullptr;
    other.size = 0;
    return *this;
}

void
BlobType::Copy(ptr_t blob_ptr, u64 blob_size) {
    // If input blob size is same as current blob size, don't need to allocate memory again
    if(blob_size != this->size) {
        Reset();

        this->ptr = new char_t[blob_size]{0};
        GlobalResourceUsage::IncrRawMemCount();
    }

    memcpy(this->ptr, blob_ptr, blob_size);
    this->size = blob_size;
}

void
BlobType::Move(ptr_t blob_ptr, u64 blob_size) {
    Reset();
    this->ptr = blob_ptr;
    this->size = blob_size;
}

}
