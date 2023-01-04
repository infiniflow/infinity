//
// Created by jinhai on 22-12-27.
//

#pragma once

#include "common/default_values.h"
#include "common/types/type_info.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class CharInfo : public TypeInfo {
public:

    static SharedPtr<CharInfo>
    Make(i64 limit);

    static SharedPtr<CharInfo>
    Make();

    explicit
    CharInfo(SizeT limit) : TypeInfo(TypeInfoType::kChar), length_limit_(limit) {}

    ~CharInfo() override = default;

    bool
    operator==(const TypeInfo& other) const override;

    [[nodiscard]] SizeT
    Size() const override {  return length_limit_; }

    [[nodiscard]] SizeT
    length_limit() const { return length_limit_; }

private:
    SizeT length_limit_ {0};
};

}

