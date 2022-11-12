//
// Created by JinHai on 2022/10/28.
//

#pragma once

#include "common/types/type_info.h"
#include "common/types/internal_types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class VarcharInfo : public TypeInfo {
public:

    static inline UniquePtr<VarcharInfo>
    Make(i64 length) {
        TypeAssert(length >= 64, "Varchar length is less than 64, uses other char type will be better.")
        return MakeUnique<VarcharInfo>(length);
    }

    explicit
    VarcharInfo(i64 length) : TypeInfo(TypeInfoType::kVarchar), length_(length) {}

    ~VarcharInfo() override = default;

    [[nodiscard]] inline i64
    length() const { return length_; }

private:
    i64 length_ {0};
};

}
