//
// Created by JinHai on 2022/8/5.
//

#include "value_expression.h"
#include <sstream>

namespace infinity {

String ValueExpression::ToString() const {
    //    std::stringstream ss;
    //    ss << value_.type_.ToString() << '(' << value_.ToString()<< ")";
    //    return ss.str();
    return value_.ToString();
}

} // namespace infinity
