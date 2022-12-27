//
// Created by JinHai on 2022/9/15.
//

#include "function_set.h"
#include "main/infinity.h"

#include <sstream>
//#include <format>
#include <iostream>

namespace infinity {

std::shared_ptr<FunctionSet>
FunctionSet::GetFunctionSet(const hsql::Expr &expr) {
    std::string function_name = expr.name;

    // Transfer the function to upper case.
    std::transform(function_name.begin(), function_name.end(), function_name.begin(), [](const auto c) {
        return std::toupper(c);
    });

    // std::unique_ptr<Catalog>& catalog
    auto& catalog = Infinity::instance().catalog();
    std::shared_ptr<FunctionSet> function_set_ptr = catalog->GetFunctionSetByName(function_name);

    return function_set_ptr;
}

std::string
FunctionSet::ToString(const std::string& name, const std::vector<std::shared_ptr<BaseExpression>>& arguments) {
    std::stringstream ss;

    ss << name;
    auto argument_count = arguments.size();
    if(argument_count == 0) {
        ss << "()";

    } else {
        ss << "(";
        for(auto i = 0; i < argument_count - 1; ++ i) {
            ss << arguments[i]->Type().ToString() << ", ";
        }
        ss << arguments.back()->Type().ToString();
        ss << ")";
    }

    return ss.str();
}





}

