//
// Created by JinHai on 2022/9/23.
//

#pragma once

#include "sql/Expr.h"

#include <string>

namespace infinity {

struct ColumnIdentifier {
public:
    static ColumnIdentifier MakeColumnIdentifier(const hsql::Expr &expr);

    explicit ColumnIdentifier(std::shared_ptr<std::string> table_name, std::shared_ptr<std::string> column_name,
                              std::shared_ptr<std::string> alias_name);

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] bool operator==(const ColumnIdentifier& other) const;

    std::shared_ptr<std::string> column_name_ptr_;
    std::shared_ptr<std::string> table_name_ptr_;
    std::shared_ptr<std::string> alias_name_ptr_;
};

}

