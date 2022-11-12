//
// Created by JinHai on 2022/10/28.
//

#pragma

#include "common/types/type_info.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"

namespace infinity {

class DecimalInfo : public TypeInfo {
public:

    static UniquePtr<DecimalInfo>
    Make(LogicalType logical_type, i64 precision, i64 scale);

    static UniquePtr<DecimalInfo>
    Make(i64 precision, i64 scale);

    explicit DecimalInfo(TypeInfoType info_type, i64 precision, i64 scale) :
        TypeInfo(info_type), precision_(precision), scale_(scale) {}

    ~DecimalInfo() override = default;

    [[nodiscard]] i64
    precision() const { return precision_; }

    [[nodiscard]] i64
    scale() const { return scale_; }

private:
    i64 precision_;
    i64 scale_;
};

}

