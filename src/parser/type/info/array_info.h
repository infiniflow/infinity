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

#include <utility>

#include "type/data_type.h"
#include "type/logical_type.h"
#include "type/type_info.h"

namespace infinity {

class ArrayInfo : public TypeInfo {
public:
    static std::shared_ptr<ArrayInfo> Make(DataType elem_type) { return std::make_shared<ArrayInfo>(std::move(elem_type)); }

    explicit ArrayInfo(DataType elem_type) : TypeInfo(TypeInfoType::kArray), elem_type_(std::move(elem_type)) {}

    ~ArrayInfo() override = default;

    bool operator==(const TypeInfo &other) const override;

    // Array always costs 8 bytes. Real data is stored in heap memory.
    [[nodiscard]] size_t Size() const override { return 8u; }

    [[nodiscard]] nlohmann::json Serialize() const override;

    [[nodiscard]] inline std::string ToString() const override { return "array(" + elem_type_.ToString() + ")"; }

private:
    DataType elem_type_;
};

} // namespace infinity
