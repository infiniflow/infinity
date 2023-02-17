//
// Created by JinHai on 2022/9/25.
//

#include "logical_cross_product.h"

#include <sstream>

namespace infinity {

LogicalCrossProduct::LogicalCrossProduct(String alias,
                                         u64 join_index,
                                         const SharedPtr<LogicalNode>& left,
                                         const SharedPtr<LogicalNode>& right)
                                         : LogicalNode(LogicalNodeType::kCrossProduct),
                                         alias_(std::move(alias)),
                                         table_index_(join_index)
                                         {
    this->set_left_node(left);
    this->set_right_node(right);
}

Vector<ColumnBinding>
LogicalCrossProduct::GetColumnBindings() const {
    Vector<ColumnBinding> left_binding = this->left_node_->GetColumnBindings();
    Vector<ColumnBinding> right_binding = this->left_node_->GetColumnBindings();
    left_binding.insert(left_binding.end(), right_binding.begin(), right_binding.end());
    return left_binding;
}

String
LogicalCrossProduct::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Cross Product: ";
    space += arrow_str.size();
    return ss.str();
}

}
