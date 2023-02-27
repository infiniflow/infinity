//
// Created by jinhai on 23-2-27.
//

#pragma once

#include "base_query_node.h"

namespace infinity {

class SetQueryNode : public BaseQueryNode {
public:
    SetQueryNode() : BaseQueryNode(QueryNodeType::kSet) {}
};

}
