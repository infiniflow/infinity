//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include "common/singleton.h"
#include "schema_definition.h"
#include "schema.h"

namespace infinity {

class Catalog : public Singleton<Catalog> {
public:
    void CreateSchema(const std::shared_ptr<SchemaDefinition>& schema_definition);
    void DeleteSchema(const std::string& schema_name);

    // Table related functions
    std::shared_ptr<Table> GetTableByName(const std::string& schema_name, const std::string& table_name);
    void AddTable(const std::string& schema_name, const std::shared_ptr<Table>& table_def);
    void DeleteTable(const std::string& schema_name, const std::string& table_name);

    // View related functions
    std::shared_ptr<View> GetViewByName(const std::string& schema_name, const std::string& view_name);
    void AddView(const std::string& schema_name, const std::shared_ptr<View>& view);
    void DeleteView(const std::string& schema_name, const std::string& view_name);

    ~Catalog() override = default;

private:
    std::unordered_map<std::string, std::shared_ptr<Schema>> schemas_;

    uint64_t schema_id_counter_{0};
};

}
