//
// Created by JinHai on 2022/11/23.
//

#include "short_string_mixed_type.h"

namespace infinity {

bool
ShortStrMixedType::Equal(const String& input) {
    if(length != input.length())
        return false;
    for(size_t i = 0; const auto& c: input) {
        if(this->ptr[i++] != c)
            return false;
    }
    return true;
}

}