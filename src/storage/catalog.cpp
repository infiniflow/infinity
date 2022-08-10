//
// Created by JinHai on 2022/7/24.
//

#include "catalog.h"
#include "schema.h"
#include "common/utility/asserter.h"

namespace infinity {

void
Catalog::CreateSchema(const std::shared_ptr<SchemaDefinition>& schema_definition) {
    const std::string& schema_name = schema_definition->name();
    if(schemas_.find(schema_name) == schemas_.end()) {
        std::shared_ptr<Schema> schema_ptr = std::make_shared<Schema>(schema_name, schema_id_counter_ ++);
        schemas_[schema_name] = schema_ptr;
    } else {
        ResponseError("Schema already exists: " + schema_name);
    }
}

void
Catalog::DeleteSchema(const std::string &schema_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found, can't be dropped: " + schema_name);
    }
    schemas_.erase(schema_name);
}

std::shared_ptr<Table>
Catalog::GetTableByName(const std::string& schema_name, const std::string& table_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found: " + schema_name);
    }
    return schemas_[schema_name]->GetTableByName(table_name);
}

void
Catalog::AddTable(const std::string &schema_name, const std::shared_ptr<Table> &table_def) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found, table can't be created: " + schema_name);
    }
    schemas_[schema_name]->AddTable(table_def);
}

void
Catalog::DeleteTable(const std::string &schema_name, const std::string &table_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found, table can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteTable(table_name);
}

std::shared_ptr<View>
Catalog::GetViewByName(const std::string& schema_name, const std::string& view_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found: " + schema_name);
    }
    return schemas_[schema_name]->GetViewByName(view_name);
}

void
Catalog::AddView(const std::string& schema_name, const std::shared_ptr<View>& view) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found, view can't be created: " + schema_name);
    }
    schemas_[schema_name]->AddView(view);
}

void
Catalog::DeleteView(const std::string& schema_name, const std::string& view_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        ResponseError("Schema not found, view can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteView(view_name);
}

}