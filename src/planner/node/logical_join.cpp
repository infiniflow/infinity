//
// Created by JinHai on 2022/9/25.
//

#include "logical_join.h"
#include <sstream>

namespace infinity {

LogicalJoin::LogicalJoin(JoinType join_type, std::vector<std::shared_ptr<BaseExpression>> conditions,
                         const std::shared_ptr<LogicalNode>& left, const std::shared_ptr<LogicalNode>& right,
                         std::shared_ptr<BindContext>& bind_context)
                         : LogicalNode(LogicalNodeType::kJoin, bind_context),
                         join_type_(join_type), conditions_(std::move(conditions)){
    this->set_left_node(left);
    this->set_right_node(right);
}

std::string
LogicalJoin::ToString(int64_t& space) {
    std::stringstream ss;
    std::string arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str << infinity::ToString(join_type_) << " on ";
    for(auto& condition: conditions_) {
        ss << condition->ToString() << " ";
    }
    space += arrow_str.size();
    return ss.str();
}

}
