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

export module physical_merge_limit;

import stl;

import base_expression;
import query_context;
import operator_state;
import physical_operator;
import physical_limit;
import physical_operator_type;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import logger;

namespace infinity {

export class PhysicalMergeLimit final : public PhysicalOperator {
public:
    explicit PhysicalMergeLimit(u64 id,
                                UniquePtr<PhysicalOperator> left,
                                SharedPtr<BaseExpression> limit_expr,
                                SharedPtr<BaseExpression> offset_expr,
                                SharedPtr<Vector<LoadMeta>> load_metas);

    ~PhysicalMergeLimit() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return left_->GetOutputNames(); }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return left_->GetOutputTypes(); }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

private:
    SharedPtr<BaseExpression> limit_expr_{};
    SharedPtr<BaseExpression> offset_expr_{};

    UniquePtr<LimitCounter> counter_{};
};

} // namespace infinity
