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
    DeleteSchema(String schema_name);

    // Table related methods
    SharedPtr<BaseTable>
    GetTableByName(String schema_name, String table_name);

    SharedPtr<BaseTable>
    GetTableByNameNoExcept(String schema_name, String table_name) noexcept;

    void
    CreateTable(String schema_name, const SharedPtr<Table>& table_def);

    void
    DeleteTable(String schema_name, String table_name);

    void
    CreateCollection(String schema_name, String collection_name);

    void
    DeleteCollection(String schema_name, String collection_name);

    Vector<SharedPtr<BaseTable>>
    GetTables(String schema_name);

    // View related methods
    SharedPtr<View>
    GetViewByName(String schema_name, String view_name);

    SharedPtr<View>
    GetViewByNameNoExcept(String schema_name, String view_name) noexcept;

    void
    CreateView(String schema_name, const SharedPtr<View>& view);

    void
    DeleteView(String schema_name, String view_name);

    Vector<SharedPtr<View>>
    GetViews(String schema_name);

    // Function related methods
    SharedPtr<FunctionSet>
    GetFunctionSetByName(String function_name);

    void
    AddFunctionSet(const SharedPtr<FunctionSet>& function_set);

    void
    DeleteFunctionSet(String function_name);

    // Table Function related methods
    SharedPtr<TableFunction>
    GetTableFunctionByName(String function_name);

    void
    AddTableFunction(const SharedPtr<TableFunction>& table_function);

    void
    DeleteTableFunction(String function_name);

    ~Catalog() override = default;

private:
    HashMap<String, SharedPtr<Schema>> schemas_;
    HashMap<String, SharedPtr<FunctionSet>> function_sets_;
    HashMap<String, SharedPtr<TableFunction>> table_functions_;

    u64 schema_id_counter_{0};
};

}
