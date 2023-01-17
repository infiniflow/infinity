//
// Created by jinhai on 23-1-17.
//

#pragma once

#include <utility>

#include "bound_statement.h"
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

#include "planner/bind_context.h"

namespace infinity {

class LogicalPlanner {
public:
    explicit
    LogicalPlanner(SharedPtr<QueryContext> query_context_ptr) : query_context_ptr_(std::move(query_context_ptr)) {
        names_ptr_ = MakeShared<Vector<String>>();
        types_ptr_ = MakeShared<Vector<DataType>>();
    }

    void
    Build(const hsql::SQLStatement &statement);

    void
    BuildSelect(const hsql::SelectStatement &statement);

    // Import operator
    void
    BuildImport(const hsql::ImportStatement &statement);

    void
    BuildImportCsv(const hsql::ImportStatement &statement);

    void
    BuildImportTbl(const hsql::ImportStatement &statement);

    void
    BuildImportBinary(const hsql::ImportStatement &statement);

    void
    BuildImportAuto(const hsql::ImportStatement &statement);

    void
    BuildInsert(const hsql::InsertStatement &statement);

    void
    BuildInsertValue(const hsql::InsertStatement &statement);

    void
    BuildInsertSelect(const hsql::InsertStatement &statement);

    // Update operator
    void
    BuildUpdate(const hsql::UpdateStatement &statement);

    // Delete operator
    void
    BuildDelete(const hsql::DeleteStatement &statement);

    // Create operator
    void
    BuildCreate(const hsql::CreateStatement &statement);

    void
    BuildCreateTable(const hsql::CreateStatement &statement);

    void
    BuildCreateView(const hsql::CreateStatement &statement);

    void
    BuildCreateTableFromTable(const hsql::CreateStatement &statement);

    void
    BuildCreateIndex(const hsql::CreateStatement &statement);

    // Drop operator
    void
    BuildDrop(const hsql::DropStatement &statement);

    void
    BuildDropTable(const hsql::DropStatement &statement);

    void
    BuildDropSchema(const hsql::DropStatement &statement);

    void
    BuildDropIndex(const hsql::DropStatement &statement);

    void
    BuildDropView(const hsql::DropStatement &statement);

    void
    BuildDropPreparedStatement(const hsql::DropStatement &statement);

    // Prepare operator
    void
    BuildPrepare(const hsql::PrepareStatement &statement);

    // Execute operator
    void
    BuildExecute(const hsql::ExecuteStatement &statement);

    // Export operator
    void
    BuildExport(const hsql::ExportStatement &statement);

    void
    BuildExportCsv(const hsql::ExportStatement &statement);

    void
    BuildExportTbl(const hsql::ExportStatement &statement);

    void
    BuildExportBinary(const hsql::ExportStatement &statement);

    void
    BuildExportAuto(const hsql::ExportStatement &statement);

    // Alter operator
    void
    BuildAlter(const hsql::AlterStatement& statement);

    void
    BuildAlterDropColumn(const hsql::AlterStatement& statement);

    // Show operator
    void
    BuildShow(const hsql::ShowStatement& statement);

    void
    BuildShowColumns(const hsql::ShowStatement& statement);

    void
    BuildShowTables(const hsql::ShowStatement& statement);

    // Transaction operator
    void
    BuildTransaction(const hsql::TransactionStatement &statement);

    void
    BuildTransactionBegin(const hsql::TransactionStatement &statement);

    void
    BuildTransactionCommit(const hsql::TransactionStatement &statement);

    void
    BuildTransactionRollback(const hsql::TransactionStatement &statement);

    [[nodiscard]] SharedPtr<LogicalNode>
    LogicalPlan() const {
        return logical_plan_;
    }

private:
    SharedPtr<QueryContext> query_context_ptr_{};

    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<DataType>> types_ptr_{};
    SharedPtr<LogicalNode> logical_plan_{};
};

}