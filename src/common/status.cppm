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

import stl;

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
    kWWConflict,
    kDuplicate,
    kUndefined,
    kInvalidIdentifier
};

export class Status {
public:
    Status() = default;

    inline explicit Status(ErrorCode code) : code_(code) {}

    inline Status(ErrorCode code, UniquePtr<String> message): code_(code), msg_(Move(message)) {}

    Status(ErrorCode code, const char *msg);

    Status(Status &s);

    Status(Status &&s) noexcept;

    Status &operator=(Status &s) noexcept;

    Status &operator=(Status &&s) noexcept;

    const Status &operator=(const Status &s) noexcept = delete;

    const Status &operator=(const Status &&s) noexcept = delete;

    static Status OK() { return {}; }

    [[nodiscard]] bool ok() const { return code_ == ErrorCode::kOk && msg_.get() == nullptr; }

    [[nodiscard]] ErrorCode code() const { return code_; }

    void Init(ErrorCode code, const char* msg);

    [[nodiscard]] const char* message() const {
        if(msg_.get() != nullptr) {
            return msg_->c_str();
        } else{
            return nullptr;
        }
    }

    void MoveStatus(Status &s);
    void MoveStatus(Status &&s);

    ErrorCode code_{ErrorCode::kOk};
    UniquePtr<String> msg_{};
};

}
