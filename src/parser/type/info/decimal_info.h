//
// Created by JinHai on 2022/11/29.
//

#pragma once

#include "parser_assert.h"
#include "type/type_info.h"
#include <memory>

namespace infinity {

class DecimalInfo : public TypeInfo {
public:
    static std::shared_ptr<DecimalInfo> Make(int64_t precision, int64_t scale);

    explicit DecimalInfo(int64_t precision, int64_t scale) : TypeInfo(TypeInfoType::kDecimal), precision_(precision), scale_(scale) {
        //        GlobalResourceUsage::IncrObjectCount();
    }

    ~DecimalInfo() override {
        //        GlobalResourceUsage::DecrObjectCount();
    }

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] int64_t precision() const { return precision_; }

    [[nodiscard]] int64_t scale() const { return scale_; }

    [[nodiscard]] size_t Size() const override { return 16u; }

    [[nodiscard]] nlohmann::json Serialize() const override;

    [[nodiscard]] inline std::string ToString() const override {
        return "decimal(" + std::to_string(precision_) + ", " + std::to_string(scale_) + ")";
    }

private:
    int64_t precision_;
    int64_t scale_;
};

} // namespace infinity
