//
// Created by JinHai on 2022/8/7.
//

#pragma once

#include "SQLParserResult.h"
#include "expression/base_expression.h"
#include "logical_node.h"
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
    CommonTableExpressionInfo(std::string alias, hsql::SelectStatement* select_stmt, std::unordered_set<std::string> masked_name_set)
        : alias_(std::move(alias)), select_statement_(select_stmt), masked_name_set_(std::move(masked_name_set)) {}

    std::string alias_;
    hsql::SelectStatement* select_statement_;
    std::unordered_set<std::string> masked_name_set_;
};

class BindContext {
public:
    explicit BindContext(std::shared_ptr<BindContext> parent)
        : parent_(std::move(parent)) {}

    virtual ~BindContext();

    void Destroy();

    // Parent bind context
    std::shared_ptr<BindContext> parent_;

    // Left and right child bind context
    std::vector<std::shared_ptr<BindContext>> children_;

    // CTE from CTE alias -> CTE statement
    std::unordered_map<std::string, std::shared_ptr<CommonTableExpressionInfo>> CTE_map_;

    // Binding, all bindings include subquery, cte, view, table ...
    std::unordered_map<std::string, std::shared_ptr<Binding>> binding_by_name_;
    std::unordered_map<std::string, std::vector<std::string>> binding_names_by_column_;

    // Binding Table
    std::vector<std::string> table_names_;

    // Bound expr in group by list
    std::vector<std::string> group_names_;
    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> group_by_name_;

    // Bound aggregate function expr
    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> aggregate_by_name_;

    // Bound expr in select list
    std::vector<std::string> project_names_;
    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> project_by_name_;

    // Bound CTE
    std::unordered_set<std::shared_ptr<CommonTableExpressionInfo>> bound_cte_set_;

    // Bound View
    std::unordered_set<std::string> bound_view_set_;

    // Bound Table (base table)
    std::unordered_set<std::string> bound_table_set_;

    // Bound subquery (TODO: How to get the subquery name?)
    std::unordered_set<std::string> bound_subquery_set_;

    // An sequence id
    size_t binding_context_id_{0};

    // Output heading of this context
    std::vector<std::string> heading_;

public:
    void AddChild(const std::shared_ptr<BindContext>& child);
    size_t GenerateBindingContextIndex();
    [[nodiscard]] std::shared_ptr<CommonTableExpressionInfo> GetCTE(const std::string& name) const;
    [[nodiscard]] bool IsCTEBound(const std::shared_ptr<CommonTableExpressionInfo>& cte) const;
    void BoundCTE(const std::shared_ptr<CommonTableExpressionInfo>& cte) { bound_cte_set_.insert(cte); }
    [[nodiscard]] bool IsViewBound(const std::string& view_name) const;
    void BoundView(const std::string& view_name) { bound_view_set_.insert(view_name); }
    [[nodiscard]] bool IsTableBound(const std::string& table_name) const;
    void BoundTable(const std::string& table_name) { bound_table_set_.insert(table_name); }

    int64_t GetNewLogicalNodeId();

    void
    AddSubqueryBinding(const std::string& name, const std::vector<LogicalType>& column_types, const std::vector<std::string>& column_names);

    void
    AddCTEBinding(const std::string& name, const std::vector<LogicalType>& column_types, const std::vector<std::string>& column_names);

    void
    AddViewBinding(const std::string& name, const std::vector<LogicalType>& column_types, const std::vector<std::string>& column_names);

    void
    AddTableBinding(const std::string& name, std::shared_ptr<Table> table_ptr, int64_t logical_node_id,
                         std::shared_ptr<LogicalNode> logical_node_ptr,
                         const std::vector<LogicalType>& column_types, const std::vector<std::string>& column_names);

    // Merge input bind context into this bind context
    void AddBindContext(const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    ResolveColumnId(const ColumnIdentifier& column_identifier, int64_t depth);

private:
    void
    AddBinding(const std::shared_ptr<Binding>& binding);

private:
    int64_t next_logical_node_id_{1};
    int64_t next_bind_context_index_{1};

public:

    // !!! TODO: Below need to be refactored !!!

public:

    // Binder, different binder have different expression build behavior.
    std::shared_ptr<ExpressionBinder> expression_binder_{nullptr};
};

}


