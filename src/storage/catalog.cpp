//
// Created by JinHai on 2022/7/24.
//
#if 0
#include "catalog.h"

#include <utility>
#include "schema.h"
#include "common/utility/infinity_assert.h"
#include "collection.h"

namespace infinity {

void
Catalog::CreateSchema(const SharedPtr<SchemaDefinition>& schema_definition) {
    String schema_name = schema_definition->name();
    StringToLower(schema_name);

    if(schemas_.find(schema_name) == schemas_.end()) {
        SharedPtr<Schema> schema_ptr = MakeShared<Schema>(schema_name, schema_id_counter_ ++);
        schemas_[schema_name] = schema_ptr;
    } else {
        CatalogError("Schema already exists: " + schema_name);
    }
}

void
Catalog::DeleteSchema(String schema_name, ConflictType conflict_type) {
    StringToLower(schema_name);

    if(schema_name == "default") {
        CatalogError("Default schema can't be dropped.");
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, can't be dropped: " + schema_name);
    }
    schemas_.erase(schema_name);
}

SharedPtr<BaseTable>
Catalog::GetTableByName(String schema_name, String table_name) {
    SharedPtr<BaseTable> result = GetTableByNameNoExcept(std::move(schema_name), std::move(table_name));
    if(result == nullptr) {
        CatalogError(fmt::format("Table isn't found: {}.{}", schema_name, table_name));
    } else {
        return result;
    }
}

SharedPtr<BaseTable>
Catalog::GetTableByNameNoExcept(String schema_name, String table_name) noexcept {
    StringToLower(schema_name);
    StringToLower(table_name);

    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        return nullptr;
    }
    return schemas_[schema_name]->GetTableByName(table_name);
}

void
Catalog::CreateTable(String schema_name, const SharedPtr<Table> &table_def, ConflictType conflict_type) {
    StringToLower(schema_name);

    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, table can't be created: " + schema_name);
    }
    schemas_[schema_name]->AddTable(table_def, conflict_type);
}

void
Catalog::DeleteTable(String schema_name, String table_name, ConflictType conflict_type) {
    StringToLower(schema_name);
    StringToLower(table_name);

    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, table can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteTable(table_name, conflict_type);
}

void
Catalog::CreateCollection(String schema_name, String collection_name, ConflictType conflict_type) {
    StringToLower(schema_name);

    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, table can't be created: " + schema_name);
    }

    String copied_schema_name = schema_name;
    SharedPtr<Collection> collection_ptr = MakeShared<Collection>(MakeShared<String>(schema_name),
                                                                  MakeShared<String>(collection_name));
    schemas_[copied_schema_name]->AddTable(collection_ptr, conflict_type);
}

void
Catalog::DeleteCollection(String schema_name, String collection_name, ConflictType conflict_type) {
    StringToLower(schema_name);
    StringToLower(collection_name);

    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, collection can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteTable(collection_name, conflict_type);
}

Vector<SharedPtr<BaseTable>>
Catalog::GetTables(String schema_name) {
    StringToLower(schema_name);

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("No schema: " + schema_name);
    }

    Vector<SharedPtr<BaseTable>> output;
    for(auto& elem: schemas_[schema_name]->tables()) {
        output.emplace_back(elem.second);
    }

    return output;
}

SharedPtr<View>
Catalog::GetViewByName(String schema_name, String view_name) {
    SharedPtr<View> result = GetViewByNameNoExcept(std::move(schema_name), std::move(view_name));
    if(result == nullptr) {
        CatalogError(fmt::format("View isn't found: {}.{}", schema_name, view_name));
    }
    return result;
}

SharedPtr<View>
Catalog::GetViewByNameNoExcept(String schema_name, String view_name) noexcept {
    StringToLower(schema_name);
    StringToLower(view_name);

    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        return nullptr;
    }
    return schemas_[schema_name]->GetViewByName(view_name);
}

void
Catalog::CreateView(String schema_name, const SharedPtr<View>& view, ConflictType conflict_type) {
    StringToLower(schema_name);
    if(schema_name.empty()) {
        schema_name = "default";
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, view can't be created: " + schema_name);
    }
    schemas_[schema_name]->AddView(view, conflict_type);
}

void
Catalog::DeleteView(String schema_name, String view_name, ConflictType conflict_type) {
    StringToLower(schema_name);
    StringToLower(view_name);
    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, view can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteView(view_name, conflict_type);
}

Vector<SharedPtr<View>>
Catalog::GetViews(String schema_name) {
    StringToLower(schema_name);

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("No schema: " + schema_name);
    }

    Vector<SharedPtr<View>> output;
    for(auto& elem: schemas_[schema_name]->views()) {
        output.emplace_back(elem.second);
    }

    return output;
}

SharedPtr<FunctionSet>
Catalog::GetFunctionSetByName(String function_name) {
    // Transfer the function to upper case.
    StringToLower(function_name);

    if(!function_sets_.contains(function_name)) {
        CatalogError("No function name: " + function_name);
    }
    return function_sets_[function_name];
}

void
Catalog::AddFunctionSet(const SharedPtr<FunctionSet>& function_set) {
    String name = function_set->name();
    StringToLower(name);
    if(function_sets_.contains(name)) {
        CatalogError("Trying to add duplicated function table_name into catalog: " + name);
    }
    function_sets_.emplace(name, function_set);
}

void
Catalog::DeleteFunctionSet(String function_name) {
    StringToLower(function_name);
    if(!function_sets_.contains(function_name)) {
        CatalogError("Delete not exist function: " + function_name);
    }
    function_sets_.erase(function_name);
}

SharedPtr<TableFunction>
Catalog::GetTableFunctionByName(String function_name) {
    StringToLower(function_name);
    if(!table_functions_.contains(function_name)) {
        CatalogError("No table function table_name: " + function_name);
    }
    return table_functions_[function_name];
}

void
Catalog::AddTableFunction(const SharedPtr<TableFunction>& table_function) {
    String name = table_function->name();
    StringToLower(name);
    if(table_functions_.contains(name)) {
        CatalogError("Trying to add duplicated table function table_name into catalog: " + name);
    }
    table_functions_.emplace(name, table_function);
}

void
Catalog::DeleteTableFunction(String function_name) {
    StringToLower(function_name);
    if(!table_functions_.contains(function_name)) {
        CatalogError("Delete not exist table function: " + function_name);
    }
    table_functions_.erase(function_name);
}

}
#endif