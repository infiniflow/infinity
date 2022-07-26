//
// Created by JinHai on 2022/7/23.
//

#pragma once


#include "logical_operator.h"
#include "SQLParserResult.h"
#include "sql/CreateStatement.h"
#include "sql/DropStatement.h"
#include "sql/InsertStatement.h"
#include "sql/DeleteStatement.h"
#include "sql/UpdateStatement.h"
#include "sql/SelectStatement.h"
#include "sql/ShowStatement.h"
#include "sql/ImportStatement.h"
#include "sql/ExportStatement.h"
#include "sql/TransactionStatement.h"
#include "sql/AlterStatement.h"
#include "sql/PrepareStatement.h"
#include "sql/ExecuteStatement.h"

#include "storage/data_type.h"

namespace infinity {

class Planner {
public:
    void CreateLogicalPlan(hsql::SQLStatement *statement);
    [[nodiscard]] std::shared_ptr<LogicalOperator> root_operator() const { return root_operator_; }
    [[nodiscard]] std::string ToString() const;

    static LogicalType TypeConversion(hsql::ColumnType type);
private:
    std::shared_ptr<LogicalOperator> CreateLogicalOperator(const hsql::SQLStatement &statement);

    // Create operator
    std::shared_ptr<LogicalOperator> BuildCreate(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateTable(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateTableFromTable(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateView(const hsql::CreateStatement& statement);
    std::shared_ptr<LogicalOperator> BuildCreateIndex(const hsql::CreateStatement& statement);

    // Drop operator
    std::shared_ptr<LogicalOperator> BuildDrop(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropTable(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> BuildDropSchema(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> build_drop_index(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> build_drop_view(const hsql::DropStatement& statement);
    std::shared_ptr<LogicalOperator> build_drop_prepared_statement(const hsql::DropStatement& statement);

    // Insert operator
    std::shared_ptr<LogicalOperator> build_insert(const hsql::InsertStatement& statement);
    std::shared_ptr<LogicalOperator> build_insert_value(const hsql::InsertStatement& statement);
    std::shared_ptr<LogicalOperator> build_insert_select(const hsql::InsertStatement& statement);

    // Delete operator
    std::shared_ptr<LogicalOperator> build_delete(const hsql::DeleteStatement& statement);

    // Update operator
    std::shared_ptr<LogicalOperator> build_update(const hsql::UpdateStatement& statement);

    // Select operator
    std::shared_ptr<LogicalOperator> BuildSelect(const hsql::SelectStatement& statement);

    // Show operator
    std::shared_ptr<LogicalOperator> build_show(const hsql::ShowStatement& statement);
    std::shared_ptr<LogicalOperator> build_show_columns(const hsql::ShowStatement& statement);
    std::shared_ptr<LogicalOperator> build_show_tables(const hsql::ShowStatement& statement);

    // Import operator
    std::shared_ptr<LogicalOperator> build_import(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> build_import_csv(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> build_import_tbl(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> build_import_binary(const hsql::ImportStatement& statement);
    std::shared_ptr<LogicalOperator> build_import_auto(const hsql::ImportStatement& statement);

    // Export operator
    std::shared_ptr<LogicalOperator> build_export(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> build_export_csv(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> build_export_tbl(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> build_export_binary(const hsql::ExportStatement& statement);
    std::shared_ptr<LogicalOperator> build_export_auto(const hsql::ExportStatement& statement);

    // Transaction operator
    std::shared_ptr<LogicalOperator> build_transaction(const hsql::TransactionStatement& statement);
    std::shared_ptr<LogicalOperator> build_transaction_begin(const hsql::TransactionStatement& statement);
    std::shared_ptr<LogicalOperator> build_transaction_commit(const hsql::TransactionStatement& statement);
    std::shared_ptr<LogicalOperator> build_transaction_rollback(const hsql::TransactionStatement& statement);

    // Alter operator
    std::shared_ptr<LogicalOperator> build_alter(const hsql::AlterStatement& statement);
    std::shared_ptr<LogicalOperator> build_alter_drop_column(const hsql::AlterStatement& statement);

    // Prepare operator
    std::shared_ptr<LogicalOperator> build_prepare(const hsql::PrepareStatement& statement);

    // Execute operator
    std::shared_ptr<LogicalOperator> build_execute(const hsql::ExecuteStatement& statement);

    std::shared_ptr<LogicalOperator> root_operator_{nullptr};
};

}
