//
// Created by JinHai on 2022/8/5.
//

#include "cast_expression.h"
#include "common/utility/infinity_assert.h"

#include <sstream>
#include <utility>

namespace infinity {

SharedPtr<BaseExpression>
CastExpression::AddCastToType(const SharedPtr<BaseExpression>& source_expr_ptr, const DataType &target_type) {
    if (source_expr_ptr->Type() == target_type) {
        return source_expr_ptr;
    }

    if(CastExpression::CanCast(source_expr_ptr->Type(), target_type)) {
        return std::make_shared<CastExpression>(source_expr_ptr, target_type);
    } else {
        PlannerError("Can't cast from: " + source_expr_ptr->Type().ToString() + " to" + target_type.ToString());
    }
}

bool
CastExpression::CanCast(const DataType& source, const DataType& target) {
    return true;
#if 0
    switch(target.GetTypeId()) {
        case LogicalTypeId::kNull:
        case LogicalTypeId::kAny:
        case LogicalTypeId::kInvalid:
            PlannerError("Invalid data type");
        default:
            ;
    }

    switch(source.GetTypeId()) {
        case LogicalTypeId::kBoolean:
        case LogicalTypeId::kTinyInt:
        case LogicalTypeId::kSmallInt:
        case LogicalTypeId::kInteger:
        case LogicalTypeId::kBigInt:
        case LogicalTypeId::kFloat:
        case LogicalTypeId::kDouble:
        case LogicalTypeId::kDecimal:
            switch(target.GetTypeId()) {
                case LogicalTypeId::kBoolean:
                case LogicalTypeId::kTinyInt:
                case LogicalTypeId::kSmallInt:
                case LogicalTypeId::kInteger:
                case LogicalTypeId::kBigInt:
                case LogicalTypeId::kFloat:
                case LogicalTypeId::kDouble:
                case LogicalTypeId::kDecimal:
                case LogicalTypeId::kText:
                    return true;
                default:
                    return false;
            }
        case LogicalTypeId::kDate:
            switch(target.GetTypeId()) {
                case LogicalTypeId::kDate:
                case LogicalTypeId::kDateTime:
                case LogicalTypeId::kVarchar:
                case LogicalTypeId::kText:
                    return true;
                default:
                    return false;
            }
        case LogicalTypeId::kTime:
            switch(target.GetTypeId()) {
                case LogicalTypeId::kTime:
                case LogicalTypeId::kVarchar:
                case LogicalTypeId::kText:
                    return true;
                default:
                    return false;
            }
        case LogicalTypeId::kDateTime:
            switch(target.GetTypeId()) {
                case LogicalTypeId::kDate:
                case LogicalTypeId::kDateTime:
                case LogicalTypeId::kVarchar:
                case LogicalTypeId::kText:
                    return true;
                default:
                    return false;
            }
        case LogicalTypeId::kInterval:
            switch(target.GetTypeId()) {
                case LogicalTypeId::kInterval:
                case LogicalTypeId::kVarchar:
                case LogicalTypeId::kText:
                    return true;
                default:
                    return false;
            }
        case LogicalTypeId::kVarchar:
        case LogicalTypeId::kText:
            switch(target.GetTypeId()) {
                case LogicalTypeId::kBoolean:
                case LogicalTypeId::kTinyInt:
                case LogicalTypeId::kSmallInt:
                case LogicalTypeId::kInteger:
                case LogicalTypeId::kBigInt:
                case LogicalTypeId::kFloat:
                case LogicalTypeId::kDouble:
                case LogicalTypeId::kDecimal:
                case LogicalTypeId::kDate:
                case LogicalTypeId::kTime:
                case LogicalTypeId::kDateTime:
                case LogicalTypeId::kInterval:
                case LogicalTypeId::kVarchar:
                case LogicalTypeId::kText:
                    return true;
                default:
                    return false;
            }
        default:
            PlannerError("Invalid data type");
    }
#endif
}


CastExpression::CastExpression(const SharedPtr<BaseExpression> &argument,
                               DataType data_type)
   : BaseExpression(ExpressionType::kCast, {argument}), target_type_(std::move(data_type)) {}

String
CastExpression::ToString() const {
    std::stringstream ss;

    ss << "Cast(" << arguments_[0]->ToString() << " As " << target_type_.ToString();

    return ss.str();
}

}
