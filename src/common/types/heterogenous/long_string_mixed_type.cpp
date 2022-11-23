//
// Created by JinHai on 2022/11/23.
//

#include "long_string_mixed_type.h"

namespace infinity {

bool
LongStrMixedType::Equal(const String &input) {
    if(this->length != input.length()) return false;

    size_t i = 0;
    for(; i < length; ++ i) {
        if(i == BaseMixedType::LONG_STR_HEADER) {
            break;
        }
        if(this->header[i] != input[i]) return false;
    }

    for(; i < length; ++ i) {
        if(this->ptr[i] != input[i]) return false;
    }
    return true;
}

}