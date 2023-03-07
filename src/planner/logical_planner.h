//
// Created by jinhai on 23-1-17.
//

#pragma once

#include <utility>

#include "bound_statement.h"

#include "planner/bind_context.h"
#include "parser/base_statement.h"
#include "parser/statement/select_statement.h"
#include "parser/statement/insert_statement.h"
#include "parser/statement/update_statement.h"
#include "parser/statement/delete_statement.h"
#include "parser/statement/create_statement.h"
#include "parser/statement/drop_statement.h"
#include "parser/statement/prepare_statement.h"
#include "parser/statement/execute_statement.h"
#include "parser/statement/alter_statement.h"
#include "parser/statement/show_statement.h"
#include "parser/statement/explain_statement.h"
#include "parser/statement/copy_statement.h"

namespace infinity {

class LogicalPlanner {
public:
    explicit
    LogicalPlanner(SharedPtr<QueryContext> query_context_ptr) : query_context_ptr_(std::move(query_context_ptr)) {
        names_ptr_ = MakeShared<Vector<String>>();
        types_ptr_ = MakeShared<Vector<DataType>>();
    }

    void
    Build(const BaseStatement* statement);

    void
    BuildSelect(const SelectStatement* statement);

    void
    BuildInsert(const InsertStatement* statement);

    void
    BuildInsertValue(const InsertStatement* statement);

    void
    BuildInsertSelect(const InsertStatement* statement);

    // Update operator
    void
    BuildUpdate(const UpdateStatement* statement);

    // Delete operator
    void
    BuildDelete(const DeleteStatement* statement);

    // Create operator
    void
    BuildCreate(const CreateStatement* statement);

    void
    BuildCreateSchema(const CreateStatement* statement);

    void
    BuildCreateTable(const CreateStatement* statement);

    void
    BuildCreateCollection(const CreateStatement* statement);

    void
    BuildCreateView(const CreateStatement* statement);

    void
    BuildCreateIndex(const CreateStatement* statement);

    // Drop operator
    void
    BuildDrop(const DropStatement* statement);

    void
    BuildDropTable(const DropStatement* statement);

    void
    BuildDropCollection(const DropStatement* statement);

    void
    BuildDropSchema(const DropStatement* statement);

    void
    BuildDropIndex(const DropStatement* statement);

    void
    BuildDropView(const DropStatement* statement);

    // Prepare operator
    void
    BuildPrepare(const PrepareStatement* statement);

    // Execute operator
    void
    BuildExecute(const ExecuteStatement* statement);


    void
    BuildCopy(const CopyStatement* statement);
    // Export operator
    void
    BuildExport(const CopyStatement* statement);

    void
    BuildExportCsv(const CopyStatement* statement);

    // Import operator
    void
    BuildImport(const CopyStatement* statement);

    void
    BuildImportCsv(const CopyStatement* statement);

    // Alter operator
    void
    BuildAlter(const AlterStatement* statement);

    // Show operator
    void
    BuildShow(const ShowStatement* statement);

    void
    BuildShowColumns(const ShowStatement* statement);

    void
    BuildShowTables(const ShowStatement* statement);

    // Transaction operator
    void
    BuildExplain(const ExplainStatement* statement);

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