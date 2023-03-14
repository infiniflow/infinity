//
// Created by JinHai on 2022/7/24.
//

#include "catalog.h"
#include "schema.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

void
Catalog::CreateSchema(const SharedPtr<SchemaDefinition>& schema_definition) {
    String schema_name = schema_definition->name();
    std::transform(schema_name.begin(), schema_name.end(), schema_name.begin(), [](const auto c) {
        return std::tolower(c);
    });

    if(schemas_.find(schema_name) == schemas_.end()) {
        SharedPtr<Schema> schema_ptr = MakeShared<Schema>(schema_name, schema_id_counter_ ++);
        schemas_[schema_name] = schema_ptr;
    } else {
        CatalogError("Schema already exists: " + schema_name);
    }
}

void
Catalog::DeleteSchema(String schema_name) {
    std::transform(schema_name.begin(), schema_name.end(), schema_name.begin(), [](const auto c) {
        return std::tolower(c);
    });

    if(schema_name == "default") {
        CatalogError("Default schema can't be dropped.");
    }

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, can't be dropped: " + schema_name);
    }
    schemas_.erase(schema_name);
}

SharedPtr<Table>
Catalog::GetTableByName(String schema_name, const String& table_name) {
    std::transform(schema_name.begin(), schema_name.end(), schema_name.begin(), [](const auto c) {
        return std::tolower(c);
    });

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found: " + schema_name);
    }
    return schemas_[schema_name]->GetTableByName(table_name);
}

void
Catalog::AddTable(String schema_name, const SharedPtr<Table> &table_def) {
    std::transform(schema_name.begin(), schema_name.end(), schema_name.begin(), [](const auto c) {
        return std::tolower(c);
    });

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, table can't be created: " + schema_name);
    }
    schemas_[schema_name]->AddTable(table_def);
}

void
Catalog::DeleteTable(String schema_name, const String &table_name) {
    std::transform(schema_name.begin(), schema_name.end(), schema_name.begin(), [](const auto c) {
        return std::tolower(c);
    });

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, table can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteTable(table_name);
}

Vector<SharedPtr<Table>>
Catalog::GetTables(String schema_name) {
    std::transform(schema_name.begin(), schema_name.end(), schema_name.begin(), [](const auto c) {
        return std::tolower(c);
    });

    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("No schema: " + schema_name);
    }

    Vector<SharedPtr<Table>> output;
    for(auto& elem: schemas_[schema_name]->tables()) {
        output.emplace_back(elem.second);
    }

    return output;
}

SharedPtr<View>
Catalog::GetViewByName(const String& schema_name, const String& view_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found: " + schema_name);
    }
    return schemas_[schema_name]->GetViewByName(view_name);
}

void
Catalog::AddView(const String& schema_name, const SharedPtr<View>& view) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, view can't be created: " + schema_name);
    }
    schemas_[schema_name]->AddView(view);
}

void
Catalog::DeleteView(const String& schema_name, const String& view_name) {
    if(schemas_.find(schema_name) == schemas_.end()) {
        CatalogError("Schema not found, view can't be deleted: " + schema_name);
    }
    schemas_[schema_name]->DeleteView(view_name);
}

SharedPtr<FunctionSet>
Catalog::GetFunctionSetByName(String& function_name) {
    // Transfer the function to upper case.
    std::transform(function_name.begin(), function_name.end(), function_name.begin(), [](const auto c) {
        return std::toupper(c);
    });

    if(!function_sets_.contains(function_name)) {
        CatalogError("No function name: " + function_name);
    }
    return function_sets_[function_name];
}

void
Catalog::AddFunctionSet(const SharedPtr<FunctionSet>& function_set) {
    const String& name = function_set->name();
    if(function_sets_.contains(name)) {
        CatalogError("Trying to add duplicated function name into catalog: " + name);
    }
    function_sets_.emplace(name, function_set);
}

void
Catalog::DeleteFunctionSet(const String& function_name) {
    if(!function_sets_.contains(function_name)) {
        CatalogError("Delete not exist function: " + function_name);
    }
    function_sets_.erase(function_name);
}

SharedPtr<TableFunction>
Catalog::GetTableFunctionByName(const String& function_name) {
    if(!table_functions_.contains(function_name)) {
        CatalogError("No table function name: " + function_name);
    }
    return table_functions_[function_name];
}

void
Catalog::AddTableFunction(const SharedPtr<TableFunction>& table_function) {
    const String& name = table_function->name();
    if(table_functions_.contains(name)) {
        CatalogError("Trying to add duplicated table function name into catalog: " + name);
    }
    table_functions_.emplace(name, table_function);
}

void
Catalog::DeleteTableFunction(const String& function_name) {
    if(!table_functions_.contains(function_name)) {
        CatalogError("Delete not exist table function: " + function_name);
    }
    table_functions_.erase(function_name);
}

}