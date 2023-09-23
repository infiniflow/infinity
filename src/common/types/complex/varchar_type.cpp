//
// Created by JinHai on 2022/10/28.
//

#include "varchar_type.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

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

bool
VarcharType::operator==(const VarcharType& other) const {
    if(this->length != other.length) return false;
    if(this->IsInlined()) {
        return strncmp(this->prefix, other.prefix, this->length) == 0;
    } else {
        if(strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH) != 0) {
            return false;
        }
        return strncmp(this->ptr, other.ptr, this->length) == 0;
    }
}

bool
VarcharType::operator>(const VarcharType& other) const {
    if(this->length > other.length) {
        SizeT len = other.length;
        if(this->IsInlined()) {
            if(other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) >= 0;
            } else {
                // Impossible: this is inline but other isn't.
            }
        } else {
            // This isn't inline.
            if(other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res != 0) {
                    return res > 0;
                }

                return strncmp(this->ptr, other.prefix, len) > 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res != 0) {
                    return res > 0;
                }
                return strncmp(this->ptr, other.ptr, len) > 0;
            }
        }
    } else {
        // this->length <= other.length
        SizeT len = this->length;
        if(this->IsInlined()) {
            if(other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) > 0;
            } else {
                // Impossible.
            }
        } else {
            if(other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res != 0) {
                    return res > 0;
                }

                return strncmp(this->ptr, other.prefix, len) > 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res != 0) {
                    return res > 0;
                }
                return strncmp(this->ptr, other.ptr, len) > 0;
            }
        }
    }
    ExecutorError("Not reachable")
}

bool
VarcharType::operator<=(const VarcharType& other) const {
    return !operator>(other);
}

bool
VarcharType::operator<(const VarcharType& other) const {
    if(this->length > other.length) {
        SizeT len = other.length;
        if(this->IsInlined()) {
            if(other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) < 0;
            } else {
                // Impossible: this is inline but other isn't.
            }
        } else {
            // This isn't inline.
            if(other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res != 0) {
                    return res < 0;
                }

                return strncmp(this->ptr, other.prefix, len) < 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res != 0) {
                    return res < 0;
                }
                return strncmp(this->ptr, other.ptr, len) < 0;
            }
        }
    } else {
        // this->length <= other.length
        SizeT len = this->length;
        if(this->IsInlined()) {
            if(other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) > 0;
            } else {
                // Impossible.
            }
        } else {
            if(other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res > 0) {
                    return true;
                } else if (res < 0) {
                    return false;
                }

                return strncmp(this->ptr, other.prefix, len) > 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if(res > 0) {
                    return true;
                } else if (res < 0) {
                    return false;
                }
                return strncmp(this->ptr, other.ptr, len) > 0;
            }
        }
    }

    return true;
}

bool
VarcharType::operator>=(const VarcharType& other) const {
    return !operator<(other);
}



void
VarcharType::DeepCopy(const VarcharType &other) {
    // Used in copy constructor and copy assignment
    if(this->length > 0) {
        Reset();
    }

    if(other.IsInlined()) {
        memcpy((char*)this, (char*)&other, sizeof(other));
    } else {
        // Deep copy all staff.
        this->length = other.length;
        memcpy(prefix, other.prefix, PREFIX_LENGTH);

        ptr = new char[this->length]{0};
        GlobalResourceUsage::IncrRawMemCount();

//        LOG_TRACE("DeepCopy: allocate memory: {}, {}", (void*)ptr, length);

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
    TypeAssert(input_len <= std::numeric_limits<u16>::max(),
               "Attempt to write string with length exceed 65535 into value");
    TypeAssert(this->length == 0, "Varchar type was already initialized.")

    length = static_cast<u16>(input_len);
    ptr = nullptr;
    if(IsInlined()) {
//        LOG_TRACE("Initialize, inline varchar: {}", length);
        memcpy(prefix, input_ptr, length);
        // ptr maybe also padding by the memcpy
    } else {
        ptr = new char[input_len]{0};
        GlobalResourceUsage::IncrRawMemCount();

//        LOG_TRACE("Initialize: allocate memory: {}, {}", (void*)ptr, length);
        memcpy(prefix, input_ptr, PREFIX_LENGTH);
        memcpy(ptr, input_ptr, length);
    }
}

void
VarcharType::InitializeAsEmptyStr() {
    length = 0;
    ptr = nullptr;
}

void
VarcharType::Reset(bool clean_memory) {
    if(!clean_memory) {
        length = 0;
        ptr = nullptr;
        return ;
    }

    if(IsInlined()) {
//        LOG_TRACE("Reset inline varchar");
        length = 0;
    } else {
        if(length == 0) {
            ptr = nullptr;
            return ;
        }

//        LOG_TRACE("Reset: free memory: {}, {}", (void*)ptr, length);

        delete[] ptr;
        GlobalResourceUsage::DecrRawMemCount();

        length = 0;
        ptr = nullptr;
    }
};

String
VarcharType::ToString() const {
    if(IsInlined()) {
        return String{prefix, static_cast<size_t>(length)};
    } else {
        return String{ptr, static_cast<size_t>(length)};
    }
}

}
