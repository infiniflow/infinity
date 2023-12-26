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
import infinity_exception;

export module physical_nested_loop_join;

namespace infinity {

export class PhysicalNestedLoopJoin : public PhysicalOperator {
public:
    explicit PhysicalNestedLoopJoin(u64 id,
                                    JoinType join_type,
                                    Vector<SharedPtr<BaseExpression>> conditions,
                                    UniquePtr<PhysicalOperator> left,
                                    UniquePtr<PhysicalOperator> right,
                                    SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kJoinNestedLoop, Move(left), Move(right), id, load_metas), join_type_(join_type),
          conditions_(Move(conditions)) {}

    ~PhysicalNestedLoopJoin() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }

    inline const Vector<SharedPtr<BaseExpression>> &conditions() const { return conditions_; }

private:
    SharedPtr<DataTable> left_table_{};
    SharedPtr<DataTable> right_table_{};
    JoinType join_type_{JoinType::kInner};
    Vector<SharedPtr<BaseExpression>> conditions_{};
};

} // namespace infinity