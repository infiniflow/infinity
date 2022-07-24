//
// Created by JinHai on 2022/7/24.
//

#include "catalog.h"
#include "schema.h"
#include "common/utility/asserter.h"

namespace infinity {

void
Catalog::create_schema(const SchemaDefinition &schema_definition) {
    const std::string& schema_name = schema_definition.name();
    if(schemas_.find(schema_name) == schemas_.end()) {
        std::shared_ptr<Schema> schema_ptr = std::make_shared<Schema>(schema_name, schema_id_counter_ ++);
        schemas_[schema_name] = schema_ptr;
    } else {
        ResponseError("Schema already exists: " + schema_name);
    }
}

void
Catalog::delete_schema(const std::string &schema_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found, can't be dropped: " + schema_name);
    }
    schemas_.erase(schema_name);
}

std::shared_ptr<TableDefinition>
Catalog::get_table_by_name(const std::string& schema_name, const std::string& table_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found: " + schema_name);
    }
    return schemas_[schema_name]->get_table_by_name(table_name);
}

}