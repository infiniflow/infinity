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
import fusion_expression;
import load_meta;
import infinity_exception;

export module physical_fusion;

namespace infinity {

export class PhysicalFusion final: public PhysicalOperator {
public:
    explicit PhysicalFusion(u64 id,
                            UniquePtr<PhysicalOperator> left,
                            UniquePtr<PhysicalOperator> right,
                            SharedPtr<FusionExpression> fusion_expr,
                            SharedPtr<Vector<LoadMeta>> load_metas);
    ~PhysicalFusion() override;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); };

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); };

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }

    String ToString(i64 &space) const;

    SharedPtr<FusionExpression> fusion_expr_;

private:

};

} // namespace infinity
