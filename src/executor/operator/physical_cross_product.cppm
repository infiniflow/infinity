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
import data_table;

export module physical_cross_product;

namespace infinity {

export class PhysicalCrossProduct final : public PhysicalOperator {
public:
    explicit PhysicalCrossProduct(u64 id, SharedPtr<PhysicalOperator> left, SharedPtr<PhysicalOperator> right)
        : PhysicalOperator(PhysicalOperatorType::kCrossProduct, Move(left), Move(right), id) {}

    ~PhysicalCrossProduct() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

private:
    SharedPtr<DataTable> left_table_{};
    SharedPtr<DataTable> right_table_{};
};

} // namespace infinity
