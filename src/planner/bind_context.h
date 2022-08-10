//
// Created by JinHai on 2022/8/7.
//

#pragma once

#include "SQLParserResult.h"
#include "expression/base_expression.h"
#include "logical_operator.h"

#include <unordered_map>
#include <utility>

namespace infinity {

class LogicalOperator;
class Table;

struct CommonTableExpressionInfo {
    CommonTableExpressionInfo(std::string alias, hsql::SelectStatement* select_stmt)
        : alias_(std::move(alias)), select_statement_(select_stmt) {}

    std::string alias_;
    hsql::SelectStatement* select_statement_;
};

class BindContext {
public:
    std::shared_ptr<BaseExpression> ResolveColumnIdentifier(const ColumnIdentifier& column_identifier);
    void AddTable(const std::shared_ptr<Table>& table_ptr);

    std::unordered_map<std::string, std::shared_ptr<CommonTableExpressionInfo>> CTE_map_;
    std::vector<std::shared_ptr<LogicalOperator>> operators_;
    uint64_t id_{0};

    std::vector<std::string> heading_;

    std::vector<std::shared_ptr<Table>> tables_;
    std::unordered_map<std::string, std::shared_ptr<Table>> tables_by_name_;
};

}


