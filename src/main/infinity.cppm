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

export module infinity_core:infinity;

import :stl;
import :query_options;
import command_statement;
import column_def;
import create_index_info;
import insert_row_expr;
import parsed_expr;
import update_statement;
import explain_statement;
import search_expr;
import select_statement;

namespace infinity {
class BaseSession;
class QueryContext;
class QueryResult;

export class Infinity {
public:
    Infinity() = default;

    ~Infinity() = default;

    u64 GetSessionId();

    static void LocalInit(const String &path, const String &config_path = "");

    static void LocalUnInit();

    static void Hello();
    // Infinity API
public:
    static SharedPtr<Infinity> RemoteConnect();

    void RemoteDisconnect();

    static SharedPtr<Infinity> LocalConnect();

    void LocalDisconnect();

    QueryResult CreateDatabase(const String &db_name, const CreateDatabaseOptions &options, const String &db_comment);

    QueryResult DropDatabase(const String &db_name, const DropDatabaseOptions &options);

    QueryResult ListDatabases();

    QueryResult GetDatabase(const String &db_name);

    QueryResult ShowDatabase(const String &db_name);

    QueryResult CreateDatabaseSnapshot(const String &db_name, const String &snapshot_name);

    QueryResult RestoreDatabaseSnapshot(const String &snapshot_name);

    QueryResult CreateSystemSnapshot(const String &snapshot_name);

    QueryResult RestoreSystemSnapshot(const String &snapshot_name);

    QueryResult ShowSnapshot(const String &snapshot_name);

    QueryResult ListSnapshots();

    QueryResult DropSnapshot(const String &snapshot_name);

    QueryResult Flush(const String &flush_type = "data");

    QueryResult Compact(const String &db_name, const String &table_name);

    QueryResult SetVariableOrConfig(const String &name, bool value, SetScope scope);

    QueryResult SetVariableOrConfig(const String &name, i64 value, SetScope scope);

    QueryResult SetVariableOrConfig(const String &name, double value, SetScope scope);

    QueryResult SetVariableOrConfig(const String &name, String value, SetScope scope);

    QueryResult ShowVariable(const String &variable_name, SetScope scope);

    QueryResult ShowVariables(SetScope scope);

    QueryResult ShowConfig(const String &config_name);

    QueryResult ShowConfigs();

    // For embedded sqllogictest
    QueryResult Query(const String &query_text);

    // Database related functions
    QueryResult CreateTable(const String &db_name,
                            const String &table_name,
                            Vector<ColumnDef *> column_defs,
                            Vector<TableConstraint *> constraints,
                            CreateTableOptions create_table_options);

    QueryResult DropTable(const String &db_name, const String &table_name, const DropTableOptions &drop_table_options);

    QueryResult ListTables(const String &db_name);

    QueryResult ShowTable(const String &db_name, const String &table_name);

    QueryResult ShowColumns(const String &db_name, const String &table_name);

    QueryResult ListTableIndexes(const String &db_name, const String &table_name);

    QueryResult ShowTables(const String &db_name);

    QueryResult GetTable(const String &db_name, const String &table_name);

    QueryResult CreateTableSnapshot(const String &db_name, const String &table_name, const String &snapshot_name);

    QueryResult RestoreTableSnapshot(const String &snapshot_name);

    // Table related functions
    QueryResult CreateIndex(const String &db_name,
                            const String &table_name,
                            const String &index_name,
                            const String &index_comment,
                            IndexInfo *index_info_ptr,
                            const CreateIndexOptions &create_index_options);

    QueryResult DropIndex(const String &db_name, const String &table_name, const String &index_name, const DropIndexOptions &drop_index_option);

    QueryResult ShowIndex(const String &db_name, const String &table_name, const String &index_name);

    QueryResult ShowIndexSegment(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id);

    QueryResult ShowIndexChunk(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id, ChunkID chunk_id);

    QueryResult ShowSegment(const String &db_name, const String &table_name, const SegmentID &segment_id);

    QueryResult ShowSegments(const String &db_name, const String &table_name);

    QueryResult ShowBlock(const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id);

    QueryResult ShowBlocks(const String &db_name, const String &table_name, const SegmentID &segment_id);

    QueryResult
    ShowBlockColumn(const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id, const SizeT &column_id);

