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
    void create_schema(const SchemaDefinition& schema_definition);
    void delete_schema(const std::string& schema_name);
    std::shared_ptr<TableDefinition> get_table_by_name(const std::string& schema_name, const std::string& table_name);

    ~Catalog() override = default;

private:
    std::unordered_map<std::string, std::shared_ptr<Schema>> schemas_;

    uint64_t schema_id_counter_{0};
};

}
