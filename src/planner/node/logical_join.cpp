//
// Created by JinHai on 2022/9/25.
//

#include "logical_join.h"
#include <sstream>

namespace infinity {

LogicalJoin::LogicalJoin(u64 node_id,
                         JoinType join_type,
                         String alias,
                         u64 join_index,
                         Vector<SharedPtr<BaseExpression>> conditions,
                         const SharedPtr<LogicalNode>& left,
                         const SharedPtr<LogicalNode>& right)
                         : LogicalNode(node_id, LogicalNodeType::kJoin),
                         join_type_(join_type),
                         alias_(std::move(alias)),
                         table_index_(join_index),
                         conditions_(std::move(conditions)) {
    this->set_left_node(left);
    this->set_right_node(right);
}

Vector<ColumnBinding>
LogicalJoin::GetColumnBindings() const {
    Vector<ColumnBinding> result_binding;
    if(join_type_ == JoinType::kMark) {
        result_binding.emplace_back(table_index_, 0);
    }
    Vector<ColumnBinding> left_binding = this->left_node_->GetColumnBindings();
    Vector<ColumnBinding> right_binding = this->right_node_->GetColumnBindings();
    result_binding.insert(result_binding.end(), left_binding.begin(), left_binding.end());
    result_binding.insert(result_binding.end(), right_binding.begin(), right_binding.end());
    return result_binding;
}

SharedPtr<Vector<String>>
LogicalJoin::GetOutputNames() const {
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

String
LogicalJoin::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << infinity::ToString(join_type_) << " on ";
    for(auto& condition: conditions_) {
        ss << condition->Name() << " ";
    }
    space += arrow_str.size();
    return ss.str();
}

}
