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
import base_expression;
import expression_state;
import data_block;
import selection;

export module expression_selector;

namespace infinity {
class ColumnVector;

export class ExpressionSelector {
public:
    SizeT Select(const SharedPtr<BaseExpression> &expr,
                 SharedPtr<ExpressionState> &state,
                 const DataBlock *input_data_block,
                 DataBlock *output_data_block,
                 SizeT count);

    void Select(const SharedPtr<BaseExpression> &expr,
                SharedPtr<ExpressionState> &state,
                SizeT count,
                const SharedPtr<Selection> &input_select,
                SharedPtr<Selection> &output_true_select,
                SharedPtr<Selection> &output_false_select);

    void Select(const SharedPtr<BaseExpression> &expr, SharedPtr<ExpressionState> &state, SizeT count, SharedPtr<Selection> &output_true_select);

    static void Select(const SharedPtr<ColumnVector> &bool_column, SizeT count, SharedPtr<Selection> &output_true_select, bool nullable);

private:
    const DataBlock *input_data_{nullptr};
};

} // namespace infinity
