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

export module infinity_core:physical_insert;

import :stl;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :load_meta;
import :infinity_exception;
import :meta_info;
import internal_types;
import data_type;
import :logger;

namespace infinity {

export class PhysicalInsert : public PhysicalOperator {
public:
    explicit PhysicalInsert(u64 id,
                            SharedPtr<TableInfo> table_info,
                            u64 table_index,
                            Vector<Vector<SharedPtr<BaseExpression>>> value_list,
                            SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr, id, load_metas), table_info_(table_info), table_index_(table_index),
          value_list_(std::move(value_list)) {}

    ~PhysicalInsert() override = default;

    void Init(QueryContext* query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<TableInfo>& table_info() { return table_info_; }
    inline const SharedPtr<TableInfo>& table_info() const { return table_info_; }

    inline const Vector<Vector<SharedPtr<BaseExpression>>> &value_list() const { return value_list_; }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    SharedPtr<TableInfo> table_info_{};
    u64 table_index_{};
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
