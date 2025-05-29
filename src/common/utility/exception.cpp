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

#include <cpptrace/cpptrace.hpp>
#include <cstdlib>
#include <execinfo.h>
#include <iostream>

module infinity_exception;

import stl;
import logger;
import third_party;
import infinity_context;
import new_txn_manager;
import txn_context;

namespace infinity {

void PrintTransactionHistory() {
    NewTxnManager *txn_manager = InfinityContext::instance().storage()->new_txn_manager();
    if (!txn_manager) {
        LOG_WARN("TxnManager is null");
        return;
    }
    Vector<SharedPtr<TxnContext>> txn_contexts = txn_manager->GetTxnContextHistories();

    SizeT history_count = txn_contexts.size();
    for (SizeT idx = 0; idx < history_count; ++idx) {
        SharedPtr<TxnContext> txn_history = txn_contexts[idx];
        LOG_CRITICAL(txn_history->ToString());
    }
}

void PrintStacktrace(const String &err_msg) {
    int trace_stack_depth = 256;
    String trace = cpptrace::generate_trace(0, trace_stack_depth).to_string();
    LOG_CRITICAL(trace);
}

#define ADD_LOG_INFO

#if defined(INFINITY_DEBUG) || defined(ADD_LOG_INFO)

void RecoverableError(Status status, const char *file_name, u32 line) {
    status.AppendMessage(fmt::format("@{}:{}", infinity::TrimPath(file_name), line));
    if (IS_LOGGER_INITIALIZED()) {
        LOG_ERROR(status.message());
    }
    throw RecoverableException(status);
}

std::string_view GetErrorMsg(const String &message) {
    auto pos = message.find_first_of('@', 0);
    return {message.data(), pos};
}

void UnrecoverableError(const String &message, const char *file_name, u32 line) {
    auto *storage = InfinityContext::instance().storage();
    if (storage != nullptr) {
        if (storage->new_txn_manager() != nullptr) {
            infinity::PrintTransactionHistory();
        }
    }
    // if (storage != nullptr) {
    //     CleanupInfoTracer *cleanup_tracer = storage->cleanup_info_tracer();
    //     String error_msg = cleanup_tracer->GetCleanupInfo();
    //     LOG_ERROR(std::move(error_msg));
    // }
    String location_message = fmt::format("{}@{}:{}", message, infinity::TrimPath(file_name), line);
    if (IS_LOGGER_INITIALIZED() && GetPrintStacktrace()) {

        PrintStacktrace(location_message);
    }
    Logger::Flush();
    throw UnrecoverableException(location_message);
}

#else

void RecoverableError(Status status) {
    if (IS_LOGGER_INITIALIZED()) {
        LOG_ERROR(status.message());
    }
    throw RecoverableException(status);
}

void UnrecoverableError(const String &message) {
    if (IS_LOGGER_INITIALIZED()) {
        LOG_CRITICAL(message);
    }
    Logger::Flush();
    throw UnrecoverableException(message);
}

std::string_view GetErrorMsg(const String &message) { return message; }

#endif

} // namespace infinity
