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
    CreateSchema(const std::shared_ptr<SchemaDefinition>& schema_definition);

    void
    DeleteSchema(const std::string& schema_name);

    // Table related methods
    std::shared_ptr<Table>
    GetTableByName(const std::string& schema_name, const std::string& table_name);

    void
    AddTable(const std::string& schema_name, const std::shared_ptr<Table>& table_def);

    void
    DeleteTable(const std::string& schema_name, const std::string& table_name);

    std::vector<std::shared_ptr<Table>>
    GetTables(const std::string& schema_name);

    // View related methods
    std::shared_ptr<View>
    GetViewByName(const std::string& schema_name, const std::string& view_name);

    void
    AddView(const std::string& schema_name, const std::shared_ptr<View>& view);

    void
    DeleteView(const std::string& schema_name, const std::string& view_name);

    // Function related methods
    std::shared_ptr<FunctionSet>
    GetFunctionSetByName(const std::string& function_name);

    void
    AddFunctionSet(const std::shared_ptr<FunctionSet>& function_set);

    void
    DeleteFunctionSet(const std::string& function_name);

    // Table Function related methods
    std::shared_ptr<TableFunction>
    GetTableFunctionByName(const std::string& function_name);

    void
    AddTableFunction(const std::shared_ptr<TableFunction>& table_function);

    void
    DeleteTableFunction(const std::string& function_name);

    ~Catalog() override = default;

private:
    std::unordered_map<std::string, std::shared_ptr<Schema>> schemas_;
    std::unordered_map<std::string, std::shared_ptr<FunctionSet>> function_sets_;
    std::unordered_map<std::string, std::shared_ptr<TableFunction>> table_functions_;

    uint64_t schema_id_counter_{0};
};

}
