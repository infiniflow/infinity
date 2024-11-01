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

module;

export module index_emvb;

import stl;

import index_base;
import base_table_ref;
import create_index_info;
import third_party;
import statement_common;

namespace infinity {

export class IndexEMVB final : public IndexBase {
public:
    IndexEMVB(SharedPtr<String> index_name,
              SharedPtr<String> index_comment,
              const String &file_name,
              Vector<String> column_names,
              const u32 residual_pq_subspace_num,
              const u32 residual_pq_subspace_bits)
        : IndexBase(IndexType::kEMVB, std::move(index_name), index_comment, file_name, std::move(column_names)),
          residual_pq_subspace_num_(residual_pq_subspace_num), residual_pq_subspace_bits_(residual_pq_subspace_bits) {}

    static SharedPtr<IndexBase> Make(SharedPtr<String> index_name,
                                     SharedPtr<String> index_comment,
                                     const String &file_name,
                                     Vector<String> column_names,
                                     const Vector<InitParameter *> &index_param_list);

    ~IndexEMVB() override = default;

    String BuildOtherParamsString() const override;

    static void ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name);

    i32 GetSizeInBytes() const override;

    void WriteAdv(char *&ptr) const override;

    String ToString() const override;

    nlohmann::json Serialize() const override;

    const u32 residual_pq_subspace_num_ = 0;
    const u32 residual_pq_subspace_bits_ = 0;
};

} // namespace infinity
