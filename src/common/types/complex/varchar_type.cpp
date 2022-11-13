//
// Created by JinHai on 2022/10/28.
//

#include "varchar_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

//VarcharType::VarcharType(const VarcharType& other) {
//    this->DeepCopy(other);
//}

void
VarcharType::DeepCopy(const VarcharType &other) {
    if(other.IsInlined()) {
        memcpy((char*)this, (char*)&other, sizeof(other));
    } else {
        // Deep copy all staff.
        this->length = other.length;
        memcpy(prefix, other.prefix, PREFIX_LENGTH);

        LOG_DEBUG("Allocate new memory: ", ptr, length);
        ptr = new char[this->length]();
        memcpy(ptr, other.ptr, this->length);
    }
}

void
VarcharType::Initialize(const String& input) {
    size_t input_len = input.length();
    Initialize(input.c_str(), input_len);
}

void
VarcharType::Initialize(const char* input_ptr) {
    size_t input_len = strlen(input_ptr);
    Initialize(input_ptr, input_len);
}

void
VarcharType::Initialize(const char* input_ptr, size_t input_len) {
    TypeAssert(input_len < std::numeric_limits<i16>::max(),
               "Attempt to write string with length exceed 65535 into value");

    length = static_cast<i16>(input_len);
    if(IsInlined()) {
        memcpy(prefix, input_ptr, input_len);
    } else {
        ptr = new char[input_len]();
        LOG_DEBUG("Allocate new memory: ", ptr, length);
        memcpy(prefix, input_ptr, PREFIX_LENGTH);
        memcpy(ptr, input_ptr, input_len);
    }
}

// Only reset the ptr, but not free the memory
void
VarcharType::Reset() {
    if(IsInlined()) {
        return ;
    }
    ptr = nullptr;
};

void
VarcharType::Destroy() {
    if(IsInlined()) {
        // No heap is allocated.
        return ;
    }

    LOG_DEBUG("Deallocate memory: ", ptr, length);
    delete ptr;
    ptr = nullptr;
}

String
VarcharType::ToString() const {
    if(IsInlined()) {
        return String(prefix, length);
    } else {
        return String(ptr, length);
    }
}

}
