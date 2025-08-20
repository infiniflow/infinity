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

export module infinity_core:index_emvb;

import :index_base;
import :base_table_ref;

import third_party;

import create_index_info;
import statement_common;

namespace infinity {

export class IndexEMVB final : public IndexBase {
public:
    IndexEMVB(std::shared_ptr<std::string> index_name,
              std::shared_ptr<std::string> index_comment,
              const std::string &file_name,
              std::vector<std::string> column_names,
              const u32 residual_pq_subspace_num,
              const u32 residual_pq_subspace_bits)
        : IndexBase(IndexType::kEMVB, std::move(index_name), index_comment, file_name, std::move(column_names)),
          residual_pq_subspace_num_(residual_pq_subspace_num), residual_pq_subspace_bits_(residual_pq_subspace_bits) {}

    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names,
                                           const std::vector<InitParameter *> &index_param_list);

    ~IndexEMVB() override = default;

    std::string BuildOtherParamsString() const override;

    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name);

    i32 GetSizeInBytes() const override;

    void WriteAdv(char *&ptr) const override;

    std::string ToString() const override;

    nlohmann::json Serialize() const override;

    const u32 residual_pq_subspace_num_ = 0;
    const u32 residual_pq_subspace_bits_ = 0;
};

} // namespace infinity
