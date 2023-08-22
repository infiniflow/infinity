//
// Created by JinHai on 2022/10/28.
//

#include "type_info.h"

namespace infinity {

bool
TypeInfo::operator!=(const TypeInfo &other) const {
    return !operator==(other);
}

}
