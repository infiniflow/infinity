// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:index_smve;

import :index_base;
import :base_table_ref;

import third_party;

import create_index_info;
import statement_common;

namespace infinity {

export class IndexSMVE final : public IndexBase {
public:
    static std::shared_ptr<IndexSMVE> Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names,
                                           const std::vector<InitParameter *> &index_param_list);

    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name);

    IndexSMVE(std::shared_ptr<std::string> index_name,
              std::shared_ptr<std::string> index_comment,
              const std::string &file_name,
              std::vector<std::string> column_names,
              u32 width,
              u32 topk)
        : IndexBase(IndexType::kSMVE, std::move(index_name), index_comment, file_name, column_names), width_(width), topk_(topk) {}

public:
    virtual i32 GetSizeInBytes() const override;
    virtual void WriteAdv(char *&ptr) const override;
    virtual std::string ToString() const override;
    virtual std::string BuildOtherParamsString() const override;
    virtual nlohmann::json Serialize() const override;

public:
    u32 width_{2048};
    u32 topk_{8};
};

} // namespace infinity
