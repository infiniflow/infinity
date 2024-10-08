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
import select_statement;
import global_resource_usage;
import query_context;

namespace infinity {

export class Infinity {
public:
    Infinity() = default;

    ~Infinity() = default;

    u64 GetSessionId();

    static void LocalInit(const String &path);

    static void LocalUnInit();

    static void Hello();
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

    // Table related functions
    QueryResult CreateIndex(const String &db_name,
                            const String &table_name,
                            const String &index_name,
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

    // Metrics
    QueryResult ShowBuffer();
    QueryResult ShowProfiles();
    QueryResult ShowMemindex();
    QueryResult ShowQueries();
    QueryResult ShowQuery(u64 query_index);
    QueryResult ShowTransactions();
    QueryResult ShowLogs();
    QueryResult ShowDeltaCheckpoint();
    QueryResult ShowFullCheckpoint();
    QueryResult ShowObjects();
    QueryResult ShowObject(const String& object_name);
    QueryResult ShowFilesInObject();
    QueryResult ShowMemory();
    QueryResult ShowMemoryObjects();
    QueryResult ShowMemoryAllocations();
    QueryResult ShowFunction(const String& function_name);

    QueryResult Insert(const String &db_name, const String &table_name, Vector<String> *columns, Vector<Vector<ParsedExpr *> *> *values);

    QueryResult Import(const String &db_name, const String &table_name, const String &path, ImportOptions import_options);

    QueryResult
    Export(const String &db_name, const String &table_name, Vector<ParsedExpr *> *columns, const String &path, ExportOptions export_options);

    QueryResult Delete(const String &db_name, const String &table_name, ParsedExpr *filter);

    QueryResult Update(const String &db_name, const String &table_name, ParsedExpr *filter, Vector<UpdateExpr *> *update_list);

    QueryResult Explain(const String &db_name,
                        const String &table_name,
                        ExplainType explain_type,
                        SearchExpr *search_expr,
                        ParsedExpr *filter,
                        ParsedExpr *limit,
                        ParsedExpr *offset,
                        Vector<ParsedExpr *> *output_columns,
                        Vector<ParsedExpr *> *highlight_columns,
                        Vector<OrderByExpr *> *order_by_list);

    QueryResult Search(const String &db_name,
                       const String &table_name,
                       SearchExpr *search_expr,
                       ParsedExpr *filter,
                       ParsedExpr *limit,
                       ParsedExpr *offset,
                       Vector<ParsedExpr *> *output_columns,
                       Vector<ParsedExpr *> *highlight_columns,
                       Vector<OrderByExpr *> *order_by_list);

    QueryResult Optimize(const String &db_name, const String &table_name, OptimizeOptions optimize_options = OptimizeOptions{});

    QueryResult AddColumns(const String &db_name, const String &table_name, Vector<SharedPtr<ColumnDef>> column_defs);

    QueryResult DropColumns(const String &db_name, const String &table_name, Vector<String> column_names);

    QueryResult Cleanup();

    QueryResult ForceCheckpoint();
    QueryResult CompactTable(const String &db_name, const String& table_name);

    // Admin interface
    QueryResult AdminShowCatalogs();
    QueryResult AdminShowCatalog(i64 index);
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
    QueryResult AdminSetFollower(String node_name, const String& leader_address);
    QueryResult AdminSetLearner(String node_name, const String& leader_address);

private:
    UniquePtr<QueryContext> GetQueryContext() const;

    SharedPtr<BaseSession> session_{};
};

} // namespace infinity
