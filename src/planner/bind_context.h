//
// Created by JinHai on 2022/8/7.
//

#pragma once

#include "SQLParserResult.h"
#include "expression/base_expression.h"
#include "column_identifier.h"
#include "binding.h"


#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace infinity {

class LogicalNode;
class Table;
class ExpressionBinder;
class QueryContext;

struct CommonTableExpressionInfo {
    CommonTableExpressionInfo(String alias, hsql::SelectStatement* select_stmt, std::unordered_set<String> masked_name_set)
        : alias_(std::move(alias)), select_statement_(select_stmt), masked_name_set_(std::move(masked_name_set)) {}

    String alias_;
    hsql::SelectStatement* select_statement_;
    std::unordered_set<String> masked_name_set_;
};

class BindContext {
public:
    explicit BindContext(SharedPtr<BindContext> parent)
        : parent_(std::move(parent)) {
        binding_context_id_ = GenerateBindingContextIndex();
    }

    virtual ~BindContext();

    void Destroy();

    // Parent bind context
    SharedPtr<BindContext> parent_;

    // Left and right child bind context
    SharedPtr<BindContext> left_child_;
    SharedPtr<BindContext> right_child_;

    Vector<SharedPtr<BindContext>> subquery_children_;

    // CTE from CTE alias -> CTE statement
    std::unordered_map<String, SharedPtr<CommonTableExpressionInfo>> CTE_map_;

    // Binding, all bindings include subquery, cte, view, table ...
    std::unordered_map<String, SharedPtr<Binding>> binding_by_name_;
    std::unordered_map<String, Vector<String>> binding_names_by_column_;

    // Binding Table
    Vector<String> table_names_;

    // Bound expr in group by list
    Vector<String> group_names_;
    std::unordered_map<String, SharedPtr<BaseExpression>> group_by_name_;

    // Bound aggregate function expr
    std::unordered_map<String, SharedPtr<BaseExpression>> aggregate_by_name_;

    // Bound expr in select list
    Vector<String> project_names_;
    std::unordered_map<String, SharedPtr<BaseExpression>> project_by_name_;

    // Bound CTE
    std::unordered_set<SharedPtr<CommonTableExpressionInfo>> bound_cte_set_;

    // Bound View
    std::unordered_set<String> bound_view_set_;

    // Bound Table (base table)
    std::unordered_set<String> bound_table_set_;

    // Bound subquery (TODO: How to get the subquery name?)
    std::unordered_set<String> bound_subquery_set_;

    // An sequence id
    size_t binding_context_id_{0};

    // Output heading of this context
    Vector<String> heading_;

public:
    void
    AddSubQueryChild(const SharedPtr<BindContext>& child);

    void
    AddLeftChild(const SharedPtr<BindContext>& left_child);

    void
    AddRightChild(const SharedPtr<BindContext>& right_child);

    size_t
    GenerateBindingContextIndex();

    [[nodiscard]] SharedPtr<CommonTableExpressionInfo>
    GetCTE(const String& name) const;

    [[nodiscard]] bool
    IsCTEBound(const SharedPtr<CommonTableExpressionInfo>& cte) const;

    void
    BoundCTE(const SharedPtr<CommonTableExpressionInfo>& cte) {
        bound_cte_set_.insert(cte);
    }

    [[nodiscard]] bool
    IsViewBound(const String& view_name) const;

    void
    BoundView(const String& view_name) {
        bound_view_set_.insert(view_name);
    }

    [[nodiscard]] bool
    IsTableBound(const String& table_name) const;

    void
    BoundTable(const String& table_name) {
        bound_table_set_.insert(table_name);
    }

    int64_t GetNewLogicalNodeId();

    void
    AddSubqueryBinding(const String& name, const Vector<DataType>& column_types, const Vector<String>& column_names);

    void
    AddCTEBinding(const String& name, const Vector<DataType>& column_types, const Vector<String>& column_names);

    void
    AddViewBinding(const String& name, const Vector<DataType>& column_types, const Vector<String>& column_names);

    void
    AddTableBinding(const String& name, SharedPtr<Table> table_ptr,
//                    int64_t logical_node_id,
//                    SharedPtr<LogicalNode> logical_node_ptr,
                    const Vector<DataType>& column_types, const Vector<String>& column_names);

    // Merge input bind context into this bind context
    void AddBindContext(const SharedPtr<BindContext>& other_ptr);

    SharedPtr<BaseExpression>
    ResolveColumnId(const ColumnIdentifier& column_identifier, int64_t depth);

private:
    void
    AddBinding(const SharedPtr<Binding>& binding);

private:
    int64_t next_logical_node_id_{1};
    int64_t next_bind_context_index_{1};

public:

    // !!! TODO: Below need to be refactored !!!

public:

    // Binder, different binder have different expression build behavior.
    SharedPtr<ExpressionBinder> expression_binder_{nullptr};
};

}


