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

    int64_t intent = -4;

    while(root != nullptr || !stack.empty()) {
        if(root != nullptr) {
            intent += 4;
            ss << root->ToString(intent) << std::endl;
            stack.push(root);
            root = root->left_node();
        } else {
            root = stack.top()->right_node();
            if(root != nullptr) {
                intent -= 4;
            }
            stack.pop();
        }
    }

    return ss.str();
}

}