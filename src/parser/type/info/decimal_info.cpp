//
// Created by JinHai on 2022/11/29.
//

#include "decimal_info.h"
#include "type/number/decimal_type.h"
#include "spdlog/fmt/fmt.h"
#include <format>

namespace infinity {

std::shared_ptr<DecimalInfo> DecimalInfo::Make(int64_t precision, int64_t scale) {
    if (precision == 0 && scale == 0) {
        precision = 38;
        scale = 38;
    }
    ParserAssert(scale <= precision, fmt::format("Decimal scale {} is greater than precision {}.", scale, precision));
    ParserAssert(precision <= 38, fmt::format("Decimal precision {} is greater than 38.", precision));

    return std::make_shared<DecimalInfo>(precision, scale);
}

bool DecimalInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kDecimal)
        return false;

    auto *decimal_info_ptr = (DecimalInfo *)(&other);

    return this->precision_ == decimal_info_ptr->precision_ && this->scale_ == decimal_info_ptr->scale_;
}

nlohmann::json DecimalInfo::Serialize() const {
    nlohmann::json res;
    res["precision"] = precision_;
    res["scale"] = scale_;
    return res;
}

} // namespace infinity