//
// Created by JinHai on 2022/9/12.
//

module;

import stl;
import parser;
import table_ref;
import base_expression;

export module join_table_ref;

namespace infinity {

class BindContext;

export class JoinTableRef : public TableRef {
public:
    explicit JoinTableRef(String alias) : TableRef(TableRefType::kJoin, Move(alias)) {}

    SharedPtr<BindContext> left_bind_context_;
    SharedPtr<BindContext> right_bind_context_;

    SharedPtr<TableRef> left_table_ref_;
    SharedPtr<TableRef> right_table_ref_;

    // Condition ???
    Vector<SharedPtr<BaseExpression>> on_conditions_;

    // Join Type ???
    JoinType join_type_;
};

} // namespace infinity