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

export class IndexSecondary final : public IndexBase {
public:
    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names,
                                           SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality) {
        return std::make_shared<IndexSecondary>(index_name, index_comment, file_name, std::move(column_names), secondary_index_cardinality);
    }

    IndexSecondary(std::shared_ptr<std::string> index_name,
                   std::shared_ptr<std::string> index_comment,
                   const std::string &file_name,
                   std::vector<std::string> column_names,
                   SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality)
        : IndexBase(IndexType::kSecondary, index_name, index_comment, file_name, std::move(column_names)),
          secondary_index_cardinality_(secondary_index_cardinality) {}

    ~IndexSecondary() final = default;

    virtual std::string BuildOtherParamsString() const override;

    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual nlohmann::json Serialize() const override;

    inline SecondaryIndexCardinality GetSecondaryIndexCardinality() const { return secondary_index_cardinality_; }

    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name, SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality);

private:
    SecondaryIndexCardinality secondary_index_cardinality_{SecondaryIndexCardinality::kHighCardinality};
};

} // namespace infinity
