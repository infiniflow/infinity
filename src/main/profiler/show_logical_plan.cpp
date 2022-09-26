//
// Created by JinHai on 2022/9/8.
//

#include "show_logical_plan.h"

#include <sstream>
#include <stack>

namespace infinity {

std::string
ShowLogicalPlan::ToString() const {
    std::stringstream ss;
    std::stack<std::shared_ptr<LogicalNode>> stack;
    std::shared_ptr<LogicalNode> root = logical_node_;

    uint64_t intent = 4;
    uint64_t level = -1;

    while(root != nullptr || !stack.empty()) {
        if(root != nullptr) {
            ++ level;
            ss << root->ToString(level * intent);
            stack.push(root);
            root = root->left_node();
        } else {
            -- level;
            root = stack.top()->right_node();
            stack.pop();
        }
    }

    return ss.str();
}

}