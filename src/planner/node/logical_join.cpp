//
// Created by JinHai on 2022/9/25.
//

#include "logical_join.h"
#include "common/value.h"
#include <sstream>

namespace infinity {

LogicalJoin::LogicalJoin(int64_t node_id, JoinType join_type, std::vector<std::shared_ptr<BaseExpression>> conditions,
                         const std::shared_ptr<LogicalNode>& left, const std::shared_ptr<LogicalNode>& right)
                         : LogicalNode(node_id, LogicalNodeType::kJoin),
                         join_type_(join_type), conditions_(std::move(conditions)){
    this->set_left_node(left);
    this->set_right_node(right);
}

std::string
LogicalJoin::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << infinity::ToString(join_type_) << " on ";
    for(auto& condition: conditions_) {
        ss << condition->ToString() << " ";
    }
    ss << std::endl;
    ss << left_node_->ToString(space + TAB);
    ss << right_node_->ToString(space + TAB);
    return ss.str();
}

}
