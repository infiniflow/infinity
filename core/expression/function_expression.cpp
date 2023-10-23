//
// Created by JinHai on 2022/8/5.
//

module;

#include <sstream>

import stl;
import expression_type;
import parser;
import scalar_function;

module function_expression;

namespace infinity {

FunctionExpression::FunctionExpression(ScalarFunction function, Vector<SharedPtr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kFunction, std::move(arguments)), func_(std::move(function)) {}

String FunctionExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    std::stringstream ss;
    if (arguments_.size() == 1) {
        // Unary argument function
        ss << func_.name() << '(' << arguments_.at(0)->Name() << ")";
        return ss.str();
    }

    if (arguments_.size() == 2) {
        // Binary argument function
        ss << '(' << arguments_.at(0)->Name() << " " << func_.name() << " " << arguments_.at(1)->Name() << ")";
        return ss.str();
    }

    ss << func_.name() << "(";
    if (arguments_.empty()) {
        ss << ")";
    } else {
        auto arguments_count = arguments_.size();
        for (auto i = 0; i < arguments_count - 1; ++i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }
    return ss.str();
}

} // namespace infinity
