//
// Created by jinhai on 23-1-17.
//

module;

import stl;
import logical_node;
import query_context;
import parser;

export module bound_statement;

namespace infinity {

export struct BoundStatement {
public:
    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<SharedPtr<DataType>>> types_ptr_{};

    virtual SharedPtr<LogicalNode> BuildPlan(QueryContext *query_context) = 0;
};

} // namespace infinity
