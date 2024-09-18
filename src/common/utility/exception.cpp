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

#include <cstdlib>
#include <execinfo.h>
#include <iostream>

module infinity_exception;

import stl;
import logger;
import third_party;
import infinity_context;
import cleanup_scanner;

namespace infinity {

void PrintStacktrace(const String &err_msg) {
    int trace_stack_depth = 256;
    void *array[256];
    int stack_num = backtrace(array, trace_stack_depth);
    char **stacktrace = backtrace_symbols(array, stack_num);

    LOG_CRITICAL(fmt::format("Error: {}", err_msg));
    for (int i = 0; i < stack_num; ++i) {
        String info = stacktrace[i];
        LOG_CRITICAL(fmt::format("{}, {}", i, info));
    }
    free(stacktrace);
}

#define ADD_LOG_INFO

#if defined(INFINITY_DEBUG) || defined(ADD_LOG_INFO)

void RecoverableError(Status status, const char *file_name, u32 line) {
    status.AppendMessage(fmt::format("@{}:{}", infinity::TrimPath(file_name), line));
    LOG_ERROR(status.message());
    throw RecoverableException(status);
}

std::string_view GetErrorMsg(const String &message) {
    auto pos = message.find_first_of('@', 0);
    return {message.data(), pos};
}

void UnrecoverableError(const String &message, const char *file_name, u32 line) {
    auto *storage = InfinityContext::instance().storage();
    if (storage != nullptr) {
        CleanupInfoTracer *cleanup_tracer = storage->cleanup_info_tracer();
        String error_msg = cleanup_tracer->GetCleanupInfo();
        std::cout << error_msg << std::endl;
        LOG_ERROR(std::move(error_msg));
    }

    PrintStacktrace(message);
    throw UnrecoverableException(fmt::format("{}@{}:{}", message, infinity::TrimPath(file_name), line));
}

#else

void RecoverableError(Status status) {
    LOG_ERROR(status.message());
    throw RecoverableException(status);
}

void UnrecoverableError(const String &message) {
    LOG_CRITICAL(message);
    throw UnrecoverableException(message);
}

std::string_view GetErrorMsg(const String &message) {
    return message;
}

#endif

} // namespace infinity
