//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "SQLParserResult.h"

#include <vector>
#include <string>
#include <memory>

// A view means a logical plan
namespace infinity {

class LogicalNode;

class View {
public:
    View(std::string view_name,
         std::string sql_text,
         hsql::SQLParserResult sql_parser_result,
         std::vector<std::string> column_names);

    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] const std::string& sql_text() const { return sql_text_; }
    const hsql::SQLStatement* GetSQLStatement() { return sql_parser_result_.getStatement(0); }
    [[nodiscard]] const std::vector<std::string>& column_names() const { return column_names_; }
    void set_view_id(uint64_t view_id) { view_id_ = view_id; }

private:
    std::string name_;
    std::string sql_text_;
    std::shared_ptr<LogicalNode> logical_plan_;
    hsql::SQLParserResult sql_parser_result_;
    std::vector<std::string> column_names_;
    uint64_t view_id_{0};
};
}

