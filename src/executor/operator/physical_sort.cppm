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
import data_table;
import load_meta;

export module physical_sort;

namespace infinity {

export class PhysicalSort : public PhysicalOperator {
public:
    explicit PhysicalSort(u64 id,
                          UniquePtr<PhysicalOperator> left,
                          Vector<SharedPtr<BaseExpression>> expressions,
                          Vector<OrderType> order_by_types,
                          SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kSort, Move(left), nullptr, id, load_metas), expressions_(Move(expressions)),
          order_by_types_(Move(order_by_types)) {}

    ~PhysicalSort() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    void Sort(const SharedPtr<DataTable> &order_by_table, const Vector<OrderType> &order_by_types_);

    static SharedPtr<DataTable> GenerateOutput(const SharedPtr<DataTable> &input_table, const SharedPtr<Vector<RowID>> &rowid_vector);

    Vector<SharedPtr<BaseExpression>> expressions_;
    Vector<OrderType> order_by_types_{};

private:
    SharedPtr<DataTable> GetOrderTable() const;

private:
    SharedPtr<DataTable> input_table_{};
    u64 input_table_index_{};
};

} // namespace infinity
