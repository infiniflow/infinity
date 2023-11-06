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

#pragma once

extern unsigned long strlen (const char *str);

enum class ErrorCode : long {

    kOk = 0,
    kNotFound,
    kNotImplemented,
    kReachQueryMemoryLimit,
    kNotSupportedCast,
    kAlreadyExist,
    kNetworkError,
    kServiceUnavailable,

};

class Status {
public:
    Status() = default;

    Status(ErrorCode code, const char *msg) : code_(code) {
        unsigned long len = strlen(msg);
        msg_ = new char[len]();
    }

    explicit Status(ErrorCode code) : code_(code) {}

    ~Status() {
        if(msg_ != nullptr) {
            delete[] msg_;
            msg_ = nullptr;
        }
    }

    Status(const Status &s);

    Status(Status &&s) noexcept;

    Status &operator=(Status &s);

    const Status &operator=(const Status &s);

    Status &operator=(Status &&s) noexcept;

    const Status &operator=(const Status &&s) noexcept;

    static Status OK() { return Status(); }

    bool ok() const { return code_ == ErrorCode::kOk && msg_ == nullptr; }

    ErrorCode code() const { return code_; }

    const char* message() const {
        return msg_;
    }

private:
    inline void CopyFrom(const Status &s);

    inline void MoveFrom(Status &s);

private:
    ErrorCode code_{ErrorCode::kOk};
    char* msg_{nullptr};
};

