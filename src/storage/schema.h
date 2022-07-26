//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include "table_definition.h"

#include <unordered_map>
#include <memory>

namespace infinity {

class Schema {
public:
    Schema(std::string name, uint64_t id) : name_(std::move(name)), schema_id_(id) {};
    std::shared_ptr<TableDefinition> GetTableByName(const std::string& name);
    void AddTable(const std::shared_ptr<TableDefinition>& table_def);
    void DeleteTable(const std::string& table_name);
    [[nodiscard]] uint64_t schema_id() const { return schema_id_; };

private:
    std::unordered_map<std::string, std::shared_ptr<TableDefinition>> tables_;
    std::string name_;
    uint64_t table_id_counter_{0};
    uint64_t schema_id_{0};
};

}