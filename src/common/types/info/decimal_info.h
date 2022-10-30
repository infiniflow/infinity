//
// Created by JinHai on 2022/10/28.
//

#pragma

#include "common/types/type_info.h"

namespace infinity {

class DecimalInfo : public TypeInfo {
public:

    DecimalInfo() : TypeInfo(TypeInfoType::kDecimal) {}
    ~DecimalInfo() override = default;
private:
};

}

