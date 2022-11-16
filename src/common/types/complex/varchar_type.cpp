//
// Created by JinHai on 2022/10/28.
//

#include "varchar_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

VarcharType::VarcharType(const String& str) {
    Initialize(str);
}

VarcharType::VarcharType(const char* ptr) {
    Initialize(ptr);
}

VarcharType::VarcharType(const char* ptr, size_t len) {
    Initialize(ptr, len);
}

VarcharType::~VarcharType() {
//    LOG_TRACE("Destructor");
    if(length == 0) return ;
    Reset();
}

VarcharType::VarcharType(const VarcharType& other) {
//    LOG_TRACE("Copy constructor");
    DeepCopy(other);
}

VarcharType::VarcharType(VarcharType&& other) noexcept {
//    LOG_TRACE("Move constructor");
    this->length = other.length;
    if(length <= INLINE_LENGTH) {
        memcpy(this->prefix, other.prefix, length);
    } else {
        memcpy(this->prefix, other.prefix, PREFIX_LENGTH);

        // Don't need to release this->ptr, since it's constructor.
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }
    other.length = 0;
}

VarcharType&
VarcharType::operator=(const VarcharType& other) {
//    LOG_TRACE("Copy assignment");
    if(this == &other) return *this;

    DeepCopy(other);
    return *this;
}

VarcharType&
VarcharType::operator=(VarcharType&& other) noexcept {
//    LOG_TRACE("Move assignment");
    if(this->length > 0) {
        // To free current ptr memory.
        Reset();
    }
    this->length = other.length;
    if(this->length <= INLINE_LENGTH) {
        memcpy(this->prefix, other.prefix, length);
    } else {
        memcpy(this->prefix, other.prefix, PREFIX_LENGTH);

        this->ptr = other.ptr;
        other.ptr = nullptr;
    }
    other.length = 0;
    return *this;
}


void
VarcharType::DeepCopy(const VarcharType &other) {
    // Used in copy constructor and copy assignment
    if(this->length > 0) {
        // this->will will be zero in copy constructor, so now is copy assignment case
        Reset();
    }

    if(other.IsInlined()) {
        memcpy((char*)this, (char*)&other, sizeof(other));
    } else {
        // Deep copy all staff.
        this->length = other.length;
        memcpy(prefix, other.prefix, PREFIX_LENGTH);

        ptr = new char[this->length]();
        LOG_TRACE("DeepCopy: allocate memory: {}, {}", (void*)ptr, length);

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
    TypeAssert(this->length == 0, "Varchar type was already initialized.")

    length = static_cast<i16>(input_len);
    ptr = nullptr;
    if(IsInlined()) {
        LOG_TRACE("Initialize, inline varchar: {}", length);
        memcpy(prefix, input_ptr, length);
        // ptr maybe also padding by the memcpy
    } else {
        ptr = new char[input_len]();
        LOG_TRACE("Initialize: allocate memory: {}, {}", (void*)ptr, length);
        memcpy(prefix, input_ptr, PREFIX_LENGTH);
        memcpy(ptr, input_ptr, length);
    }
}

void
VarcharType::Reset() {
    if(IsInlined()) {
        LOG_TRACE("Reset inline varchar");
        length = 0;
    } else {
        LOG_TRACE("Reset: free memory: {}, {}", (void*)ptr, length);
        length = 0;
        delete[] ptr;
        ptr = nullptr;
    }
};

String
VarcharType::ToString() const {
    if(IsInlined()) {
        return String(prefix, length);
    } else {
        return String(ptr, length);
    }
}

}
