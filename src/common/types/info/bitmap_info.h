//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "common/default_values.h"
#include "common/types/type_info.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class BitmapInfo : public TypeInfo {
public:

    static SharedPtr<BitmapInfo>
    Make(i64 limit);

    static SharedPtr<BitmapInfo>
    Make();

    explicit
    BitmapInfo(SizeT limit) : TypeInfo(TypeInfoType::kBitmap), length_limit_(limit) {}

    ~BitmapInfo() override = default;

    bool
    operator==(const TypeInfo& other) const override;

    [[nodiscard]] SizeT
    Size() const override {  return length_limit_; }

    [[nodiscard]] SizeT
    length_limit() const { return length_limit_; }

    [[nodiscard]] nlohmann::json
    Serialize() const override;

private:
    SizeT length_limit_ {0};
};

}
