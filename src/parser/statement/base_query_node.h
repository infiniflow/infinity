//
// Created by jinhai on 23-2-27.
//

#pragma once

namespace infinity {

enum class QueryNodeType {
    kSelect,
    kSet,
};

class BaseQueryNode {
public:
    explicit
    BaseQueryNode(QueryNodeType type) : node_type_(type) {}
    QueryNodeType node_type_{QueryNodeType::kSelect};
};

}
