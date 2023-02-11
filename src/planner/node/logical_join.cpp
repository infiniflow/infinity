//
// Created by JinHai on 2022/9/25.
//

#include "logical_join.h"
#include <sstream>

namespace infinity {

LogicalJoin::LogicalJoin(JoinType join_type,
                         String alias,
                         u64 join_index,
                         Vector<SharedPtr<BaseExpression>> conditions,
                         const SharedPtr<LogicalNode>& left,
                         const SharedPtr<LogicalNode>& right)
                         : LogicalNode(LogicalNodeType::kJoin),
                         join_type_(join_type),
                         alias_(std::move(alias)),
                         table_index_(join_index),
                         conditions_(std::move(conditions)) {
    this->set_left_node(left);
    this->set_right_node(right);
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
        ss << condition->ToString() << " ";
    }
    space += arrow_str.size();
    return ss.str();
}

}
