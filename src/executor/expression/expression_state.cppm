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

export module infinity_core:expression_state;

import :infinity_type;

import std;

namespace infinity {
export class BaseExpression;
export class AggregateExpression;
export class CaseExpression;
export class CastExpression;
export class ReferenceExpression;
export class FunctionExpression;
export class ValueExpression;
export class InExpression;
export class FilterFulltextExpression;
export class ColumnVector;

export enum class AggregateFlag : i8 {
    kUninitialized = 0,
    kRunning = 1,
    kFinish = 2,
    kRunAndFinish = 3,
};

export class ExpressionState {
public:
    // Static functions
    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<BaseExpression> &expression, char * = nullptr);

    static std::shared_ptr<ExpressionState>
    CreateState(const std::shared_ptr<AggregateExpression> &agg_expr, char *agg_state, const AggregateFlag agg_flag);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<CaseExpression> &agg_expr);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<CastExpression> &agg_expr);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<ReferenceExpression> &agg_expr);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<FunctionExpression> &agg_expr);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<ValueExpression> &agg_expr);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<InExpression> &in_expr);

    static std::shared_ptr<ExpressionState> CreateState(const std::shared_ptr<FilterFulltextExpression> &filter_fulltext_expr);

public:
    void AddChild(const std::shared_ptr<BaseExpression> &expression);

    std::vector<std::shared_ptr<ExpressionState>> &Children() { return children_; }

    std::shared_ptr<ColumnVector> &OutputColumnVector() { return column_vector_; }

    char *agg_state_{};

    AggregateFlag agg_flag_{AggregateFlag::kUninitialized};

private:
    std::vector<std::shared_ptr<ExpressionState>> children_;
    std::string name_;

    // output blocks and each block have one output column;
    std::shared_ptr<ColumnVector> column_vector_{nullptr};
};

} // namespace infinity
