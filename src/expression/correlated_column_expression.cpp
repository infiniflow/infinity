//
// Created by JinHai on 2022/8/5.
//

#include "correlated_column_expression.h"

#include <utility>

namespace infinity {

CorrelatedColumnExpression::CorrelatedColumnExpression(DataType data_type,
                                                       String column_name)
        : BaseExpression(ExpressionType::kCorrelatedColumn, {}),
          data_type_(std::move(data_type)),
          column_name_(std::move(column_name)) {}

String
CorrelatedColumnExpression::ToString() const {
    return {};
}


}

