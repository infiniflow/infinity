//
// Created by jinhai on 23-2-22.
//

#pragma once

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
#include "parser/table_reference/base_table_reference.h"

namespace infinity {

class Statement {
public:
    static void Explain(const BaseStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size = 0);

    static void BuildCreate(const CreateStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildInsert(const InsertStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildCopy(const CopyStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildDrop(const DropStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildSelect(const SelectStatement *statement,
                            SharedPtr<Vector<SharedPtr<String>>> &stmt_string,
                            i64 intent_size,
                            SharedPtr<String> alias_ptr = nullptr);

    static void BuildBaseTableRef(const BaseTableReference *table_ref, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildShow(const ShowStatement *statement, SharedPtr<Vector<SharedPtr<String>>> &stmt_string, i64 intent_size);

    static void BuildFlush(const FlushStatement *flush_statement, SharedPtr<Vector<SharedPtr<String>>> &result, i64 intent_size);
};

} // namespace infinity