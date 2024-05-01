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
module;

export module infinity;

import stl;
import config;
import resource_manager;
import task_scheduler;
import storage;
import status;
import query_result;
import query_options;
import infinity_context;
import session;
import parsed_expr;
import search_expr;
import column_def;
import create_index_info;
import update_statement;
import explain_statement;
import command_statement;

namespace infinity {

export class Infinity {
public:
    Infinity() = default;

    ~Infinity() = default;

    u64 GetSessionId();

    static void LocalInit(const String &path);

    static void LocalUnInit();

    // Infinity API
public:
    static SharedPtr<Infinity> RemoteConnect();

    void RemoteDisconnect();

    static SharedPtr<Infinity> LocalConnect();

    void LocalDisconnect();

    QueryResult CreateDatabase(const String &db_name, const CreateDatabaseOptions &options);

    QueryResult DropDatabase(const String &db_name, const DropDatabaseOptions &options);

    QueryResult ListDatabases();

    QueryResult GetDatabase(const String &db_name);

    QueryResult ShowDatabase(const String &db_name);

    QueryResult Flush();

    QueryResult SetVariable(const String &variable_name, const String &variable_value, SetScope scope);

    QueryResult ShowVariable(const String &variable_name);

    // For embedded sqllogictest
    QueryResult Query(const String &query_text);

    // Database related functions
    QueryResult CreateTable(const String &db_name,
                            const String &table_name,
                            Vector<ColumnDef *> column_defs,
                            Vector<TableConstraint *> constraints,
                            const CreateTableOptions &create_table_options);

    QueryResult DropTable(const String &db_name, const String &table_name, const DropTableOptions &drop_table_options);

    QueryResult ListTables(const String &db_name);

    QueryResult ShowTable(const String &db_name, const String &table_name);

    QueryResult ShowColumns(const String &db_name, const String &table_name);

    QueryResult ListTableIndexes(const String &db_name, const String &table_name);

    QueryResult ShowTables(const String &db_name);

    QueryResult GetTable(const String &db_name, const String &table_name);

    // Table related functions
    QueryResult CreateIndex(const String &db_name,
                            const String &table_name,
                            const String &index_name,
                            Vector<IndexInfo *> *index_info_list,
                            const CreateIndexOptions &create_index_options);

    QueryResult DropIndex(const String &db_name, const String &table_name, const String &index_name, const DropIndexOptions &drop_index_option);

    QueryResult ShowIndex(const String &db_name, const String &table_name, const String &index_name);

    QueryResult ShowSegment(const String &db_name, const String &table_name, const SegmentID &segment_id);

    QueryResult ShowSegments(const String &db_name, const String &table_name);

    QueryResult ShowBlock(const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id);

    QueryResult ShowBlocks(const String &db_name, const String &table_name, const SegmentID &segment_id);

    QueryResult
    ShowBlockColumn(const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id, const SizeT &column_id);

    QueryResult Insert(const String &db_name, const String &table_name, Vector<String> *columns, Vector<Vector<ParsedExpr *> *> *values);

    QueryResult Import(const String &db_name, const String &table_name, const String &path, ImportOptions import_options);

    QueryResult Delete(const String &db_name, const String &table_name, ParsedExpr *filter);

    QueryResult Update(const String &db_name, const String &table_name, ParsedExpr *filter, Vector<UpdateExpr *> *update_list);

    QueryResult Explain(const String &db_name,
                        const String &table_name,
                        ExplainType explain_type,
                        SearchExpr *search_expr,
                        ParsedExpr *filter,
                        Vector<ParsedExpr *> *output_columns);

    QueryResult
    Search(const String &db_name, const String &table_name, SearchExpr *search_expr, ParsedExpr *filter, Vector<ParsedExpr *> *output_columns);

    QueryResult Optimize(const String &db_name, const String &table_name);

private:
    SharedPtr<BaseSession> session_{};
};

} // namespace infinity
