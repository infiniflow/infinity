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

#pragma once

#include "parser_assert.h"
#include "type/type_info.h"
#include "stats/global_resource_usage.h"
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
