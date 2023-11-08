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
import table_collection_entry;
import base_expression;

export module physical_update;

namespace infinity {

export class PhysicalUpdate : public PhysicalOperator {
public:
    explicit PhysicalUpdate(u64 id,
                            SharedPtr<PhysicalOperator> left,
                            TableCollectionEntry *table_entry_ptr,
                            const Vector<Pair<SizeT, SharedPtr<BaseExpression>>> &update_columns)
        : PhysicalOperator(PhysicalOperatorType::kUpdate, left, nullptr, id), table_entry_ptr_(table_entry_ptr), update_columns_(update_columns) {}

    ~PhysicalUpdate() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    TableCollectionEntry *table_entry_ptr_;
    const Vector<Pair<SizeT, SharedPtr<BaseExpression>>> &update_columns_;

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
