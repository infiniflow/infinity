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

module infinity_core:physical_show.impl;

import :physical_show;

namespace infinity {

void PhysicalShow::Init(QueryContext *query_context) {
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);
    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    auto double_type = std::make_shared<DataType>(LogicalType::kDouble);

    output_names_ = std::make_shared<std::vector<std::string>>();
    output_types_ = std::make_shared<std::vector<std::shared_ptr<DataType>>>();

    switch (show_type_) {
        case ShowStmtType::kDatabase: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kTable: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kIndex: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kIndexSegment: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kIndexChunks: {
            *output_names_ = {"id", "name", "row_count"};
            *output_types_ = {bigint_type, varchar_type, bigint_type};
            break;
        }
        case ShowStmtType::kIndexChunk: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kDatabases: {
            *output_names_ = {"database", "dir", "comment"};
            *output_types_ = {varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kTables: {
            *output_names_ = {"database", "table", "table_id", "column_count", "segment_count", "block_count", "create_timestamp", "comment"};
            *output_types_ = {varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, varchar_type};
            break;
        }
        case ShowStmtType::kColumns: {
            *output_names_ = {"name", "type", "default", "comment"};
            *output_types_ = {varchar_type, varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kIndexes: {
            *output_names_ = {"index_name", "index_comment", "index_type", "column_id", "column_name", "path", "index segments", "other_parameters"};
            *output_types_ = {varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kConfigs: {
            *output_names_ = {"config_name", "value", "description"};
            *output_types_ = {varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kProfiles: {
            *output_names_ = {"record_no",
                              "command parsing",
                              "logical plan building",
                              "plan optimizing",
                              "physical plan building",
                              "pipeline building",
                              "task building",
                              "execution",
                              "commit",
                              "rollback",
                              "total_cost"};

            *output_types_ = {varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type,
                              varchar_type};
            break;
        }
        case ShowStmtType::kSegments: {
            *output_names_ = {"id", "status", "size"};
            *output_types_ = {bigint_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kSegment: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }

        case ShowStmtType::kBlocks: {
            *output_names_ = {"id", "size", "row_count"};
            *output_types_ = {bigint_type, varchar_type, bigint_type};
            break;
        }

        case ShowStmtType::kBlock: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }

        case ShowStmtType::kBlockColumn: {
            *output_names_ = {"name", "description"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }

        case ShowStmtType::kSessionVariable: {
            *output_names_ = {"value"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kGlobalVariable: {
            *output_names_ = {"value"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kSessionVariables: {
            *output_names_ = {"name", "value", "description"};
            *output_types_ = {varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kGlobalVariables: {
            *output_names_ = {"name", "value", "description"};
            *output_types_ = {varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kConfig: {
            *output_names_ = {"value"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kBuffer: {
            *output_names_ = {"path", "status", "size", "buffered_type", "type"};
            *output_types_ = {varchar_type, varchar_type, bigint_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kMemIndex: {
            *output_names_ = {"index_name", "table_name", "db_name", "size", "row_count"};
            *output_types_ = {varchar_type, varchar_type, varchar_type, bigint_type, bigint_type};
            break;
        }
        case ShowStmtType::kQueries: {
            *output_names_ = {"session_id", "query_id", "query_kind", "start_time", "time_consumption"};
            *output_types_ = {bigint_type, bigint_type, varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kQuery: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kTransactions: {
            *output_names_ = {"transaction_id", "transaction_text"};
            *output_types_ = {bigint_type, varchar_type};
            break;
        }
        case ShowStmtType::kTransaction: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            *output_names_ = {"txn_id", "text", "begin_ts", "commit_ts", "state", "type", "operations"};
            *output_types_ = {varchar_type, varchar_type, bigint_type, bigint_type, varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kLogs: {
            *output_names_ = {"commit_ts", "transaction_id", "command_type", "text"};
            *output_types_ = {bigint_type, bigint_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kCatalog: {
            *output_names_ = {"meta_data_json"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kListCatalogKey: {
            *output_names_ = {"key", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kCatalogToFile: {
            *output_names_ = {"operator_status"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            *output_names_ = {"file_name", "object_name", "offset", "size"};
            *output_types_ = {varchar_type, varchar_type, bigint_type, bigint_type};
            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            *output_names_ = {"name", "reference_count", "size", "parts", "deleted_ranges"};
            *output_types_ = {varchar_type, bigint_type, bigint_type, bigint_type, varchar_type};
            break;
        }
        case ShowStmtType::kPersistenceObject: {
            *output_names_ = {"start", "end"};
            *output_types_ = {bigint_type, bigint_type};
            break;
        }
        case ShowStmtType::kMemory: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kMemoryObjects: {
            *output_names_ = {"name", "count"};
            *output_types_ = {varchar_type, bigint_type};
            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            *output_names_ = {"name", "total_size"};
            *output_types_ = {varchar_type, bigint_type};
            break;
        }
        case ShowStmtType::kFunction: {
            *output_names_ = {"value"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kListSnapshots: {
            *output_names_ = {"name", "scope", "time", "commit", "size"};
            *output_types_ = {varchar_type, varchar_type, varchar_type, bigint_type, varchar_type};
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            *output_names_ = {"name", "value"};
            *output_types_ = {varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kTasks: {
            *output_names_ = {"time", "type", "status", "description"};
            *output_types_ = {varchar_type, varchar_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kListCaches: {
            *output_names_ = {"type", "name", "commit_ts", "detail"};
            *output_types_ = {varchar_type, varchar_type, bigint_type, varchar_type};
            break;
        }
        case ShowStmtType::kShowCache: {
            *output_names_ = {"cache_type", "number", "request_count", "hit_count", "hit_rate"};
            *output_types_ = {varchar_type, bigint_type, bigint_type, bigint_type, double_type};
            break;
        }
        case ShowStmtType::kListCompact: {
            *output_names_ = {"txn", "begin_ts", "commit_ts", "committed", "detail"};
            *output_types_ = {bigint_type, bigint_type, bigint_type, bool_type, varchar_type};
            break;
        }
        case ShowStmtType::kListCheckpoint: {
            *output_names_ = {"txn", "begin_ts", "commit_ts", "committed", "timestamp", "type", "brief"};
            *output_types_ = {bigint_type, bigint_type, bigint_type, bool_type, bigint_type, varchar_type, varchar_type};
            break;
        }
        case ShowStmtType::kShowCheckpoint: {
            *output_names_ = {"detail"};
            *output_types_ = {varchar_type};
            break;
        }
        case ShowStmtType::kListOptimize: {
            *output_names_ = {"txn", "begin_ts", "commit_ts", "committed", "detail"};
            *output_types_ = {bigint_type, bigint_type, bigint_type, bool_type, varchar_type};
            break;
        }
        case ShowStmtType::kListImport: {
            *output_names_ = {"txn", "begin", "commit", "committed", "detail"};
            *output_types_ = {bigint_type, bigint_type, bigint_type, bool_type, varchar_type};
            break;
        }
        case ShowStmtType::kListClean: {
            *output_names_ = {"txn", "begin_ts", "commit_ts", "committed", "brief"};
            *output_types_ = {bigint_type, bigint_type, bigint_type, bool_type, varchar_type};
            break;
        }
        case ShowStmtType::kShowClean: {
            *output_names_ = {"detail"};
            *output_types_ = {varchar_type};
            break;
        }
        default: {
            Status status = Status::NotSupport("Not implemented show type");
            RecoverableError(status);
        }
    }
}

bool PhysicalShow::Execute(QueryContext *query_context, OperatorState *operator_state) {
    ShowOperatorState *show_operator_state = (ShowOperatorState *)(operator_state);
    DeferFn defer_fn([&]() { show_operator_state->SetComplete(); });

    switch (show_type_) {
        case ShowStmtType::kDatabase: {
            ExecuteShowDatabase(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kTable: {
            ExecuteShowTable(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kIndex: {
            ExecuteShowIndex(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kIndexSegment: {
            ExecuteShowIndexSegment(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kIndexChunks: {
            ExecuteShowIndexChunks(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kIndexChunk: {
            ExecuteShowIndexChunk(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kDatabases: {
            ExecuteShowDatabases(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kTables: {
            ExecuteShowTables(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kColumns: {
            ExecuteShowColumns(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kIndexes: {
            ExecuteShowIndexes(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kConfigs: {
            ExecuteShowConfigs(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kProfiles: {
            ExecuteShowProfiles(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kSegments: {
            ExecuteShowSegments(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kSegment: {
            ExecuteShowSegmentDetail(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kBlocks: {
            ExecuteShowBlocks(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kBlock: {
            ExecuteShowBlockDetail(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kBlockColumn: {
            ExecuteShowBlockColumn(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kTasks: {
            ExecuteShowTasks(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kSessionVariable: {
            ExecuteShowSessionVariable(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kGlobalVariable: {
            ExecuteShowGlobalVariable(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kSessionVariables: {
            ExecuteShowSessionVariables(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kGlobalVariables: {
            ExecuteShowGlobalVariables(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kConfig: {
            ExecuteShowConfig(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kBuffer: {
            ExecuteShowBuffer(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kMemIndex: {
            ExecuteShowMemIndex(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kQueries: {
            ExecuteShowQueries(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kQuery: {
            ExecuteShowQuery(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kTransactions: {
            ExecuteShowTransactions(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            ExecuteShowTransactionHistory(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kTransaction: {
            ExecuteShowTransaction(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kLogs: {
            ExecuteShowLogs(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kCatalog: {
            ExecuteShowCatalog(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListCatalogKey: {
            ExecuteListCatalogKey(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kCatalogToFile: {
            ExecuteShowCatalogToFile(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            ExecuteShowPersistenceFiles(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            ExecuteShowPersistenceObjects(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kPersistenceObject: {
            ExecuteShowPersistenceObject(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kMemory: {
            ExecuteShowMemory(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kMemoryObjects: {
            ExecuteShowMemoryObjects(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            ExecuteShowMemoryAllocation(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kFunction: {
            ExecuteShowFunction(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListSnapshots: {
            ExecuteListSnapshots(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            ExecuteShowSnapshot(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListCaches: {
            ExecuteListCaches(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kShowCache: {
            ExecuteShowCache(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListCompact: {
            ExecuteListCompact(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListCheckpoint: {
            ExecuteListCheckpoint(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kShowCheckpoint: {
            ExecuteShowCheckpoint(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListOptimize: {
            ExecuteListOptimize(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListImport: {
            ExecuteListImport(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kListClean: {
            ExecuteListClean(query_context, show_operator_state);
            break;
        }
        case ShowStmtType::kShowClean: {
            ExecuteShowClean(query_context, show_operator_state);
            break;
        }
        default: {
            UnrecoverableError("Invalid chunk scan type");
        }
    }
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
