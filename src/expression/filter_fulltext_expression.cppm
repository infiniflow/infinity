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
export module filter_fulltext_expression;
import base_expression;
import data_type;
import stl;
import function_expr;

namespace infinity {

export class FilterFulltextExpression final : public BaseExpression {
public:
    FilterFulltextExpression(String fields, String matching_text, String options_text);

    static SharedPtr<FilterFulltextExpression> BuildFilterFulltextExpression(const FunctionExpr &expr);

    String ToString() const override;

    DataType Type() const override;

    String fields_;
    String matching_text_;
    String options_text_;
};

} // namespace infinity
