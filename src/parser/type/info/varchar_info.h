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

#include "type/type_info.h"
#include <memory>

namespace infinity {

class VarcharInfo : public TypeInfo {
public:
    static std::shared_ptr<VarcharInfo> Make(int64_t dimension);

    explicit VarcharInfo(int64_t dimension) : TypeInfo(TypeInfoType::kVarchar), dimension_(dimension) {
        // GlobalResourceUsage::IncrObjectCount();
    }

    ~VarcharInfo() override {
        // GlobalResourceUsage::DecrObjectCount();
    }

    bool operator==(const TypeInfo &other) const override;

    [[nodiscard]] int64_t dimension() const { return dimension_; }

    [[nodiscard]] size_t Size() const override { return 16u; }

    [[nodiscard]] nlohmann::json Serialize() const override;

    [[nodiscard]] inline std::string ToString() const override { return "varchar(" + std::to_string(dimension_) + ")"; }

private:
    int64_t dimension_;
};
} // namespace infinity