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

import :query_options;
import :infinity_type;

import std;
import std.compat;

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

    uint64_t GetSessionId();

    static void LocalInit(const std::string &path, const std::string &config_path = "");

    static void LocalUnInit();

    static void Hello();
    // Infinity API
public:
    static std::shared_ptr<Infinity> RemoteConnect();

    void RemoteDisconnect();

    static std::shared_ptr<Infinity> LocalConnect();

    void LocalDisconnect();

    QueryResult CreateDatabase(const std::string &db_name, const CreateDatabaseOptions &options, const std::string &db_comment);

    QueryResult DropDatabase(const std::string &db_name, const DropDatabaseOptions &options);

    QueryResult ListDatabases();

    QueryResult GetDatabase(const std::string &db_name);

    QueryResult ShowDatabase(const std::string &db_name);

    QueryResult CreateDatabaseSnapshot(const std::string &db_name, const std::string &snapshot_name);

    QueryResult RestoreDatabaseSnapshot(const std::string &snapshot_name);

    QueryResult CreateSystemSnapshot(const std::string &snapshot_name);

    QueryResult RestoreSystemSnapshot(const std::string &snapshot_name);

    QueryResult ShowSnapshot(const std::string &snapshot_name);

    QueryResult ListSnapshots();

    QueryResult DropSnapshot(const std::string &snapshot_name);

    QueryResult Flush(const std::string &flush_type = "data");

    QueryResult Compact(const std::string &db_name, const std::string &table_name);

    QueryResult SetVariableOrConfig(const std::string &name, bool value, SetScope scope);

    QueryResult SetVariableOrConfig(const std::string &name, i64 value, SetScope scope);

    QueryResult SetVariableOrConfig(const std::string &name, double value, SetScope scope);

    QueryResult SetVariableOrConfig(const std::string &name, std::string value, SetScope scope);

    QueryResult ShowVariable(const std::string &variable_name, SetScope scope);

    QueryResult ShowVariables(SetScope scope);

    QueryResult ShowConfig(const std::string &config_name);

    QueryResult ShowConfigs();

    // For embedded sqllogictest
    QueryResult Query(const std::string &query_text);

    // Database related functions
    QueryResult CreateTable(const std::string &db_name,
                            const std::string &table_name,
                            std::vector<ColumnDef *> column_defs,
                            std::vector<TableConstraint *> constraints,
                            CreateTableOptions create_table_options);

    QueryResult DropTable(const std::string &db_name, const std::string &table_name, const DropTableOptions &drop_table_options);

    QueryResult RenameTable(const std::string &db_name, const std::string &table_name, const std::string &new_table_name);

    QueryResult ShowTable(const std::string &db_name, const std::string &table_name);

    QueryResult ShowColumns(const std::string &db_name, const std::string &table_name);

    QueryResult ListTableIndexes(const std::string &db_name, const std::string &table_name);

    QueryResult ShowTables(const std::string &db_name);

    QueryResult GetTable(const std::string &db_name, const std::string &table_name);

    QueryResult CreateTableSnapshot(const std::string &db_name, const std::string &table_name, const std::string &snapshot_name);

    QueryResult RestoreTableSnapshot(const std::string &snapshot_name);

    // Table related functions
    QueryResult CreateIndex(const std::string &db_name,
                            const std::string &table_name,
                            const std::string &index_name,
                            const std::string &index_comment,
                            IndexInfo *index_info_ptr,
                            const CreateIndexOptions &create_index_options);

    QueryResult
    DropIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name, const DropIndexOptions &drop_index_option);

    QueryResult ShowIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name);

    QueryResult ShowIndexSegment(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id);

    QueryResult
    ShowIndexChunk(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id, ChunkID chunk_id);

    QueryResult ShowSegment(const std::string &db_name, const std::string &table_name, const SegmentID &segment_id);

    QueryResult ShowSegments(const std::string &db_name, const std::string &table_name);

    QueryResult ShowBlock(const std::string &db_name, const std::string &table_name, const SegmentID &segment_id, const BlockID &block_id);

    QueryResult ShowBlocks(const std::string &db_name, const std::string &table_name, const SegmentID &segment_id);

    QueryResult ShowBlockColumn(const std::string &db_name,
                                const std::string &table_name,
                                const SegmentID &segment_id,
                                const BlockID &block_id,
                                const std::size_t &column_id);

    // TODO: ask whether need to add snapshot related functions
    // QueryResult CreateTableSnapshot(const std::string &db_name, const std::string &table_name, const std::string &snapshot_name);

    // QueryResult RestoreTableSnapshot(const std::string &snapshot_name);

    // QueryResult ShowSnapshots();

    // QueryResult DropSnapshot(const std::string &snapshot_name);

    // Metrics
    QueryResult ShowBuffer();
    QueryResult ShowProfiles();
    QueryResult ShowMemindex();
    QueryResult ShowQueries();
    QueryResult ShowQuery(uint64_t query_index);
    QueryResult ShowTransactions();
    QueryResult ShowLogs();
    QueryResult ShowObjects();
    QueryResult ShowObject(const std::string &object_name);
    QueryResult ShowFilesInObject();
    QueryResult ShowMemory();
    QueryResult ShowMemoryObjects();
    QueryResult ShowMemoryAllocations();
    QueryResult ShowFunction(const std::string &function_name);

    QueryResult Insert(const std::string &db_name, const std::string &table_name, std::vector<InsertRowExpr *> *&insert_rows);

    QueryResult Import(const std::string &db_name, const std::string &table_name, const std::string &path, ImportOptions import_options);

    QueryResult Export(const std::string &db_name,
                       const std::string &table_name,
                       std::vector<ParsedExpr *> *columns,
                       const std::string &path,
                       ExportOptions export_options);

    QueryResult Delete(const std::string &db_name, const std::string &table_name, ParsedExpr *filter);

    QueryResult Update(const std::string &db_name, const std::string &table_name, ParsedExpr *filter, std::vector<UpdateExpr *> *update_list);

    QueryResult Explain(const std::string &db_name,
                        const std::string &table_name,
                        ExplainType explain_type,
                        SearchExpr *&search_expr,
                        ParsedExpr *filter,
                        ParsedExpr *limit,
                        ParsedExpr *offset,
                        std::vector<ParsedExpr *> *output_columns,
                        std::vector<ParsedExpr *> *highlight_columns,
                        std::vector<OrderByExpr *> *order_by_list,
                        std::vector<ParsedExpr *> *group_by_list,
                        ParsedExpr *having);

    QueryResult Search(const std::string &db_name,
                       const std::string &table_name,
                       SearchExpr *&search_expr,
                       ParsedExpr *filter,
                       ParsedExpr *limit,
                       ParsedExpr *offset,
                       std::vector<ParsedExpr *> *&output_columns,
                       std::vector<ParsedExpr *> *highlight_columns,
                       std::vector<OrderByExpr *> *order_by_list,
                       std::vector<ParsedExpr *> *group_by_list,
                       ParsedExpr *having,
                       bool total_hits_count_flag);

    QueryResult Optimize(const std::string &db_name, const std::string &table_name, OptimizeOptions optimize_options = OptimizeOptions{});

    QueryResult AddColumns(const std::string &db_name, const std::string &table_name, std::vector<std::shared_ptr<ColumnDef>> column_defs);

    QueryResult DropColumns(const std::string &db_name, const std::string &table_name, std::vector<std::string> column_names);

    QueryResult Cleanup();

    QueryResult DumpIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name);

    QueryResult ForceCheckpoint();
    QueryResult CompactTable(const std::string &db_name, const std::string &table_name);

    QueryResult TestCommand(const std::string &command_content);

    // Admin interface
    QueryResult AdminShowLogs();
    QueryResult AdminShowLog(i64 index);
    QueryResult AdminShowConfigs();
    QueryResult AdminShowVariables();
    QueryResult AdminShowVariable(std::string var_name);
    QueryResult AdminShowNodes();
    QueryResult AdminShowNode(std::string var_name);
    QueryResult AdminShowCurrentNode();
    QueryResult AdminSetAdmin();
    QueryResult AdminSetStandalone();
    QueryResult AdminSetLeader(std::string node_name);
    QueryResult AdminSetFollower(std::string node_name, const std::string &leader_address);
    QueryResult AdminSetLearner(std::string node_name, const std::string &leader_address);
    QueryResult AdminRemoveNode(std::string var_name);

private:
    std::variant<std::unique_ptr<QueryContext>, QueryResult> GetQueryContext(bool is_admin_stmt = false, bool is_admin_show_node = false) const;

    std::shared_ptr<BaseSession> session_{};
};

} // namespace infinity
