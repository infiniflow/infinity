//
// Created by JinHai on 2022/8/5.
//

module;

import stl;
import base_expression;
import parser;
import scalar_function;

export module function_expression;

namespace infinity {

export class FunctionExpression : public BaseExpression {
public:
    FunctionExpression(ScalarFunction function, Vector<SharedPtr<BaseExpression>> arguments);

    inline DataType Type() const override { return func_.return_type(); }

    String ToString() const override;

public:
    ScalarFunction func_;
};

} // namespace infinity
