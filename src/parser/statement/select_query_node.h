//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "base_query_node.h"

namespace infinity {

class SelectQueryNode : public BaseQueryNode {
public:
    SelectQueryNode() : BaseQueryNode(QueryNodeType::kSelect) {}
};

}

