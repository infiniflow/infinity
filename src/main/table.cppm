//
// Created by jinhai on 23-10-30.
//

export module table;

import stl;
import query_result;
import query_options;
import value;
import parser;

namespace infinity {

export class Table {
public:
    QueryResult CreateIndex(const String &index_name, const String &column_name, CreateIndexOptions create_index_options);

    QueryResult DropIndex(const String &index_name);

    QueryResult Insert(const Vector<Value> &value_lists);

    QueryResult Import(const String &path, ImportOptions import_options);

    QueryResult Delete(const String &condition);

    QueryResult Update(const String &condition, Vector<Pair<String, Value>> &set_lists);

    QueryResult Search(const Vector<Pair<ParsedExpr*, ParsedExpr*>> &vector_expr,
                       const Vector<Pair<ParsedExpr*, ParsedExpr*>> &fts_expr,
                       const ParsedExpr *filter,
                       const Vector<ParsedExpr*> &output_columns,
                       ParsedExpr* offset,
                       ParsedExpr* limit);
};

} // namespace infinity
