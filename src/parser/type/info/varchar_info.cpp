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

#include "type/info/varchar_info.h"
#include "default_value.h"
#include "parser_assert.h"
#include "type/type_info.h"

namespace infinity {

std::shared_ptr<VarcharInfo> VarcharInfo::Make(int64_t dimension) {
    ParserAssert(dimension <= MAX_VARCHAR_SIZE_INTERNAL, "Varchar dimension should less than " + std::to_string(MAX_VARCHAR_SIZE_INTERNAL));
    return std::make_shared<VarcharInfo>(dimension);
}

bool VarcharInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kVarchar) {
        return false;
    }
    auto varchar_info = static_cast<const VarcharInfo &>(other);
    return varchar_info.dimension_ == this->dimension_;
}

nlohmann::json VarcharInfo::Serialize() const {
    nlohmann::json res;
    res["dimension"] = dimension_;
    return res;
}

} // namespace infinity
