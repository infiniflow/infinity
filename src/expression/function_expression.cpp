//
// Created by JinHai on 2022/8/5.
//

#include "function_expression.h"
#include <sstream>

#include <utility>

namespace infinity {

FunctionExpression::FunctionExpression(ScalarFunction function,
                                       std::vector<std::shared_ptr<BaseExpression>> arguments)
   : BaseExpression(ExpressionType::kFunction, std::move(arguments)), func_(std::move(function)) {}

String
FunctionExpression::ToString() const {
    std::stringstream ss;
    ss << func_.name()<< "(";
    if(arguments_.empty()) {
        ss << ")";
    } else {
        auto arguments_count = arguments_.size();
        for(auto i = 0; i < arguments_count - 1; ++ i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }
    return ss.str();
}

}


