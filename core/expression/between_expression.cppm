//
// Created by JinHai on 2022/8/5.
//

module;

import column_binding;
import base_expression;
import parser;
import stl;

export module between_expression;

namespace infinity {

export class BetweenExpression : public BaseExpression {
public:
    BetweenExpression(const SharedPtr<BaseExpression> &value,
                      const SharedPtr<BaseExpression> &upper_bound,
                      const SharedPtr<BaseExpression> &lower_bound,
                      bool upper_inclusive,
                      bool lower_inclusive);

    String ToString() const override;

    DataType Type() const override;

private:
    bool lower_inclusive_{false};
    bool upper_inclusive_{false};
};

} // namespace infinity
