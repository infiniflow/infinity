//
// Created by JinHai on 2022/9/12.
//

module;

import stl;
import parser;
import table_ref;

export module subquery_table_ref;

namespace infinity {

class BoundStatement;
class SubqueryTableRef : public TableRef {
public:
    explicit SubqueryTableRef(SharedPtr<BoundStatement> subquery_node, u64 table_index, String alias)
        : TableRef(TableRefType::kSubquery, Move(alias)), subquery_node_(Move(subquery_node)), table_index_(table_index) {}

    SharedPtr<BoundStatement> subquery_node_{nullptr};
    u64 table_index_{};
};

} // namespace infinity