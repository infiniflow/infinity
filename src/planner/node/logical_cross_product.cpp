//
// Created by JinHai on 2022/9/25.
//

#include "logical_cross_product.h"

#include <sstream>

namespace infinity {

LogicalCrossProduct::LogicalCrossProduct(u64 node_id,
                                         String alias,
                                         const SharedPtr<LogicalNode>& left,
                                         const SharedPtr<LogicalNode>& right)
                                         : LogicalNode(node_id, LogicalNodeType::kCrossProduct),
                                         alias_(std::move(alias))
                                         {
    this->set_left_node(left);
    this->set_right_node(right);
}

Vector<ColumnBinding>
LogicalCrossProduct::GetColumnBindings() const {
    Vector<ColumnBinding> left_binding = this->left_node_->GetColumnBindings();
    Vector<ColumnBinding> right_binding = this->right_node_->GetColumnBindings();
    left_binding.insert(left_binding.end(), right_binding.begin(), right_binding.end());
    return left_binding;
}

SharedPtr<Vector<String>>
LogicalCrossProduct::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SharedPtr<Vector<String>> left_output_names = left_node_->GetOutputNames();
    SharedPtr<Vector<String>> right_output_names = right_node_->GetOutputNames();

    result->reserve(left_output_names->size() + right_output_names->size());
    for(auto& name_str: *left_output_names) {
        result->emplace_back(name_str);
    }

    for(auto& name_str: *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}

SharedPtr<Vector<DataType>>
LogicalCrossProduct::GetOutputTypes() const {
    SharedPtr<Vector<DataType>> result = MakeShared<Vector<DataType>>();
    SharedPtr<Vector<DataType>> left_output_names = left_node_->GetOutputTypes();
    SharedPtr<Vector<DataType>> right_output_names = right_node_->GetOutputTypes();
    result->reserve(left_output_names->size() + right_output_names->size());
    for(auto& name_str: *left_output_names) {
        result->emplace_back(name_str);
    }

    for(auto& name_str: *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
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
