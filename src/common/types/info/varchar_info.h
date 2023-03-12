//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/default_values.h"
#include "common/types/type_info.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class VarcharInfo : public TypeInfo {
public:

    static SharedPtr<VarcharInfo>
    Make(i64 limit);

    static SharedPtr<VarcharInfo>
    Make();

    explicit
    VarcharInfo(SizeT limit) : TypeInfo(TypeInfoType::kVarchar), length_limit_(limit) {}

    ~VarcharInfo() override = default;

    bool
    operator==(const TypeInfo& other) const override;

    // Varchar type costs 16 bytes.
    [[nodiscard]] SizeT
    Size() const override {  return 16u; }

    [[nodiscard]] SizeT
    length_limit() const { return length_limit_; }

    inline void
    UpdateLengthLimit(SizeT new_length_limit) {
        length_limit_ = new_length_limit;
    }

private:
    SizeT length_limit_ {0};
};

}
