//
// Created by JinHai on 2022/8/7.
//

#pragma once

#include "SQLParserResult.h"

#include <unordered_map>
#include <utility>

namespace infinity {

class LogicalOperator;

struct CommonTableExpressionInfo {
    CommonTableExpressionInfo(std::string alias, hsql::SelectStatement* select_stmt)
        : alias_(std::move(alias)), select_statement_(select_stmt) {}

    std::string alias_;
    hsql::SelectStatement* select_statement_;
};

class BindContext {
public:
    std::unordered_map<std::string, std::shared_ptr<CommonTableExpressionInfo>> CTE_map_;
    std::vector<std::shared_ptr<LogicalOperator>> operators_;
    uint64_t id_{0};
};

}


