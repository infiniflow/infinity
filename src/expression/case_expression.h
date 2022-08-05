//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class CaseExpression : public BaseExpression {
public:
    CaseExpression(const std::shared_ptr<BaseExpression>& when,
                   const std::shared_ptr<BaseExpression>& then,
                   const std::shared_ptr<BaseExpression>& otherwise);
    LogicalType DataType() override;
    std::string ToString() const override;
private:

};

}
