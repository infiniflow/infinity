//
// Created by jinhai on 23-10-15.
//
module;

import stl;
import infinity_exception;

export module infinity_assert;

namespace infinity {

export template<typename ExceptionType>
inline void Assert(bool is_true, const String& message, const char* file_path, i32 line) {
    if (!(is_true)) {
        String err_msg = message;
#ifdef INFINITY_DEBUG
        err_msg.append(" @").append(file_path).append(":").append(ToStr(line));
#endif
        throw ExceptionType(err_msg);                                                                                                               \
    }
}

export template<typename ExceptionType>
inline void Error(const String& message, const char* file_path, i32 line) {
    return Assert<ExceptionType>(false, message, file_path, line);
}

}
