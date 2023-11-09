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

export module status;

namespace infinity {

export enum class ErrorCode : long {

    kOk = 0,
    kError,
    kNotFound,
    kNotImplemented,
    kReachQueryMemoryLimit,
    kNotSupportedCast,
    kAlreadyExist,
    kNetworkError,
    kServiceUnavailable,

};

export class Status {
public:
    Status() = default;

    inline explicit Status(ErrorCode code) : code_(code) {}

    Status(ErrorCode code, const char *msg);

    inline ~Status() {
        if(msg_ != nullptr) {
            delete[] msg_;
            msg_ = nullptr;
        }
    }

    Status(Status &s);

    Status(Status &&s) noexcept;

    Status &operator=(Status &s) noexcept;

    Status &operator=(Status &&s) noexcept;

    const Status &operator=(const Status &s) noexcept = delete;

    const Status &operator=(const Status &&s) noexcept = delete;

    static Status OK() { return {}; }

    [[nodiscard]] bool ok() const { return code_ == ErrorCode::kOk && msg_ == nullptr; }

    [[nodiscard]] ErrorCode code() const { return code_; }

    void Init(ErrorCode code, const char* msg);

    [[nodiscard]] const char* message() const {
        return msg_;
    }

private:
    void Move(Status &s);
    void Move(Status &&s);

private:
    ErrorCode code_{ErrorCode::kOk};
    char* msg_{nullptr};
};

}
