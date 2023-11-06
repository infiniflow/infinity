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

#include "varchar_type.h"
#include "parser_assert.h"
#include <cstring>
#include <numeric>

namespace infinity {

VarcharType::VarcharType(const std::string &str) { Initialize(str); }

VarcharType::VarcharType(const char *ptr) { Initialize(ptr); }

VarcharType::VarcharType(const char *ptr, size_t len) { Initialize(ptr, len); }

VarcharType::~VarcharType() {
    //    LOG_TRACE("Destructor");
    if (length == 0)
        return;
    Reset();
}

VarcharType::VarcharType(const VarcharType &other) {
    //    LOG_TRACE("Copy constructor");
    DeepCopy(other);
}

VarcharType::VarcharType(VarcharType &&other) noexcept {
    //    LOG_TRACE("Move constructor");
    this->length = other.length;
    if (length <= INLINE_LENGTH) {
        memcpy(this->prefix, other.prefix, length);
    } else {
        memcpy(this->prefix, other.prefix, PREFIX_LENGTH);

        // Don't need to release this->ptr, since it's constructor.
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }
    other.length = 0;
}

VarcharType &VarcharType::operator=(const VarcharType &other) {
    //    LOG_TRACE("Copy assignment");
    if (this == &other)
        return *this;

    DeepCopy(other);
    return *this;
}

VarcharType &VarcharType::operator=(VarcharType &&other) noexcept {
    //    LOG_TRACE("Move assignment");
    if (this->length > 0) {
        // To free current ptr memory.
        Reset();
    }
    this->length = other.length;
    if (this->length <= INLINE_LENGTH) {
        memcpy(this->prefix, other.prefix, length);
    } else {
        memcpy(this->prefix, other.prefix, PREFIX_LENGTH);

        this->ptr = other.ptr;
        other.ptr = nullptr;
    }
    other.length = 0;
    return *this;
}

bool VarcharType::operator==(const VarcharType &other) const {
    if (this->length != other.length)
        return false;
    if (this->IsInlined()) {
        return strncmp(this->prefix, other.prefix, this->length) == 0;
    } else {
        if (strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH) != 0) {
            return false;
        }
        return strncmp(this->ptr, other.ptr, this->length) == 0;
    }
}

bool VarcharType::operator>(const VarcharType &other) const {
    if (this->length > other.length) {
        size_t len = other.length;
        if (this->IsInlined()) {
            if (other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) >= 0;
            } else {
                // Impossible: this is inline but other isn't.
            }
        } else {
            // This isn't inline.
            if (other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res != 0) {
                    return res > 0;
                }

                return strncmp(this->ptr, other.prefix, len) > 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res != 0) {
                    return res > 0;
                }
                return strncmp(this->ptr, other.ptr, len) > 0;
            }
        }
    } else {
        // this->length <= other.length
        size_t len = this->length;
        if (this->IsInlined()) {
            if (other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) > 0;
            } else {
                // Impossible.
            }
        } else {
            if (other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res != 0) {
                    return res > 0;
                }

                return strncmp(this->ptr, other.prefix, len) > 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res != 0) {
                    return res > 0;
                }
                return strncmp(this->ptr, other.ptr, len) > 0;
            }
        }
    }
    ParserError("Not reachable");
    return false;
}

bool VarcharType::operator<=(const VarcharType &other) const { return !operator>(other); }

bool VarcharType::operator<(const VarcharType &other) const {
    if (this->length > other.length) {
        size_t len = other.length;
        if (this->IsInlined()) {
            if (other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) < 0;
            } else {
                // Impossible: this is inline but other isn't.
            }
        } else {
            // This isn't inline.
            if (other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res != 0) {
                    return res < 0;
                }

                return strncmp(this->ptr, other.prefix, len) < 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res != 0) {
                    return res < 0;
                }
                return strncmp(this->ptr, other.ptr, len) < 0;
            }
        }
    } else {
        // this->length <= other.length
        size_t len = this->length;
        if (this->IsInlined()) {
            if (other.IsInlined()) {
                return strncmp(this->prefix, other.prefix, len) > 0;
            } else {
                // Impossible.
            }
        } else {
            if (other.IsInlined()) {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res > 0) {
                    return true;
                } else if (res < 0) {
                    return false;
                }

                return strncmp(this->ptr, other.prefix, len) > 0;
            } else {
                int res = strncmp(this->prefix, other.prefix, VarcharType::PREFIX_LENGTH);
                if (res > 0) {
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

bool VarcharType::operator>=(const VarcharType &other) const { return !operator<(other); }

void VarcharType::DeepCopy(const VarcharType &other) {
    // Used in copy constructor and copy assignment
    if (this->length > 0) {
        Reset();
    }

    if (other.IsInlined()) {
        memcpy((char *)this, (char *)&other, sizeof(other));
    } else {
        // Deep copy all staff.
        this->length = other.length;
        memcpy(prefix, other.prefix, PREFIX_LENGTH);

        ptr = new char[this->length]{0};

        memcpy(ptr, other.ptr, this->length);
    }
}

void VarcharType::Initialize(const std::string &input) {
    size_t input_len = input.length();
    Initialize(input.c_str(), input_len);
}

void VarcharType::Initialize(const char *input_ptr) {
    size_t input_len = strlen(input_ptr);
    Initialize(input_ptr, input_len);
}

void VarcharType::Initialize(const char *input_ptr, size_t input_len) {
    ParserAssert(input_len <= std::numeric_limits<uint16_t>::max(), "Attempt to write string with length exceed 65535 into value");
    ParserAssert(this->length == 0, "Varchar type was already initialized.");

    length = static_cast<uint16_t>(input_len);
    ptr = nullptr;
    if (IsInlined()) {
        memcpy(prefix, input_ptr, length);
    } else {
        ptr = new char[input_len]{0};
        memcpy(prefix, input_ptr, PREFIX_LENGTH);
        memcpy(ptr, input_ptr, length);
    }
}

void VarcharType::InitializeAsEmptyStr() {
    length = 0;
    ptr = nullptr;
}

void VarcharType::Reset(bool clean_memory) {
    if (!clean_memory) {
        length = 0;
        ptr = nullptr;
        return;
    }

    if (IsInlined()) {
        //        LOG_TRACE("Reset inline varchar");
        length = 0;
    } else {
        if (length == 0) {
            ptr = nullptr;
            return;
        }

        delete[] ptr;

        length = 0;
        ptr = nullptr;
    }
};

std::string VarcharType::ToString() const {
    if (IsInlined()) {
        return std::string{prefix, static_cast<size_t>(length)};
    } else {
        return std::string{ptr, static_cast<size_t>(length)};
    }
}

} // namespace infinity
