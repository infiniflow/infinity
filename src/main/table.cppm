// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

export module table;

import stl;
import query_options;
import value;
import parser;
import session;
import query_result;

namespace infinity {

export class Table {
public:
    Table(String table_name, SharedPtr<BaseSession> session) : table_name_(Move(table_name)), session_(Move(session)) {}

    QueryResult CreateIndex(const String &index_name, Vector<IndexInfo *> *index_info_list, CreateIndexOptions create_index_options);

    QueryResult DropIndex(const String &index_name);

    QueryResult Insert(Vector<String> *columns, Vector<Vector<ParsedExpr *> *> *values);

    QueryResult Import(const String &path, ImportOptions import_options);

    QueryResult Delete(ParsedExpr *filter);

    QueryResult Update(ParsedExpr *filter, Vector<UpdateExpr *> *update_list);

    QueryResult Search(Vector<ParsedExpr *> &knn_exprs,
                       Vector<Pair<ParsedExpr *, ParsedExpr *>> &fts_expr,
                       ParsedExpr *filter,
                       Vector<ParsedExpr *> *output_columns,
                       ParsedExpr *offset,
                       ParsedExpr *limit);

private:
    String table_name_{};
    SharedPtr<BaseSession> session_{};
};

} // namespace infinity
