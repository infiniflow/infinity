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

#include "sparse_info.h"

namespace infinity {

bool SparseInfo::operator==(const TypeInfo &other) const {
    if (other.type() != TypeInfoType::kSparse) {
        return false;
    }
    const auto *other_sparse_info = static_cast<const SparseInfo *>(&other);
    return data_type_ == other_sparse_info->data_type_ && index_type_ == other_sparse_info->index_type_ && dimension_ == other_sparse_info->dimension_;
}

nlohmann::json SparseInfo::Serialize() const {
    nlohmann::json res;
    res["data_type"] = data_type_;
    res["index_type"] = index_type_;
    res["dimension"] = dimension_;
    return res;
}

std::unique_ptr<SparseInfo> SparseInfo::Deserialize(const nlohmann::json &json) {
    return std::make_unique<SparseInfo>(json["data_type"], json["index_type"], json["dimension"]);
}

} // namespace infinity