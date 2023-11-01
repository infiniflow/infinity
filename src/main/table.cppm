//
// Created by jinhai on 23-10-30.
//

export module table;

import stl;
import query_result;
import query_options;
import value;
import parser;
import std;
import session;

namespace infinity {

export class Table {
public:
    Table(String table_name, SharedPtr<EmbeddedSession> session)
        : table_name_(std::move(table_name)), session_(std::move(session)) {}

    QueryResult CreateIndex(const String &index_name, Vector<String> *column_names, CreateIndexOptions create_index_options);

    QueryResult DropIndex(const String &index_name);

    QueryResult Insert(Vector<String> *columns, Vector<Vector<ParsedExpr *> *> *values);

    QueryResult Import(const String &path, ImportOptions import_options);

    QueryResult Delete(const String &condition);

    QueryResult Update(const String &condition, Vector<Pair<String, ParsedExpr *>> &set_lists);

    QueryResult Search(Vector<Pair<ParsedExpr *, ParsedExpr *>> &vector_expr,
                       Vector<Pair<ParsedExpr *, ParsedExpr *>> &fts_expr,
                       ParsedExpr *filter,
                       Vector<ParsedExpr *> *output_columns,
                       ParsedExpr *offset,
                       ParsedExpr *limit);

private:
    String table_name_{};
    SharedPtr<EmbeddedSession> session_{};
};

} // namespace infinity
