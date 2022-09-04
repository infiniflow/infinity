//
// Created by JinHai on 2022/9/4.
//

#include "exception.h"

namespace infinity {

std::string
Exception::BuildMessageInternal(std::vector<std::string>& values) {
    auto values_count = values.size();
    if(values_count > 0) {
        std::string msg(values[0]);
        for(size_t idx = 1; idx < values_count; ++ idx) {
            msg += (" " + values[idx]);
        }
        return msg;
    }
    return std::string();
}

}