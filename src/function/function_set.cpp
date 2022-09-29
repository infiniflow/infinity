//
// Created by JinHai on 2022/9/15.
//

#include "function_set.h"
#include "main/infinity.h"

#include <sstream>

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
        ss << "()" << std::endl;

    } else {
        ss << "(";
        for(auto i = 0; i < argument_count - 1; ++ i) {
            ss << arguments[i]->DataType().ToString() << ", ";
        }
        ss << arguments.back()->DataType().ToString();
        ss << ")" << std::endl;
    }

    return ss.str();
}

void
AggregateFunctionSet::AddFunction(const infinity::AggregateFunction& func) {
    functions_.emplace_back(func);
}

AggregateFunction
AggregateFunctionSet::GetMostMatchFunction(const std::vector<std::shared_ptr<BaseExpression>> &input_arguments) {
    PlannerError("not implemented.");
}

void
ScalarFunctionSet::AddFunction(const infinity::ScalarFunction& func) {
    functions_.emplace_back(func);
}

ScalarFunction
ScalarFunctionSet::GetMostMatchFunction(const std::vector<std::shared_ptr<BaseExpression>> &input_arguments) {


    int64_t lowest_cost = std::numeric_limits<int64_t>::max();
    size_t function_count = functions_.size();
    std::vector<int64_t> candidates_index;

    for(auto i = 0; i < function_count; ++ i) {
        ScalarFunction& function = functions_[i];
        int64_t cost = MatchFunctionCost(function, input_arguments);
        if(cost >= 0 && cost <= lowest_cost) {
            // Have matched function and may be one of the candidate
            if(cost == lowest_cost) {
                candidates_index.emplace_back(i);
                continue;
            }
            lowest_cost = cost;
            candidates_index.clear();
        }
    }

    if(candidates_index.empty()) {
        // No matched function
        std::stringstream ss;
        ss << "Can't find matched function for " << FunctionSet::ToString(name_, input_arguments) << std::endl;
        ss << "Candidate functions: " << std::endl;
        for(auto& function: functions_) {
            ss << function.ToString() << std::endl;
        }
        PlannerError(ss.str());
    }

    if(candidates_index.size() > 1) {
        // multiple functions matched
        std::stringstream ss;
        ss << "Multiple matched functions for " << FunctionSet::ToString(name_, input_arguments) << std::endl;
        ss << "Matched candidate functions: " << std::endl;
        for(auto index: candidates_index) {
            ss << functions_[index].ToString() << std::endl;
        }
        PlannerError(ss.str());
    }

    return functions_[candidates_index[0]];
}

int64_t
ScalarFunctionSet::MatchFunctionCost(const ScalarFunction& func,
                                     const std::vector<std::shared_ptr<BaseExpression>>& arguments) {
    // TODO: variable argument list function need to handled here.

    if(func.parameter_types_.size() != arguments.size()) {
        // Argument count is mismatched.
        return -1; // Invalid function index
    }

    auto argument_count = arguments.size();
    int64_t total_cost = 0;
    for(auto i = 0; i < argument_count; ++ i) {
        // Get the cost from argument to parameter
        int64_t type_cast_cost = LogicalType::CastRule(arguments[i]->DataType().GetTypeId(), func.parameter_types_[i].GetTypeId());
        if(type_cast_cost < 0) {
            // Can't cast the value type;
            return -1;
        } else {
            total_cost += type_cast_cost;
        }
    }

    return total_cost;
}

void
TableFunctionSet::AddFunction(const infinity::TableFunction& func) {
    functions_.emplace_back(func);
}

}

