//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "common/types/internal_types.h"
#include "parser/statement/select_statement.h"

// A view means a logical plan
namespace infinity {

class LogicalNode;

class View {
public:
    View(String view_name,
         String sql_text,
         SelectStatement* select_statement_,
         Vector<String> column_names);

    [[nodiscard]] const String& name() const { return name_; }
    [[nodiscard]] const String& sql_text() const { return sql_text_; }
    const SelectStatement* GetSQLStatement() { return select_statement_; }
    [[nodiscard]] const Vector<String>& column_names() const { return column_names_; }

private:
    String name_;
    String sql_text_;
    SharedPtr<LogicalNode> logical_plan_;
    SelectStatement* select_statement_;
    Vector<String> column_names_;
};
}

