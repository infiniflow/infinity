//
// Created by JinHai on 2022/8/5.
//

module;

import column_binding;
import parser;
import stl;
import expression_type;
import bound_cast_func;
import base_expression;
// import aggregate_function;

export module cast_expression;

namespace infinity {

export class CastExpression : public BaseExpression {
public:
    CastExpression(BoundCastFunc cast_function, const SharedPtr<BaseExpression> &argument, DataType target_type)
        : BaseExpression(ExpressionType::kCast, {argument}), target_type_(Move(target_type)), func_(cast_function) {}

    inline DataType Type() const override { return target_type_; }

    String ToString() const override;

    static bool CanCast(const DataType &source, const DataType &target);

    static SharedPtr<BaseExpression> AddCastToType(const SharedPtr<BaseExpression> &expr, const DataType &target_type);

    BoundCastFunc func_;

private:
    DataType target_type_;
};

} // namespace infinity
