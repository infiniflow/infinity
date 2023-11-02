//
// Created by jinhai on 23-1-18.
//

module;

import bound_statement;
import table_ref;
import base_expression;
import bind_context;
import logical_node;
import query_context;
import stl;
import std;

export module bound_delete_statement;

namespace infinity {

// class BindContext;
// class QueryContext;
// class BaseExpression;

export struct BoundDeleteStatement final : public BoundStatement {
public:
    static inline SharedPtr<BoundDeleteStatement> Make(SharedPtr<BindContext> bind_context) {
        return MakeShared<BoundDeleteStatement>(std::move(bind_context));
    }

public:
    inline explicit BoundDeleteStatement(SharedPtr<BindContext> bind_context) : bind_context_(std::move(bind_context)) {}

    SharedPtr<LogicalNode> BuildPlan(QueryContext *query_context) final;

    SharedPtr<LogicalNode> BuildFrom(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context);

    SharedPtr<LogicalNode> BuildBaseTable(SharedPtr<TableRef> &table_ref, QueryContext *query_context, const SharedPtr<BindContext> &bind_context);

    SharedPtr<LogicalNode> BuildFilter(SharedPtr<LogicalNode> &root,
                                       Vector<SharedPtr<BaseExpression>> &conditions,
                                       QueryContext *query_context,
                                       const SharedPtr<BindContext> &bind_context);

    void BuildSubquery(SharedPtr<LogicalNode> &root,
                       SharedPtr<BaseExpression> &condition,
                       QueryContext *query_context,
                       const SharedPtr<BindContext> &bind_context);

    SharedPtr<BaseExpression> UnnestSubquery(SharedPtr<LogicalNode> &root,
                                             SharedPtr<BaseExpression> &condition,
                                             QueryContext *query_context,
                                             const SharedPtr<BindContext> &bind_context);

public:
    SharedPtr<BindContext> bind_context_{};

    // From clause
    SharedPtr<TableRef> table_ref_ptr_{};

    // Where conditions
    Vector<SharedPtr<BaseExpression>> where_conditions_{};

    // For build subquery
    bool building_subquery_{false};
};

} // namespace infinity
