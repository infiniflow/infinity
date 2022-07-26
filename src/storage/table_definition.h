//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include "column_definition.h"

#include <vector>

namespace infinity {

class TableDefinition {
public:
    explicit TableDefinition(std::string table_name, std::vector<ColumnDefinition> columns, bool if_not_exists)
        : columns_(std::move(columns)), name_(std::move(table_name)), if_not_exists_(if_not_exists) {}

    void set_table_id(uint64_t table_id) { table_id_ = table_id; }
    [[nodiscard]] const std::vector<ColumnDefinition>& columns() const { return columns_; }
    [[nodiscard]] uint64_t column_count() const { return columns_.size(); }
    [[nodiscard]] const std::string& name() const { return name_; }
private:
    std::vector<ColumnDefinition> columns_;
    std::string name_;
    bool if_not_exists_{false};
    uint64_t table_id_{0};
};



}


