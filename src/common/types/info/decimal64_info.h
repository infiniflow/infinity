//
// Created by JinHai on 2022/11/29.
//

#pragma once

#include "common/types/type_info.h"
#include "common/types/logical_type.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class Decimal64Info : public TypeInfo {
public:

    static SharedPtr<Decimal64Info>
    Make(i64 precision, i64 scale);

    explicit Decimal64Info(i64 precision, i64 scale) :
    TypeInfo(TypeInfoType::kDecimal64), precision_(precision), scale_(scale) {
//        GlobalResourceUsage::IncrObjectCount();
    }

    ~Decimal64Info() override {
//        GlobalResourceUsage::DecrObjectCount();
    }

    bool
    operator==(const TypeInfo& other) const override;

    [[nodiscard]] i64
    precision() const { return precision_; }

    [[nodiscard]] i64
    scale() const { return scale_; }

    [[nodiscard]] size_t
    Size() const override { return 8u; }

private:
    i64 precision_;
    i64 scale_;
};

}
