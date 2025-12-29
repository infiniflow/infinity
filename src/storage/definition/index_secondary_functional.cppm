// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:index_secondary_functional;

import :index_base;
import :base_table_ref;

import create_index_info;
import :base_expression;
import :function_expression;

namespace infinity {

export class IndexSecondaryFunctional final : public IndexBase {
public:
    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names,
                                           std::shared_ptr<std::string> func_col_params,
                                           DataType func_return_type,
                                           std::shared_ptr<std::string> function_expression_str,
                                           SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality) {
        return std::make_shared<IndexSecondaryFunctional>(index_name,
                                                          index_comment,
                                                          file_name,
                                                          std::move(column_names),
                                                          func_col_params,
                                                          func_return_type,
                                                          function_expression_str,
                                                          secondary_index_cardinality);
    }

    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                           std::shared_ptr<std::string> index_comment,
                                           const std::string &file_name,
                                           std::vector<std::string> column_names,
                                           std::shared_ptr<std::string> func_col_params,
                                           DataType func_return_type,
                                           SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality) {
        return std::make_shared<IndexSecondaryFunctional>(index_name,
                                                          index_comment,
                                                          file_name,
                                                          std::move(column_names),
                                                          func_col_params,
                                                          func_return_type,
                                                          nullptr,
                                                          secondary_index_cardinality);
    }

    IndexSecondaryFunctional(std::shared_ptr<std::string> index_name,
                             std::shared_ptr<std::string> index_comment,
                             const std::string &file_name,
                             std::vector<std::string> column_names,
                             std::shared_ptr<std::string> func_col_params,
                             DataType func_return_type,
                             std::shared_ptr<std::string> function_expression_str,
                             SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality)
        : IndexBase(IndexType::kSecondaryFunctional, index_name, index_comment, file_name, std::move(column_names)),
          secondary_index_cardinality_(secondary_index_cardinality), func_col_params_(func_col_params), func_return_type_(func_return_type),
          function_expression_json_str_(function_expression_str) {}

    ~IndexSecondaryFunctional() final = default;

    virtual std::string BuildOtherParamsString() const override;

    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual nlohmann::json Serialize() const override;

    virtual std::string ToString() const override;

    inline SecondaryIndexCardinality GetSecondaryIndexCardinality() const { return secondary_index_cardinality_; }
    inline DataType GetFuncReturnType() const { return func_return_type_; }
    inline std::shared_ptr<std::string> GetFuncColParams() const { return func_col_params_; }
    inline std::shared_ptr<std::string> GetFunctionExpressionJsonStr() const { return function_expression_json_str_; }
    inline void SetFunctionExpressionJsonStr(std::shared_ptr<std::string> function_expression_json_str) {
        function_expression_json_str_ = function_expression_json_str;
    }

    static void ValidateColumnAndReturnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref,
                                                const std::string &column_name,
                                                DataType &data_type,
                                                SecondaryIndexCardinality secondary_index_cardinality = SecondaryIndexCardinality::kHighCardinality);

private:
    SecondaryIndexCardinality secondary_index_cardinality_{SecondaryIndexCardinality::kHighCardinality};
    std::shared_ptr<std::string> func_col_params_{};
    DataType func_return_type_{LogicalType::kInvalid};
    std::shared_ptr<std::string> function_expression_json_str_{};
};

} // namespace infinity
