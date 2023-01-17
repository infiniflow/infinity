//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include "function/function_set.h"
#include "common/singleton.h"
#include "schema_definition.h"
#include "schema.h"

namespace infinity {

class Catalog : public Singleton<Catalog> {
public:
    void
    CreateSchema(const SharedPtr<SchemaDefinition>& schema_definition);

    void
    DeleteSchema(const String& schema_name);

    // Table related methods
    SharedPtr<Table>
    GetTableByName(const String& schema_name, const String& table_name);

    void
    AddTable(const String& schema_name, const SharedPtr<Table>& table_def);

    void
    DeleteTable(const String& schema_name, const String& table_name);

    std::vector<SharedPtr<Table>>
    GetTables(const String& schema_name);

    // View related methods
    SharedPtr<View>
    GetViewByName(const String& schema_name, const String& view_name);

    void
    AddView(const String& schema_name, const SharedPtr<View>& view);

    void
    DeleteView(const String& schema_name, const String& view_name);

    // Function related methods
    SharedPtr<FunctionSet>
    GetFunctionSetByName(String& function_name);

    void
    AddFunctionSet(const SharedPtr<FunctionSet>& function_set);

    void
    DeleteFunctionSet(const String& function_name);

    // Table Function related methods
    SharedPtr<TableFunction>
    GetTableFunctionByName(const String& function_name);

    void
    AddTableFunction(const SharedPtr<TableFunction>& table_function);

    void
    DeleteTableFunction(const String& function_name);

    ~Catalog() override = default;

private:
    HashMap<String, SharedPtr<Schema>> schemas_;
    HashMap<String, SharedPtr<FunctionSet>> function_sets_;
    HashMap<String, SharedPtr<TableFunction>> table_functions_;

    u64 schema_id_counter_{0};
};

}
