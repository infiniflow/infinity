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
import load_meta;

export module physical_project;

namespace infinity {

export class PhysicalProject : public PhysicalOperator {
public:
    explicit PhysicalProject(u64 id,
                             u64 table_index,
                             UniquePtr<PhysicalOperator> left,
                             Vector<SharedPtr<BaseExpression>> expressions,
                             SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kProjection, Move(left), nullptr, id, load_metas), expressions_(Move(expressions)),
          projection_table_index_(table_index) {}

    ~PhysicalProject() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    Vector<SharedPtr<BaseExpression>> expressions_{};

    inline u64 TableIndex() const { return projection_table_index_; }

private:
    //    ExpressionExecutor executor;
    u64 projection_table_index_{};
};

} // namespace infinity
