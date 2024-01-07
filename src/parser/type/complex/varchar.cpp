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

#include "varchar.h"

#include "parser_assert.h"
#include <cstring>
#include <numeric>

namespace infinity {

// Varchar::Varchar(const std::string &str) { Initialize(str); }
//
// Varchar::Varchar(const char *ptr) { Initialize(ptr); }
//
// Varchar::Varchar(const char *ptr, size_t len) { Initialize(ptr, len); }

Varchar::~Varchar() { Reset(); }

Varchar::Varchar(const Varchar &other) { DeepCopy(other); }

// Varchar::Varchar(Varchar &&other) noexcept {
//     //    LOG_TRACE("std::move constructor");
//     this->length_ = other.length_;
//     if (other.IsInlined()) {
//         memcpy(this->short_.data_, other.short_.data_, length_);
//     } else {
//         if(other.IsValue()) {
//             memcpy(this->value_.prefix_, other.value_.prefix_, VARCHAR_PREFIX_LENGTH);
//             // Don't need to release this->ptr, since it's constructor.
//             this->value_.ptr_ = other.value_.ptr_;
//             other.value_.ptr_ = nullptr;
//             other.length_ = 0;
//         } else {
//             // Is ColumnVector
//             ParserError("Only support move constructor operator from varchar value to varchar value");
//         }
//     }
// }

Varchar &Varchar::operator=(const Varchar &other) {
    //    LOG_TRACE("Copy assignment");
    if (this == &other)
        return *this;

    DeepCopy(other);
    return *this;
}

Varchar &Varchar::operator=(Varchar &&other) noexcept {
    //    LOG_TRACE("std::move assignment");
    if(this->IsValue()) {
        // Free memory
        Reset();
    }

    this->length_ = other.length_;
    this->is_value_ = other.is_value_;
    if (other.IsInlined()) {
        memcpy(this->short_.data_, other.short_.data_, length_);
    } else {
        if (other.IsValue()) {
            memcpy(this->value_.prefix_, other.value_.prefix_, VARCHAR_PREFIX_LENGTH);
            // Don't need to release this->ptr, since it's constructor.
            this->value_.ptr_ = other.value_.ptr_;
            other.value_.ptr_ = nullptr;
        } else {
            // Is ColumnVector
            ParserError("Only support move constructor operator from varchar value to varchar value");
        }
    }
    other.length_ = 0;
    return *this;
}

bool Varchar::operator==(const Varchar &other) const {
    if (this->length_ != other.length_)
        return false;
    if (this->IsValue() != other.IsValue()) {
        return false;
    }
    if (this->IsInlined()) {
        return strncmp(this->short_.data_, other.short_.data_, this->length_) == 0;
    } else {
        if (IsValue()) {
            if (strncmp(this->value_.prefix_, other.value_.prefix_, VARCHAR_PREFIX_LENGTH) != 0) {
                return false;
            }
            return strncmp(this->value_.ptr_, other.value_.ptr_, this->length_) == 0;
        } else {
            // Is ColumnVector
            ParserError("Only support equal operator from varchar value to varchar value");
        }
    }
    return true;
}
//
// bool Varchar::operator>(const Varchar &other) const {
//    if (this->length > other.length) {
//        size_t len = other.length;
//        if (this->IsInlined()) {
//            if (other.IsInlined()) {
//                return strncmp(this->prefix, other.prefix, len) >= 0;
//            } else {
//                // Impossible: this is inline but other isn't.
//            }
//        } else {
//            // This isn't inline.
//            if (other.IsInlined()) {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res != 0) {
//                    return res > 0;
//                }
//
//                return strncmp(this->ptr, other.prefix, len) > 0;
//            } else {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res != 0) {
//                    return res > 0;
//                }
//                return strncmp(this->ptr, other.ptr, len) > 0;
//            }
//        }
//    } else {
//        // this->length <= other.length
//        size_t len = this->length;
//        if (this->IsInlined()) {
//            if (other.IsInlined()) {
//                return strncmp(this->prefix, other.prefix, len) > 0;
//            } else {
//                // Impossible.
//            }
//        } else {
//            if (other.IsInlined()) {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res != 0) {
//                    return res > 0;
//                }
//
//                return strncmp(this->ptr, other.prefix, len) > 0;
//            } else {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res != 0) {
//                    return res > 0;
//                }
//                return strncmp(this->ptr, other.ptr, len) > 0;
//            }
//        }
//    }
//    ParserError("Not reachable");
//    return false;
//}
//
// bool Varchar::operator<=(const Varchar &other) const { return !operator>(other); }
//
// bool Varchar::operator<(const Varchar &other) const {
//    if (this->length > other.length) {
//        size_t len = other.length;
//        if (this->IsInlined()) {
//            if (other.IsInlined()) {
//                return strncmp(this->prefix, other.prefix, len) < 0;
//            } else {
//                // Impossible: this is inline but other isn't.
//            }
//        } else {
//            // This isn't inline.
//            if (other.IsInlined()) {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res != 0) {
//                    return res < 0;
//                }
//
//                return strncmp(this->ptr, other.prefix, len) < 0;
//            } else {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res != 0) {
//                    return res < 0;
//                }
//                return strncmp(this->ptr, other.ptr, len) < 0;
//            }
//        }
//    } else {
//        // this->length <= other.length
//        size_t len = this->length;
//        if (this->IsInlined()) {
//            if (other.IsInlined()) {
//                return strncmp(this->prefix, other.prefix, len) > 0;
//            } else {
//                // Impossible.
//            }
//        } else {
//            if (other.IsInlined()) {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res > 0) {
//                    return true;
//                } else if (res < 0) {
//                    return false;
//                }
//
//                return strncmp(this->ptr, other.prefix, len) > 0;
//            } else {
//                int res = strncmp(this->prefix, other.prefix, Varchar::PREFIX_LENGTH);
//                if (res > 0) {
//                    return true;
//                } else if (res < 0) {
//                    return false;
//                }
//                return strncmp(this->ptr, other.ptr, len) > 0;
//            }
//        }
//    }
//
//    return true;
//}
//
// bool Varchar::operator>=(const Varchar &other) const { return !operator<(other); }
//
void Varchar::DeepCopy(const Varchar &other) {
    // Used in copy constructor and copy assignment
    if (!other.IsValue() && !other.IsInlined()) {
        ParserError("Only support copy from varchar value to varchar value");
    }

    if (this->length_ > 0) {
        Reset();
    }
    this->is_value_ = other.is_value_;
    if (other.IsInlined()) {
        memcpy((char *)this, (char *)&other, sizeof(other));
    } else {
        // Deep copy all staff.
        this->length_ = other.length_;
        memcpy(this->value_.prefix_, other.value_.prefix_, VARCHAR_PREFIX_LENGTH);

        this->value_.ptr_ = new char[this->length_];

        memcpy(value_.ptr_, other.value_.ptr_, this->length_);
    }
}

void Varchar::InitAsValue(const std::string &input) {
    size_t input_len = input.length();
    InitAsValue(input.c_str(), input_len, false);
}

void Varchar::InitAsValue(const char *input_ptr, bool is_move) {
    size_t input_len = strlen(input_ptr);
    InitAsValue(input_ptr, input_len, is_move);
}

void Varchar::InitAsValue(const char *input_ptr, size_t input_len, bool is_move) {
    if (input_len > VARCHAR_LENGTH_LIMIT) {
        ParserError("Attempt to write string with length exceed 8M into value");
    }

    if (length_ != 0) {
        ParserError("Varchar type was already initialized.");
    }

    length_ = static_cast<uint16_t>(input_len);
    is_value_ = true;

    if (IsInlined()) {
        memcpy(short_.data_, input_ptr, length_);
        if (is_move) {
            delete input_ptr;
        }
    } else {
        memcpy(value_.prefix_, input_ptr, VARCHAR_PREFIX_LENGTH);
        if (is_move) {
            value_.ptr_ = const_cast<char *>(input_ptr);
        } else {
            value_.ptr_ = new char[input_len];
            memcpy(value_.ptr_, input_ptr, input_len);
        }
    }
}

//
// void Varchar::InitializeAsEmptyStr() {
//    length = 0;
//    ptr = nullptr;
//}
//
void Varchar::Reset(bool clean_memory) {
    if (IsInlined()) {
        length_ = 0;
        return;
    }

    if (!clean_memory) {
        length_ = 0;
        if (IsValue()) {
            value_.ptr_ = nullptr;
        } else {
            // column_vector
            vector_.chunk_id_ = 0;
            vector_.chunk_offset_ = 0;
        }
        return;
    }

    if (IsValue()) {
        delete[] value_.ptr_;
        length_ = 0;
        value_.ptr_ = nullptr;
    }
}

std::string Varchar::ToString() const {
    if (IsInlined()) {
        return std::string{short_.data_, static_cast<size_t>(length_)};
    } else {
        if (is_value_) {
            return std::string{value_.ptr_, static_cast<size_t>(length_)};
        } else {
            return std::string{vector_.prefix_, static_cast<size_t>(VARCHAR_PREFIX_LENGTH)};
        }
    }
}

} // namespace infinity
