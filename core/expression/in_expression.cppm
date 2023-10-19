//
// Created by JinHai on 2022/8/4.
//

module;

import column_binding;
import base_expression;
import parser;
import stl;

export module in_expression;
namespace infinity {

export enum class InType {
    kInvalid,
    kIn,
    kNotIn,
};

export class InExpression : public BaseExpression {
public:
    InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, const Vector<SharedPtr<BaseExpression>> &value_list);

    String ToString() const override;

    inline DataType Type() const override { return DataType{LogicalType::kBoolean}; }

    inline const SharedPtr<BaseExpression> &left_operand() const { return left_operand_ptr_; }

    inline SharedPtr<BaseExpression> &left_operand() { return left_operand_ptr_; }

    inline InType in_type() const { return in_type_; }

private:
    SharedPtr<BaseExpression> left_operand_ptr_;
    InType in_type_;
};

} // namespace infinity
