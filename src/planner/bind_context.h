//
// Created by JinHai on 2022/8/7.
//

#pragma once

#include "SQLParserResult.h"
#include "expression/base_expression.h"
#include "logical_node.h"


#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace infinity {

class LogicalNode;
class Table;
class ExpressionBinder;
class Binding;

struct CommonTableExpressionInfo {
    CommonTableExpressionInfo(std::string alias, hsql::SelectStatement* select_stmt, std::unordered_set<std::string> masked_name_set)
        : alias_(std::move(alias)), select_statement_(select_stmt), masked_name_set_(std::move(masked_name_set)) {}

    std::string alias_;
    hsql::SelectStatement* select_statement_;
    std::unordered_set<std::string> masked_name_set_;
};

class BindContext {
public:
    std::shared_ptr<BaseExpression> ResolveColumnIdentifier(const ColumnIdentifier& column_identifier);
    void AddTable(const std::shared_ptr<Table>& table_ptr);

public:
    // Parent bind context
    std::shared_ptr<BindContext> parent_;

    // Left and right child bind context
    std::weak_ptr<BindContext> left_;
    std::weak_ptr<BindContext> right_;

    // CTE from CTE alias -> CTE statement
    std::unordered_map<std::string, std::shared_ptr<CommonTableExpressionInfo>> CTE_map_;

    // Binding, all bindings include subquery, cte, view, table ...
    std::vector<std::shared_ptr<Binding>> bindings_;
    std::unordered_map<std::string, std::shared_ptr<Binding>> bindings_by_name_;

    // Bound CTE binding
    std::unordered_set<std::string> bound_cte_set_;

    // !!! TODO: Below need to be refactored !!!

    // All logical operator
    std::vector<std::shared_ptr<LogicalNode>> operators_;

    // An sequence id
    uint64_t id_{0};

    // Output heading of this context
    std::vector<std::string> heading_;

    // Binding Table
    std::vector<std::shared_ptr<Table>> tables_;
    std::unordered_map<std::string, std::shared_ptr<Table>> tables_by_name_;

    // Bind group by expr
    std::vector<std::shared_ptr<BaseExpression>> groups_;
    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> groups_by_expr_;

    // Bind aggregate function by expr
    std::vector<std::shared_ptr<BaseExpression>> aggregates_;
    std::unordered_map<std::string, std::shared_ptr<BaseExpression>> aggregates_by_expr_;

    // Binder, different binder have different expression build behavior.
    std::shared_ptr<ExpressionBinder> binder_{nullptr};
};

}


