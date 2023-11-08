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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;

export module physical_insert;

namespace infinity {

class TableCollectionEntry;

export class PhysicalInsert : public PhysicalOperator {
public:
    explicit
    PhysicalInsert(u64 id, TableCollectionEntry *table_collection_entry, u64 table_index, Vector<Vector<SharedPtr<BaseExpression>>> value_list)
        : PhysicalOperator(PhysicalOperatorType::kInsert, nullptr, nullptr, id), table_collection_entry_(table_collection_entry),
          table_index_(table_index), value_list_(Move(value_list)) {}

    ~PhysicalInsert() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline const TableCollectionEntry *table_collection_entry() const { return table_collection_entry_; }

    inline const Vector<Vector<SharedPtr<BaseExpression>>> &value_list() const { return value_list_; }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    TableCollectionEntry *table_collection_entry_{};
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
