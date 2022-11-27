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

    static inline UniquePtr<VarcharInfo>
    Make(i64 limit) {
        TypeAssert(limit >= 64 && limit <= DEFAULT_VARCHAR_SIZE,
                   "Varchar length can't be less than 64 or larger than 65535.")
        return MakeUnique<VarcharInfo>(limit);
    }

    static inline UniquePtr<VarcharInfo>
    Make() {
        return MakeUnique<VarcharInfo>(DEFAULT_VARCHAR_SIZE);
    }

    explicit
    VarcharInfo(size_t limit) : TypeInfo(TypeInfoType::kVarchar), length_limit_(limit) {}

    ~VarcharInfo() override = default;

    // Varchar type costs 16 bytes.
    [[nodiscard]] size_t
    Size() const override {  return 16u; }

    [[nodiscard]] size_t
    length_limit() const { return length_limit_; }

private:
    size_t length_limit_ {0};
};

}
