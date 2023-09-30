//
// Created by JinHai on 2022/9/4.
//

#include "exception.h"
#include "common/types/alias/containers.h"

namespace infinity {

String
Exception::BuildMessageInternal(Vector<String>& values) {
    auto values_count = values.size();
    if(values_count > 0) {
        String msg(values[0]);
        for(size_t idx = 1; idx < values_count; ++idx) {
            msg += (" " + values[idx]);
        }
        return msg;
    }
    return String();
}

}