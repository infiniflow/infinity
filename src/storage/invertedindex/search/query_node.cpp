#include "query_node.h"

import status;
import infinity_exception;
import logger;
namespace infinity {

// optimize: from leaf to root, replace tree node in place

// expected property of optimized node:
// 1. children of "and" can only be term or "or", because "and", "not", "and_not" will be optimized
// 2. children of "or" can only be term, "and" or "and_not", because "or" will be optimized, and "not" is not allowed
// 3. children of "not" can only be term, "and" or "and_not", because "not" is not allowed, and "or" will be flattened to not list
// 4. "and_not" does not exist in parser output, it is generated during optimization
//    "and_not": first child can be term, "and", "or", other children form a list of "not"

// 1. deal with "not": "not" must finally combine with parent "and" and turn into "and_not"
// rule for "not":
// 1.1. parent of "not" cannot be "not"
// 1.2. if parent is "and", then "not" should be combined with "and" and turn into "and_not"
// 1.3. if parent is "or", then the children of parent "or" must all be "not", and the "or" should be turned into "not (child and ...)"
// example:
// valid query: "A and (not B)" => "A and_not B"
// valid query: "A and ((not B) and (not C))" => "A and_not (B, C)"
// valid query: "A and ((not B) or (not C))" => "A and_not (B and C)"
// invalid query: "A or (not B)" : can't generate "and_not"
// invalid query: "A and ((not B) or C)" : subexpression "(not B) or C" is invalid
// here it is equivalent to "(A and_not B) or (A and C)", but it is more simple to disallow this case

std::unique_ptr<QueryNode> Not::OptimizeInPlaceInner(std::unique_ptr<QueryNode> &) {
    if (children_.empty()) {
        UnrecoverableError("Invalid query statement: Not node should have at least 1 child");
    }
    std::vector<std::unique_ptr<QueryNode>> new_not_list;
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::TERM:
            case QueryNodeType::AND:
            case QueryNodeType::AND_NOT: {
                new_not_list.emplace_back(std::move(child));
                break;
            }
            case QueryNodeType::OR: {
                auto &or_node = static_cast<Or &>(*child);
                for (auto &or_child : or_node.GetChildren()) {
                    new_not_list.emplace_back(std::move(or_child));
                }
                break;
            }
            default: {
                UnrecoverableError("OptimizeInPlaceInner: Unexpected case!");
                break;
            }
        }
    }
    auto new_not_node = std::make_unique<Not>(); // new node, weight is reset to 1.0
    new_not_node->GetChildren() = std::move(new_not_list);
    return new_not_node;
}

// 2. deal with "and":
// rule for "and" (in execute order)
// 2.1. for all children,
//      if a child is "and" (it is already optimized, can only contain term or "or"), then flatten it into "and list"
//      if a child is term or "or", move it to "and list"
//      if a child is "not", move it to "not list", deal with it later
//      if a child is "and_not", move first child to "and list" (flatten it if it is an "and"), other children to "not list"
// 2.2. build result:
//      all cases:  "and list" | "not list"
//                       Y     |      Y       => build "and_not"
//                       Y     |      N       => build "and"
//                       N     |      Y       => build "not"

std::unique_ptr<QueryNode> And::OptimizeInPlaceInner(std::unique_ptr<QueryNode> &) {
    if (children_.size() < 2) {
        UnrecoverableError("Invalid query statement: And node should have at least 2 children");
    }
    std::vector<std::unique_ptr<QueryNode>> and_list;
    std::vector<std::unique_ptr<QueryNode>> not_list;
    // 2.1.
    for (auto &child : children_) {
        switch (child->GetType()) {
            case QueryNodeType::AND: {
                auto &and_node = static_cast<And &>(*child);
                and_list.insert(and_list.end(),
                                std::make_move_iterator(and_node.GetChildren().begin()),
                                std::make_move_iterator(and_node.GetChildren().end()));
                break;
            }
            case QueryNodeType::TERM:
            case QueryNodeType::OR: {
                and_list.emplace_back(std::move(child));
                break;
            }
            case QueryNodeType::NOT: {
                auto &not_node = static_cast<Not &>(*child);
                not_list.insert(not_list.end(),
                                std::make_move_iterator(not_node.GetChildren().begin()),
                                std::make_move_iterator(not_node.GetChildren().end()));
                break;
            }
            case QueryNodeType::AND_NOT: {
                auto &and_not_node = static_cast<AndNot &>(*child);
                if (auto &first_child = and_not_node.GetChildren().front(); first_child->GetType() == QueryNodeType::AND) {
                    auto &and_node = static_cast<And &>(*first_child);
                    and_list.insert(and_list.end(),
                                    std::make_move_iterator(and_node.GetChildren().begin()),
                                    std::make_move_iterator(and_node.GetChildren().end()));
                } else {
                    and_list.emplace_back(std::move(first_child));
                }
                not_list.insert(not_list.end(),
                                std::make_move_iterator(std::next(and_not_node.GetChildren().begin())),
                                std::make_move_iterator(and_not_node.GetChildren().end()));
            }
            default: {
                UnrecoverableError("OptimizeInPlaceInner: Unexpected case!");
                break;
            }
        }
    }
    // 2.2.
    if (and_list.empty()) {
        // at least 2 children
        auto not_node = std::make_unique<Not>(); // new node, weight is reset to 1.0
        not_node->GetChildren() = std::move(not_list);
        return not_node;
    } else if (not_list.empty()) {
        // at least 2 children
        auto and_node = std::make_unique<And>(); // new node, weight is reset to 1.0
        and_node->GetChildren() = std::move(and_list);
        return and_node;
    } else {
        auto and_not_node = std::make_unique<AndNot>(); // new node, weight is reset to 1.0
        // build first child from and_list
        if (and_list.size() == 1) {
            and_not_node->GetChildren().emplace_back(std::move(and_list.front()));
        } else {
            auto and_node = std::make_unique<And>(); // new node, weight is reset to 1.0
            and_node->GetChildren() = std::move(and_list);
            and_not_node->GetChildren().emplace_back(std::move(and_node));
        }
        // move not_list to and_not_node
        and_not_node->GetChildren().insert(and_not_node->GetChildren().end(),
                                           std::make_move_iterator(not_list.begin()),
                                           std::make_move_iterator(not_list.end()));
        return and_not_node;
    }
}

} // namespace infinity
