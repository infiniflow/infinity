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

export module infinity_core:index_secondary;

import :index_base;
import :base_table_ref;

import create_index_info;

namespace infinity {

// Does not need any extra member.
export class IndexSecondary final : public IndexBase {
public:
    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names) {
        return std::make_shared<IndexSecondary>(index_name, index_comment, file_name, std::move(column_names));
    }

    IndexSecondary(std::shared_ptr<std::string> index_name,
                   std::shared_ptr<std::string> index_comment,
                   const std::string &file_name,
                   std::vector<std::string> column_names)
        : IndexBase(IndexType::kSecondary, index_name, index_comment, file_name, std::move(column_names)) {}

    ~IndexSecondary() final = default;

    virtual std::string BuildOtherParamsString() const override;

    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name);
};

} // namespace infinity
