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

import base_expression;
import bound_select_statement;
import parser;
import stl;
import column_expression;

export module subquery_expression;

namespace infinity {

//class ColumnExpression;

export class SubqueryExpression : public BaseExpression {
public:
    explicit SubqueryExpression(UniquePtr<BoundSelectStatement> select_node, SubqueryType subquery_type);

    String ToString() const override;

    DataType Type() const override;

    UniquePtr<BoundSelectStatement> bound_select_statement_ptr_{};
    SubqueryType subquery_type_;

    // Used in IN subquery, EXISTS won't use it.
    SharedPtr<BaseExpression> left_{nullptr};

    // Correlated column expressions;
    Vector<SharedPtr<ColumnExpression>> correlated_columns{};

private:

};

} // namespace infinity
