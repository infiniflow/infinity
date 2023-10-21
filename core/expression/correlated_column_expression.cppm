//
// Created by JinHai on 2022/8/5.
//

module;

import parser;
import stl;
import base_expression;

export module correlated_column_expression;

namespace infinity {

class CorrelatedColumnExpression : public BaseExpression {
public:
    CorrelatedColumnExpression(DataType data_type, String column_name);

    inline DataType Type() const override { return data_type_; }

    String ToString() const override;

private:
    DataType data_type_;
    String column_name_;
};

} // namespace infinity