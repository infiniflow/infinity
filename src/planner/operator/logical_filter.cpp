//
// Created by JinHai on 2022/8/9.
//

#include "logical_filter.h"

#include <sstream>

namespace infinity {

std::string LogicalFilter::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "Logical Filter: " << expression_->ToString() << std::endl;
    return ss.str();
}

}