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

#include <execinfo.h>
#include <cstdlib>

import stl;
import logger;
import third_party;

module infinity_exception;

namespace infinity {

void
PrintStacktrace(const String& err_msg) {
    int trace_stack_depth = 256;
    void* array[256];
    int stack_num = backtrace(array, trace_stack_depth);
    char** stacktrace = backtrace_symbols(array, stack_num);

    LOG_CRITICAL(Format("Error: {}", err_msg));
    for (int i = 0; i < stack_num; ++i) {
        String info = stacktrace[i];
        LOG_CRITICAL(Format("{}, {}", i, info));
    }
    free(stacktrace);
}

void Assert(bool is_true,
                   const String &message,
                   const char *file_name,
                   u32 line) {
    if (!(is_true)) {
        String err_msg = message;
        err_msg.append(" @").append(infinity::TrimPath(file_name)).append(":").append(ToStr(line));
        PrintStacktrace(err_msg);
        throw Exception(err_msg);
    }
}

void
Error(const String &message, const char *file_name, u32 line) {
    return Assert(false, message, file_name, line);
}

}
