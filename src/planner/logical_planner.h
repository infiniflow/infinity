//
// Created by jinhai on 23-1-17.
//

#pragma once

#include <utility>

#include "bound_statement.h"

#include "parser/base_statement.h"
#include "parser/statement/alter_statement.h"
#include "parser/statement/copy_statement.h"
#include "parser/statement/create_statement.h"
#include "parser/statement/delete_statement.h"
#include "parser/statement/drop_statement.h"
#include "parser/statement/execute_statement.h"
#include "parser/statement/explain_statement.h"
#include "parser/statement/flush_statement.h"
#include "parser/statement/insert_statement.h"
#include "parser/statement/prepare_statement.h"
#include "parser/statement/select_statement.h"
#include "parser/statement/show_statement.h"
#include "parser/statement/update_statement.h"
#include "planner/bind_context.h"

namespace infinity {

class LogicalPlanner {
public:
    explicit LogicalPlanner(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {
        names_ptr_ = MakeShared<Vector<String>>();
        types_ptr_ = MakeShared<Vector<DataType>>();
    }

    void Build(const BaseStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildSelect(const SelectStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildInsert(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildInsertValue(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildInsertSelect(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Update operator
    void BuildUpdate(const UpdateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Delete operator
    void BuildDelete(const DeleteStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Create operator
    void BuildCreate(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildCreateSchema(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildCreateTable(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildCreateCollection(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildCreateView(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildCreateIndex(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Drop operator
    void BuildDrop(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildDropTable(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildDropCollection(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildDropSchema(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildDropIndex(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildDropView(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Prepare operator
    void BuildPrepare(const PrepareStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Execute operator
    void BuildExecute(const ExecuteStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildCopy(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Export operator
    void BuildExport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Import operator
    void BuildImport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Alter operator
    void BuildAlter(const AlterStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Show operator
    void BuildShow(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildShowColumns(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildShowTables(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildShowViews(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Flush
    void BuildFlush(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildFlushData(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildFlushLog(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    void BuildFlushBuffer(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    // Explain
    void BuildExplain(const ExplainStatement *statement, SharedPtr<BindContext> &bind_context_ptr);

    [[nodiscard]] SharedPtr<LogicalNode> LogicalPlan() const { return logical_plan_; }

private:
    QueryContext *query_context_ptr_{};

    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<DataType>> types_ptr_{};
    SharedPtr<LogicalNode> logical_plan_{};
};

} // namespace infinity