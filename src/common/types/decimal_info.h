//
// Created by JinHai on 2022/10/28.
//

#pragma

#include "type_info.h"

namespace infinity {

class DecimalInfo : public TypeInfo {
public:

    DecimalInfo() : TypeInfo(TypeInfoType::kDecimal) {}

private:
};

}

