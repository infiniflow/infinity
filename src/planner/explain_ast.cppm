//
// Created by jinhai on 23-2-22.
//

module;

import parser;
import stl;

export module explain_ast;

namespace infinity {

export class ExplainAST {
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