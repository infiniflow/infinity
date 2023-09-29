//
// Created by JinHai on 2022/8/5.
//

#include "cast_expression.h"
#include "common/utility/infinity_assert.h"
#include "function/cast/cast_function.h"

#include <sstream>
#include <utility>

namespace infinity {

SharedPtr<BaseExpression>
CastExpression::AddCastToType(const SharedPtr<BaseExpression>& source_expr_ptr, const DataType& target_type) {
    if(source_expr_ptr->Type() == target_type) {
        return source_expr_ptr;
    }

    if(CastExpression::CanCast(source_expr_ptr->Type(), target_type)) {
        BoundCastFunc cast = CastFunction::GetBoundFunc(source_expr_ptr->Type(), target_type);
        return MakeShared<CastExpression>(cast, source_expr_ptr, target_type);
    } else {
        PlannerError("Can't cast from: " + source_expr_ptr->Type().ToString() + " to" + target_type.ToString());
    }
}

bool
CastExpression::CanCast(const DataType& source, const DataType& target) {
    switch(target.type()) {
        case LogicalType::kNull:
        case LogicalType::kInvalid:
            PlannerError("Invalid data type");
        default:;
    }

    switch(source.type()) {
        case LogicalType::kBoolean:
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kFloat:
        case LogicalType::kDouble:
        case LogicalType::kDecimal:
            switch(target.type()) {
                case LogicalType::kBoolean:
                case LogicalType::kTinyInt:
                case LogicalType::kSmallInt:
                case LogicalType::kInteger:
                case LogicalType::kBigInt:
                case LogicalType::kFloat:
                case LogicalType::kDouble:
                case LogicalType::kDecimal:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kDate:
            switch(target.type()) {
                case LogicalType::kDate:
                case LogicalType::kDateTime:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kTime:
            switch(target.type()) {
                case LogicalType::kTime:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kDateTime:
            switch(target.type()) {
                case LogicalType::kDate:
                case LogicalType::kDateTime:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kInterval:
            switch(target.type()) {
                case LogicalType::kInterval:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        case LogicalType::kVarchar:
            switch(target.type()) {
                case LogicalType::kBoolean:
                case LogicalType::kTinyInt:
                case LogicalType::kSmallInt:
                case LogicalType::kInteger:
                case LogicalType::kBigInt:
                case LogicalType::kFloat:
                case LogicalType::kDouble:
                case LogicalType::kDecimal:
                case LogicalType::kDate:
                case LogicalType::kTime:
                case LogicalType::kDateTime:
                case LogicalType::kInterval:
                case LogicalType::kVarchar:
                    return true;
                default:
                    return false;
            }
        default:
            PlannerError("Invalid data type");
    }
}

String
CastExpression::ToString() const {
    std::stringstream ss;

    ss << "Cast(" << arguments_[0]->Name() << " As " << target_type_.ToString() << ")";

    return ss.str();
}

}
