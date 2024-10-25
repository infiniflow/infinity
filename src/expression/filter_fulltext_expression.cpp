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

module filter_fulltext_expression;

import base_expression;
import expression_type;
import data_type;
import stl;
import logical_type;
import third_party;
import infinity_exception;
import status;
import parsed_expr;
import function_expr;
import constant_expr;

namespace infinity {

FilterFulltextExpression::FilterFulltextExpression(String fields, String matching_text, String options_text)
    : BaseExpression(ExpressionType::kFilterFullText, Vector<SharedPtr<BaseExpression>>()), fields_(std::move(fields)),
      matching_text_(std::move(matching_text)), options_text_(std::move(options_text)) {}

String FilterFulltextExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    return fmt::format("FILTER_FULLTEXT('{}', '{}'{})", fields_, matching_text_, options_text_.empty() ? "" : fmt::format(", '{}'", options_text_));
}

DataType FilterFulltextExpression::Type() const { return DataType{LogicalType::kBoolean}; }

SharedPtr<FilterFulltextExpression> FilterFulltextExpression::BuildFilterFulltextExpression(const FunctionExpr &expr) {
    if (!expr.arguments_ || expr.arguments_->size() < 2 || expr.arguments_->size() > 3) {
        RecoverableError(Status::SyntaxError("FILTER_FULLTEXT() requires 2 or 3 arguments"));
    }
    auto get_str_from_expr = [](const ParsedExpr *pared_expr) -> String {
        if (pared_expr->type_ != ParsedExprType::kConstant) {
            RecoverableError(Status::SyntaxError("FILTER_FULLTEXT() arguments must be constant strings"));
        }
        auto *const_expr = static_cast<const ConstantExpr *>(pared_expr);
        if (const_expr->literal_type_ != LiteralType::kString) {
            RecoverableError(Status::SyntaxError("FILTER_FULLTEXT() arguments must be constant strings"));
        }
        return const_expr->ToString();
    };
    auto fields = get_str_from_expr((*expr.arguments_)[0]);
    auto matching_text = get_str_from_expr((*expr.arguments_)[1]);
    auto options_text = expr.arguments_->size() < 3 ? "" : get_str_from_expr((*expr.arguments_)[2]);
    return MakeShared<FilterFulltextExpression>(std::move(fields), std::move(matching_text), std::move(options_text));
}

u64 FilterFulltextExpression::Hash() const {
    u64 h = 0;
    h ^= std::hash<String>()(fields_);
    h ^= std::hash<String>()(matching_text_);
    h ^= std::hash<String>()(options_text_);
    return h;
}

bool FilterFulltextExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kFilterFullText) {
        return false;
    }
    const auto &other = static_cast<const FilterFulltextExpression &>(other_base);
    return std::strcmp(fields_.c_str(), other.fields_.c_str()) == 0 && std::strcmp(matching_text_.c_str(), other.matching_text_.c_str()) == 0 &&
           std::strcmp(options_text_.c_str(), other.options_text_.c_str()) == 0;
}

} // namespace infinity
