//
// Created by JinHai on 2022/9/25.
//

#include "logical_cross_product.h"
#include "common/value.h"

#include <sstream>

namespace infinity {

LogicalCrossProduct::LogicalCrossProduct(const std::shared_ptr<LogicalNode>& left,
                                         const std::shared_ptr<LogicalNode>& right,
                                         std::shared_ptr<BindContext>& bind_context)
                                         : LogicalNode(LogicalNodeType::kCrossProduct, bind_context)
                                         {
    this->set_left_node(left);
    this->set_right_node(right);
}

std::string LogicalCrossProduct::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << "Cross Product: " << std::endl;
    space += arrow_str.size();
    return ss.str();
}

}
