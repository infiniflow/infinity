//
// Created by jinhai on 23-11-5.
//

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
    void* array[trace_stack_depth];
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
