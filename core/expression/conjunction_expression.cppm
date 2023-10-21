//
// Created by JinHai on 2022/8/5.
//

module;

import stl;
import base_expression;
import parser;

export module conjunction_expression;

namespace infinity {

export enum class ConjunctionType {
    kInvalid,
    kAnd,
    kOr,
};

export class ConjunctionExpression : public BaseExpression {
public:
    ConjunctionExpression(ConjunctionType conjunction_type,
                          const SharedPtr<BaseExpression> &left_operand,
                          const SharedPtr<BaseExpression> &right_operand);

    inline DataType Type() const override { return DataType{LogicalType::kBoolean}; }

    String ToString() const override;

    ConjunctionType conjunction_type() const { return conjunction_type_; }

private:
    ConjunctionType conjunction_type_{ConjunctionType::kInvalid};
};

} // namespace infinity