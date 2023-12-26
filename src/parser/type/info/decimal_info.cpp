// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "decimal_info.h"
#include "spdlog/fmt/fmt.h"
#include "type/number/decimal_type.h"

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
