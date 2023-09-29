//
// Created by JinHai on 2022/12/14.
//

#include "scalar_function_set.h"
#include <sstream>

namespace infinity {

ScalarFunctionSet::~ScalarFunctionSet() {
    functions_.clear();
}

void
ScalarFunctionSet::AddFunction(const ScalarFunction& func) {
    functions_.emplace_back(func);
}

ScalarFunction
ScalarFunctionSet::GetMostMatchFunction(const Vector<SharedPtr<BaseExpression>> &input_arguments) {

    i64 lowest_cost = std::numeric_limits<i64>::max();
    SizeT function_count = functions_.size();
    Vector<i64> candidates_index;

    for(auto i = 0; i < function_count; ++ i) {
        ScalarFunction& function = functions_[i];
        i64 cost = MatchFunctionCost(function, input_arguments);
        if(cost >= 0 && cost <= lowest_cost) {
            // Have matched function and may be one of the candidate
            if(cost == lowest_cost) {
                candidates_index.emplace_back(i);
                continue;
            }
            lowest_cost = cost;
            candidates_index.clear();
            candidates_index.emplace_back(i);
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

i64
ScalarFunctionSet::MatchFunctionCost(const ScalarFunction& func,
                                     const Vector<SharedPtr<BaseExpression>>& arguments) {
    // TODO: variable argument list function need to handled here.

    if(func.parameter_types_.size() != arguments.size()) {
        // Argument count is mismatched.
        return -1; // Invalid function index
    }

    auto argument_count = arguments.size();
    i64 total_cost = 0;
    for(auto i = 0; i < argument_count; ++ i) {
        // Get the cost from argument to parameter
        i64 type_cast_cost = DataType::CastRule(arguments[i]->Type(), func.parameter_types_[i]);
        if(type_cast_cost < 0) {
            // Can't cast the value type;
            return -1;
        } else {
            total_cost += type_cast_cost;
        }
    }

    return total_cost;
}

}