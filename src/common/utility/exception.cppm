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

export module infinity_core:infinity_exception;

import :status;

import std;

namespace infinity {

std::atomic_bool print_stacktrace = true;

export bool GetPrintStacktrace() { return print_stacktrace.load(); }
export void SetPrintStacktrace(bool enable) { print_stacktrace.store(enable); }

std::atomic_bool print_transaction_history = true;
export bool GetPrintTransactionHistory() { return print_transaction_history.load(); }
export void SetPrintTransactionHistory(bool enable) { print_transaction_history.store(enable); }

export void PrintStacktrace(const std::string &err_msg);
export void PrintTransactionHistory();

export class RecoverableException : public std::exception {
public:
    explicit RecoverableException(Status status) : status_(std::move(status)) {}
    [[nodiscard]] inline const char *what() const noexcept override { return status_.message(); }

    RecoverableException(const RecoverableException &other) : status_(other.status_.clone()) {}

    inline ErrorCode ErrorCode() const { return status_.code(); }

    inline std::string_view ErrorMessage() const { return status_.message(); }

private:
    Status status_;
};

export class UnrecoverableException : public std::exception {
public:
    explicit UnrecoverableException(std::string message) : message_(std::move(message)) {}
    [[nodiscard]] inline const char *what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

#define ADD_LOG_INFO

#if defined(INFINITY_DEBUG) || defined(ADD_LOG_INFO)

export void RecoverableError(Status status,
                             const char *file_name = std::source_location::current().file_name(),
                             u32 line = std::source_location::current().line());

export void UnrecoverableError(const std::string &message,
                               const char *file_name = std::source_location::current().file_name(),
                               u32 line = std::source_location::current().line());

export std::string_view GetErrorMsg(const std::string &message);

#else

export void RecoverableError(Status status);

export void UnrecoverableError(const std::string &message);

export std::string_view GetErrorMsg(const std::string &message);

#endif

} // namespace infinity
