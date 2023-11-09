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

module;

#include <cstring>

import stl;

module status;

namespace infinity {

Status::Status(ErrorCode code, const char *msg) : code_(code) {
    u64 len = strlen(msg);
    msg_ = new char[len]();
    memcpy(msg_, msg, len);
}

Status::Status(Status &s) {
    Move(s);
}

Status::Status(Status &&s) noexcept {
    Move(s);
}

Status &Status::operator=(Status &s) noexcept {
    Move(s);
    return *this;
}

Status &Status::operator=(Status &&s) noexcept {
    Move(s);
    return *this;
}

void Status::Move(Status &s) {
    code_ = s.code_;
    msg_ = s.msg_;
    s.msg_ = nullptr;
}

void Status::Move(Status &&s) {
    code_ = s.code_;
    msg_ = s.msg_;
    s.msg_ = nullptr;
}

void Status::Init(ErrorCode code, const char* msg) {
    if(msg_ != nullptr) {
        delete[] msg_;
        msg_ = nullptr;
    }
    u64 len = strlen(msg);
    msg_ = new char[len]();
    memcpy(msg_, msg, len);

    code_ = code;
}

} // namespace infinity