    // TODO: ask whether need to add snapshot related functions
    // QueryResult CreateTableSnapshot(const String &db_name, const String &table_name, const String &snapshot_name);

    // QueryResult RestoreTableSnapshot(const String &snapshot_name);

    // QueryResult ShowSnapshots();

    // QueryResult DropSnapshot(const String &snapshot_name);

    // Metrics
    QueryResult ShowBuffer();
    QueryResult ShowProfiles();
    QueryResult ShowMemindex();
    QueryResult ShowQueries();
    QueryResult ShowQuery(u64 query_index);
    QueryResult ShowTransactions();
    QueryResult ShowLogs();
    QueryResult ShowObjects();
    QueryResult ShowObject(const String &object_name);
    QueryResult ShowFilesInObject();
    QueryResult ShowMemory();
    QueryResult ShowMemoryObjects();
    QueryResult ShowMemoryAllocations();
    QueryResult ShowFunction(const String &function_name);
    QueryResult ListCaches();
    QueryResult ShowCache();

    QueryResult Insert(const String &db_name, const String &table_name, Vector<InsertRowExpr *> *&insert_rows);

    QueryResult Import(const String &db_name, const String &table_name, const String &path, ImportOptions import_options);

    QueryResult
    Export(const String &db_name, const String &table_name, Vector<ParsedExpr *> *columns, const String &path, ExportOptions export_options);

    QueryResult Delete(const String &db_name, const String &table_name, ParsedExpr *filter);

    QueryResult Update(const String &db_name, const String &table_name, ParsedExpr *filter, Vector<UpdateExpr *> *update_list);

    QueryResult Explain(const String &db_name,
                        const String &table_name,
                        ExplainType explain_type,
                        SearchExpr *&search_expr,
                        ParsedExpr *filter,
                        ParsedExpr *limit,
                        ParsedExpr *offset,
                        Vector<ParsedExpr *> *output_columns,
                        Vector<ParsedExpr *> *highlight_columns,
                        Vector<OrderByExpr *> *order_by_list,
                        Vector<ParsedExpr *> *group_by_list,
                        ParsedExpr *having);

    QueryResult Search(const String &db_name,
                       const String &table_name,
                       SearchExpr *&search_expr,
                       ParsedExpr *filter,
                       ParsedExpr *limit,
                       ParsedExpr *offset,
                       Vector<ParsedExpr *> *&output_columns,
                       Vector<ParsedExpr *> *highlight_columns,
                       Vector<OrderByExpr *> *order_by_list,
                       Vector<ParsedExpr *> *group_by_list,
                       ParsedExpr *having,
                       bool total_hits_count_flag);

    QueryResult Optimize(const String &db_name, const String &table_name, OptimizeOptions optimize_options = OptimizeOptions{});

    QueryResult AddColumns(const String &db_name, const String &table_name, Vector<SharedPtr<ColumnDef>> column_defs);

    QueryResult DropColumns(const String &db_name, const String &table_name, Vector<String> column_names);

    QueryResult Cleanup();

    QueryResult DumpIndex(const String &db_name, const String &table_name, const String &index_name);

    QueryResult ForceCheckpoint();
    QueryResult CompactTable(const String &db_name, const String &table_name);

    QueryResult TestCommand(const String &command_content);

    // Admin interface
    QueryResult AdminShowLogs();
    QueryResult AdminShowLog(i64 index);
    QueryResult AdminShowConfigs();
    QueryResult AdminShowVariables();
    QueryResult AdminShowVariable(String var_name);
    QueryResult AdminShowNodes();
    QueryResult AdminShowNode(String var_name);
    QueryResult AdminShowCurrentNode();
    QueryResult AdminSetAdmin();
    QueryResult AdminSetStandalone();
    QueryResult AdminSetLeader(String node_name);
    QueryResult AdminSetFollower(String node_name, const String &leader_address);
    QueryResult AdminSetLearner(String node_name, const String &leader_address);
    QueryResult AdminRemoveNode(String var_name);

private:
    std::variant<UniquePtr<QueryContext>, QueryResult> GetQueryContext(bool is_admin_stmt = false, bool is_admin_show_node = false) const;

    SharedPtr<BaseSession> session_{};
};

} // namespace infinity
