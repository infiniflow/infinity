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

#include <iomanip>
#include <map>
#include <string>
#include <unordered_map>

module infinity_core:physical_show.impl;

import :physical_show;
import :stl;
import :new_txn;
import :query_context;

import :profiler;
import :operator_state;
import :data_block;

import :infinity_exception;
import :value_expression;
import :logical_show;
import :meta_info;

import :value;
import :table_def;
import :data_table;
import :third_party;
import :index_base;
import :index_hnsw;
import :index_full_text;
import :database_detail;
import :default_values;
import :defer_op;
import :config;
import :session;
import :options;
import :status;
import :virtual_store;
import :utility;
import :buffer_manager;
import :session_manager;
import compilation_config;
import logical_type;
import create_index_info;
import :variables;
import :default_values;
import :wal_manager;
import :logger;
import :memory_indexer;
import :background_process;
import :compaction_process;
import :bg_task;
import :bg_task_type;
import :buffer_obj;
import :file_worker_type;
import :system_info;
import :wal_entry;
import :memindex_tracer;
import :persistence_manager;
import global_resource_usage;
import :infinity_context;
import :obj_status;
import admin_statement;
import :result_cache_manager;
import :peer_task;
import :node_info;
import :txn_context;
import :txn_state;
import :snapshot_brief;
import command_statement;
import :chunk_index_meta;
import :new_txn_manager;
import :kv_store;
import :meta_tree;
import show_statement;
import column_def;
import data_type;
import row_id;

namespace infinity {

void PhysicalShow::Init(QueryContext *query_context) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (show_type_) {
        case ShowStmtType::kDatabase: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kTable: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kIndex: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kIndexSegment: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kIndexChunk: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_types_->emplace_back(varchar_type);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kDatabases: {
            output_names_->reserve(3);
            output_types_->reserve(3);
            output_names_->emplace_back("database");
            output_names_->emplace_back("dir");
            output_names_->emplace_back("comment");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kTables: {

            output_names_->reserve(9);
            output_types_->reserve(9);

            output_names_->emplace_back("database");
            output_names_->emplace_back("table");
            output_names_->emplace_back("column_count");
            output_names_->emplace_back("block_count");
            output_names_->emplace_back("block_capacity");
            output_names_->emplace_back("segment_count");
            output_names_->emplace_back("segment_capacity");
            output_names_->emplace_back("comment");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kColumns: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("name");
            output_names_->emplace_back("type");
            output_names_->emplace_back("default");
            output_names_->emplace_back("comment");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kIndexes: {

            output_names_->reserve(8);
            output_types_->reserve(8);

            output_names_->emplace_back("index_name");
            output_names_->emplace_back("index_comment");
            output_names_->emplace_back("index_type");
            output_names_->emplace_back("column_id");
            output_names_->emplace_back("column_name");
            output_names_->emplace_back("path");
            output_names_->emplace_back("index segments");
            output_names_->emplace_back("other_parameters");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kConfigs: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("config_name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kProfiles: {

            output_names_->reserve(9);
            output_types_->reserve(9);

            output_names_->emplace_back("record_no");
            output_names_->emplace_back("command parsing");
            output_names_->emplace_back("logical plan building");
            output_names_->emplace_back("plan optimizing");
            output_names_->emplace_back("physical plan building");
            output_names_->emplace_back("pipeline building");
            output_names_->emplace_back("task building");
            output_names_->emplace_back("execution");
            output_names_->emplace_back("commit");
            output_names_->emplace_back("rollback");
            output_names_->emplace_back("total_cost");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kSegments: {
            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("id");
            output_names_->emplace_back("status");
            output_names_->emplace_back("size");

            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kSegment: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }

        case ShowStmtType::kBlocks: {
            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("id");
            output_names_->emplace_back("size");
            output_names_->emplace_back("row_count");

            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            break;
        }

        case ShowStmtType::kBlock: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }

        case ShowStmtType::kBlockColumn: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("name");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }

        case ShowStmtType::kSessionVariable: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kGlobalVariable: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kSessionVariables: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kGlobalVariables: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kConfig: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kBuffer: {
            output_names_->reserve(5);
            output_types_->reserve(5);
            output_names_->emplace_back("path");
            output_names_->emplace_back("status");
            output_names_->emplace_back("size");
            output_names_->emplace_back("buffered_type");
            output_names_->emplace_back("type");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kMemIndex: {
            output_names_->reserve(5);
            output_types_->reserve(5);
            output_names_->emplace_back("index_name");
            output_names_->emplace_back("table_name");
            output_names_->emplace_back("db_name");
            output_names_->emplace_back("size");
            output_names_->emplace_back("row_count");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowStmtType::kQueries: {
            output_names_->reserve(5);
            output_types_->reserve(5);
            output_names_->emplace_back("session_id");
            output_names_->emplace_back("query_id");
            output_names_->emplace_back("query_kind");
            output_names_->emplace_back("start_time");
            output_names_->emplace_back("time_consumption");
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kQuery: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kTransactions: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("transaction_id");
            output_names_->emplace_back("transaction_text");
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kTransaction: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            output_names_->reserve(7);
            output_types_->reserve(7);
            output_names_->emplace_back("txn_id");
            output_names_->emplace_back("text");
            output_names_->emplace_back("begin_ts");
            output_names_->emplace_back("commit_ts");
            output_names_->emplace_back("state");
            output_names_->emplace_back("type");
            output_names_->emplace_back("operations");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kLogs: {
            output_names_->reserve(4);
            output_types_->reserve(4);
            output_names_->emplace_back("commit_ts");
            output_names_->emplace_back("transaction_id");
            output_names_->emplace_back("command_type");
            output_names_->emplace_back("text");
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kCatalog: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("meta_data_json");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kCatalogToFile: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("operator_status");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            output_names_->reserve(4);
            output_types_->reserve(4);
            output_names_->emplace_back("file_name");
            output_names_->emplace_back("object_name");
            output_names_->emplace_back("offset");
            output_names_->emplace_back("size");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            output_names_->reserve(5);
            output_types_->reserve(5);
            output_names_->emplace_back("name");
            output_names_->emplace_back("reference_count");
            output_names_->emplace_back("size");
            output_names_->emplace_back("parts");
            output_names_->emplace_back("deleted_ranges");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kPersistenceObject: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("start");
            output_names_->emplace_back("end");
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowStmtType::kMemory: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kMemoryObjects: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_names_->emplace_back("count");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_names_->emplace_back("total_size");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowStmtType::kFunction: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kListSnapshots: {
            output_names_->reserve(5);
            output_types_->reserve(5);
            output_names_->emplace_back("name");
            output_names_->emplace_back("scope");
            output_names_->emplace_back("time");
            output_names_->emplace_back("commit");
            output_names_->emplace_back("size");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            output_names_->reserve(2);
            output_types_->reserve(2);
            output_names_->emplace_back("name");
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowStmtType::kTasks: {
            output_names_->reserve(4);
            output_types_->reserve(4);
            output_names_->emplace_back("time");
            output_names_->emplace_back("type");
            output_names_->emplace_back("status");
            output_names_->emplace_back("description");
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
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
        default: {
            String error_message = "Invalid chunk scan type";
            UnrecoverableError(error_message);
        }
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalShow::ExecuteShowDatabase(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output database detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    SharedPtr<DatabaseInfo> database_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(database_info, status) = txn->GetDatabaseInfo(db_name_);

    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append database name to the 1 column
            const String *table_name = database_info->db_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append database storage directory to the 1 column
            Value value = Value::MakeVarchar(database_info->absolute_db_path_ ? *database_info->absolute_db_path_ : "N/A");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append database storage directory to the 1 column
            Value value = Value::MakeVarchar(std::to_string(database_info->table_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("comment");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*(database_info->db_comment_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowTable(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    SharedPtr<TableInfo> table_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(table_info, status) = txn->GetTableInfo(db_name_, *object_name_);
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(db_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const String *table_name = table_info->table_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_comment");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_info->table_comment_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_info->table_full_dir_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_info->column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_info->segment_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_info->row_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndex(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    SharedPtr<TableIndexInfo> table_index_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(table_index_info, status) = txn->GetTableIndexInfo(db_name_, *object_name_, index_name_.value());
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(db_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("table_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*object_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(index_name_.value());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_comment");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*(table_index_info->index_comment_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_type");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_type_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_column_names");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_column_names_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_column_ids");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_column_ids_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("other_parameters");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_other_params_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            const String *table_dir = table_index_info->index_entry_dir_.get();
            Value value = Value::MakeVarchar(*table_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    //    {
    //        SizeT column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("storage_size");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            const String table_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *table_index_info->index_entry_dir_);
    //            String index_size_str;
    //            if (query_context->persistence_manager() == nullptr) {
    //                index_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(table_dir));
    //            } else {
    //                const Vector<String> &paths = table_index_info->files_;
    //                SizeT index_size = 0;
    //                for (const String &path : paths) {
    //                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
    //                    if (!status.ok()) {
    //                        RecoverableError(status);
    //                    }
    //                    index_size += file_size;
    //                }
    //                index_size_str = Utility::FormatByteSize(index_size);
    //            }
    //            Value value = Value::MakeVarchar(index_size_str);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_index_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_index_info->segment_index_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexSegment(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    SharedPtr<SegmentIndexInfo> segment_index_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(segment_index_info, status) = txn->GetSegmentIndexInfo(db_name_, *object_name_, index_name_.value(), segment_id_.value());
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_id_.value()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    String full_segment_index_dir = Path(InfinityContext::instance().config()->DataDir()) / *segment_index_info->index_dir_;
    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(full_segment_index_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("index_segment_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String index_size_str;
            if (query_context->persistence_manager() == nullptr) {
                index_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(full_segment_index_dir));
            } else {
                Vector<String> &paths = segment_index_info->files_;
                SizeT index_segment_size = 0;
                for (const String &path : paths) {
                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
                    if (!status.ok()) {
                        RecoverableError(status);
                    }
                    index_segment_size += file_size;
                }
                index_size_str = Utility::FormatByteSize(index_segment_size);
            }

            Value value = Value::MakeVarchar(index_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("chunk_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_index_info->chunk_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexChunk(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table detailed info
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    String base_name;
    RowID base_row_id;
    SizeT row_cnt = 0;
    TxnTimeStamp deprecate_ts = 0;

    // Get tables from catalog
    NewTxn *txn = query_context->GetNewTxn();
    auto [chunk_index_info, status] = txn->GetChunkIndexInfo(db_name_, *object_name_, index_name_.value(), segment_id_.value(), chunk_id_.value());
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    base_name = chunk_index_info->base_name_;
    base_row_id = chunk_index_info->base_row_id_;
    row_cnt = chunk_index_info->row_cnt_;

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("file_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(base_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("start_row");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(base_row_id.ToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(row_cnt));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("deprecate_timestamp");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(deprecate_ts));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Vector<DatabaseDetail> databases_detail;
    NewTxn *txn = query_context->GetNewTxn();
    Vector<String> db_names;
    Status status = txn->ListDatabase(db_names);
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }
    for (auto &db_name : db_names) {
        auto [database_info, status] = txn->GetDatabaseInfo(db_name);
        if (!status.ok()) {
            RecoverableError(status);
            return;
        }
        databases_detail.emplace_back(DatabaseDetail{
            .db_name_ = MakeShared<String>(db_name),
            .db_entry_dir_ = std::move(database_info->db_entry_dir_),
            .db_comment_ = std::move(database_info->db_comment_),
        });
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, varchar_type};
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (auto &database_detail : databases_detail) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = database_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append entry dir to the 1 column
            const String *db_entry_dir = database_detail.db_entry_dir_.get();
            Value value = Value::MakeVarchar(*db_entry_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append comment to the 2 column
            const String *db_comment = database_detail.db_comment_.get();
            Value value = Value::MakeVarchar(*db_comment);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowTables(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Get tables from catalog
    Vector<SharedPtr<TableDetail>> tables_detail;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    status = txn->GetTables(db_name_, tables_detail);
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (auto &table_detail_ptr : tables_detail) {
        // Initialize the output data block
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_detail_ptr->db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_detail_ptr->table_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count the 3 column
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append block count the 4 column
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->block_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append block capacity the 5 column
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->block_capacity_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append segment count the 6 column
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->segment_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append segment capacity the 7 column
            SizeT default_row_size = table_detail_ptr->segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table comment the 8 column
            Value value = Value::MakeVarchar(*table_detail_ptr->table_comment_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowTasks(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    NewTxnManager *txn_mgr = query_context->storage()->new_txn_manager();
    Vector<SharedPtr<BGTaskInfo>> bg_task_info_list = txn_mgr->GetTaskInfoList();
    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    for (SizeT i = 0; i < bg_task_info_list.size(); ++i) {
        BGTaskInfo *bg_task_info_ptr = bg_task_info_list[i].get();
        SizeT task_count_ = bg_task_info_ptr->task_info_list_.size();
        for (SizeT j = 0; j < task_count_; ++j) {
            if (!output_block_ptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            SizeT column_id = 0;
            {
                std::time_t task_time_t = std::chrono::system_clock::to_time_t(bg_task_info_ptr->task_time_);
                std::tm *task_tm = std::localtime(&task_time_t);
                std::ostringstream oss;
                oss << std::put_time(task_tm, "%Y-%m-%d %H:%M:%S");
                Value value = Value::MakeVarchar(oss.str());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(bg_task_info_ptr->type_));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(bg_task_info_ptr->status_list_[j]);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            ++column_id;
            {
                String task_text = bg_task_info_ptr->task_info_list_[j];
                Value value = Value::MakeVarchar(task_text);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            if (++row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                show_operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowProfiles(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto catalog = query_context->storage()->new_catalog();

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type,
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
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    auto records = catalog->GetProfileRecords();
    for (SizeT i = 0; i < records.size(); ++i) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        // Output record no
        ValueExpression record_no_expr(Value::MakeVarchar(fmt::format("{}", i)));
        record_no_expr.AppendToChunk(output_block_ptr->column_vectors[0]);

        // Output each query phase
        i64 total_cost{};
        SizeT column_count = column_types.size();
        for (SizeT j = 0; j < column_count - 2; ++j) {
            i64 this_time = records[i]->ElapsedAt(j);
            total_cost += this_time;

            NanoSeconds duration(this_time);
            ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(duration)));
            phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors[j + 1]);
        }

        // Output total query duration
        NanoSeconds total_duration(total_cost);
        ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(total_duration)));
        phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors.back());

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

/**
 * @brief Execute Show table details statement (i.e. show t1)
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowColumns(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<TableInfo> table_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(table_info, status) = txn->GetTableInfo(db_name_, *object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
        varchar_type,
    };
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    SizeT column_count = table_info->column_count_;
    for (SizeT input_column_id = 0; input_column_id < column_count; ++input_column_id) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        const ColumnDef *column = table_info->GetColumnDefByIdx(input_column_id);

        SizeT output_column_idx = 0;
        {
            // Append column name to the 1st column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column type to the 2nd column, if the column type is embedded type, append the embedded type
            String column_type = column->type()->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column default value to the 3rd column
            String column_default = column->default_expr_->ToString();
            Value value = Value::MakeVarchar(column_default);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column comment to the 4th column
            Value value = Value::MakeVarchar(column->comment());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowSegments(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    Vector<SharedPtr<SegmentInfo>> segment_info_list;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(segment_info_list, status) = txn->GetSegmentsInfo(db_name_, *object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
        varchar_type,
    };
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (auto &segment_info : segment_info_list) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        SizeT column_id = 0;
        {
            Value value = Value::MakeBigInt(segment_info->segment_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            //            SegmentEntry::SegmentStatusToString(segment_info->status_)
            Value value = Value::MakeVarchar("No value");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String segment_size_str = "TODO";
            //            String full_segment_dir = fmt::format("{}/seg_{}")
            //            String full_segment_dir = Path(InfinityContext::instance().config()->DataDir()) / *segment_info->segment_dir_;
            //            if (query_context->persistence_manager() == nullptr) {
            //                segment_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(full_segment_dir));
            //            } else {
            //                const Vector<String> &paths = segment_info->files_;
            //                SizeT segment_size = 0;
            //                for (const String &path : paths) {
            //                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
            //                    if (!status.ok()) {
            //                        RecoverableError(status);
            //                    }
            //                    segment_size += file_size;
            //                }
            //                segment_size_str = Utility::FormatByteSize(segment_size);
            //            }
            Value value = Value::MakeVarchar(segment_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowSegmentDetail(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<SegmentInfo> segment_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(segment_info, status) = txn->GetSegmentInfo(db_name_, *object_name_, *segment_id_);
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};
    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->segment_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("status");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // SegmentEntry::SegmentStatusToString(segment_info->status_)
            Value value = Value::MakeVarchar("No value");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            //            Value value = Value::MakeVarchar(*segment_info->segment_dir_);
            Value value = Value::MakeVarchar("TODO");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String segment_size_str;
            segment_size_str = Utility::FormatByteSize(segment_info->storage_size_);
            Value value = Value::MakeVarchar(segment_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("block_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->block_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->row_capacity_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->row_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("actual_row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->actual_row_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("room");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->row_capacity_ - segment_info->row_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_info->column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowBlocks(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    Vector<SharedPtr<BlockInfo>> block_info_array;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(block_info_array, status) = txn->GetBlocksInfo(db_name_, *object_name_, *segment_id_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
        bigint_type,
    };
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (const auto &block_info : block_info_array) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        SizeT column_id = 0;
        {
            Value value = Value::MakeBigInt(block_info->block_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
        ++column_id;
        {
            String block_size_str;
            if (query_context->persistence_manager() == nullptr) {
                String full_block_dir = Path(InfinityContext::instance().config()->DataDir()) / *block_info->block_dir_;
                block_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(full_block_dir));
            } else {
                Vector<String> &paths = block_info->files_;
                SizeT block_size = 0;
                for (const String &path : paths) {
                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
                    if (!status.ok()) {
                        RecoverableError(status);
                    }
                    block_size += file_size;
                }
                block_size_str = Utility::FormatByteSize(block_size);
            }
            Value value = Value::MakeVarchar(block_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(block_info->row_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowBlockDetail(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<BlockInfo> block_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(block_info, status) = txn->GetBlockInfo(db_name_, *object_name_, *segment_id_, *block_id_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type};
    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(block_info->block_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String full_block_dir = Path(InfinityContext::instance().config()->DataDir()) / *block_info->block_dir_;
            Value value = Value::MakeVarchar(full_block_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            SizeT block_storage_size = block_info->storage_size_;
            String block_storage_size_str = Utility::FormatByteSize(block_storage_size);
            Value value = Value::MakeVarchar(block_storage_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            SizeT row_capacity = block_info->row_capacity_;
            Value value = Value::MakeVarchar(std::to_string(row_capacity));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            SizeT row_count = block_info->row_count_;
            Value value = Value::MakeVarchar(std::to_string(row_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("checkpoint_row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            SizeT checkpoint_row_count = block_info->checkpoint_row_count_;
            Value value = Value::MakeVarchar(std::to_string(checkpoint_row_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            SizeT column_count = block_info->column_count_;
            Value value = Value::MakeVarchar(std::to_string(column_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("checkpoint_ts");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            SizeT checkpoint_ts = block_info->checkpoint_ts_;
            Value value = Value::MakeVarchar(std::to_string(checkpoint_ts));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowBlockColumn(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<BlockColumnInfo> block_column_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(block_column_info, status) = txn->GetBlockColumnInfo(db_name_, *object_name_, *segment_id_, *block_id_, *column_id_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };
    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("column_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(block_column_info->column_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("data_type");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(block_column_info->data_type_->ToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar("TODO");
            //            Value value = Value::MakeVarchar(*block_column_info->filename_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("extra_file_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(block_column_info->extra_file_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String storage_size_str = Utility::FormatByteSize(block_column_info->storage_size_);
            Value value = Value::MakeVarchar(storage_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("extra_file_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String outline_storage;
            for (SizeT idx = 0; idx < block_column_info->extra_file_count_; ++idx) {
                outline_storage += *block_column_info->extra_file_names_[idx];
                outline_storage += ";";
            }

            Value value = Value::MakeVarchar(outline_storage);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

// Execute show system table
void PhysicalShow::ExecuteShowConfigs(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Config *global_config = query_context->global_config();

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    // Config
    {
        {
            Value value = Value::MakeVarchar(VERSION_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // option value
            Value value = Value::MakeVarchar(global_config->Version());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option description
            Value value = Value::MakeVarchar("Infinity version.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        { // option name
            Value value = Value::MakeVarchar(TIME_ZONE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            i64 time_zone_bias = global_config->TimeZoneBias();
            if (time_zone_bias >= 0) {
                Value value = Value::MakeVarchar(fmt::format("{}+{}", global_config->TimeZone(), time_zone_bias));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            } else {
                Value value = Value::MakeVarchar(fmt::format("{}{}", global_config->TimeZone(), time_zone_bias));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Time zone information.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CPU_LIMIT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CPULimit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("CPU number used by infinity executor.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(RECORD_RUNNING_QUERY_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = global_config->RecordRunningQuery() ? Value::MakeVarchar("true") : Value::MakeVarchar("false");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("To record running query");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SERVER_ADDRESS_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->ServerAddress());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity server ip");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_SERVER_IP_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->PeerServerIP());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity peer server ip");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_SERVER_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerServerPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity peer server port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(POSTGRES_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PostgresPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Postgres port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(HTTP_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->HTTPPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("HTTP port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CLIENT_PORT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->ClientPort()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Thrift RPC port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CONNECTION_POOL_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->ConnectionPoolSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Connection pool capacity.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_SERVER_CONNECTION_POOL_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerServerConnectionPoolSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Connection pool capacity.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILENAME_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->LogFileName());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log file name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->LogDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_TO_STDOUT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = global_config->LogToStdout() ? Value::MakeVarchar("true") : Value::MakeVarchar("false");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("If log is also output to standard output");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILE_MAX_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileMaxSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Max log file size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_FILE_ROTATE_COUNT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->LogFileRotateCount()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log files rotation limitation");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(LOG_LEVEL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(LogLevel2Str(global_config->GetLogLevel()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log level");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    if (query_context->persistence_manager() != nullptr) {
        {
            {
                // option name
                Value value = Value::MakeVarchar(PERSISTENCE_DIR_OPTION_NAME);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar(global_config->PersistenceDir());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar("Virtual filesystem directory");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
            }
        }

        {
            {
                // option name
                Value value = Value::MakeVarchar(PERSISTENCE_OBJECT_SIZE_LIMIT_OPTION_NAME);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar(std::to_string(global_config->PersistenceObjectSizeLimit()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar("Virtual file limitation");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
            }
        }
    } else {
        {
            {
                // option name
                Value value = Value::MakeVarchar(DATA_DIR_OPTION_NAME);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar(global_config->DataDir());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }
            {
                // option name type
                Value value = Value::MakeVarchar("Data directory");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
            }
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CATALOG_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->CatalogDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Catalog directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->SnapshotDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Snapshot directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CLEANUP_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CleanupInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Cleanup period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(COMPACT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CompactInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Compact period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(OPTIMIZE_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->OptimizeIndexInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Optimize memory index period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(MEM_INDEX_CAPACITY_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->MemIndexCapacity()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Real-time index building row capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SNAPSHOT_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->SnapshotDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Snapshot storage directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(DENSE_INDEX_BUILDING_WORKER_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->DenseIndexBuildingWorker()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Dense vector index building worker count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(SPARSE_INDEX_BUILDING_WORKER_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->SparseIndexBuildingWorker()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Sparse vector index building worker count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(FULLTEXT_INDEX_BUILDING_WORKER_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->FulltextIndexBuildingWorker()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Full-text index building worker count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(BUFFER_MANAGER_SIZE_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->BufferManagerSize()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Buffer manager memory size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(TEMP_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->TempDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Temporary data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->WALDir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_COMPACT_THRESHOLD_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->WALCompactThreshold()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log compact triggering threshold");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(CHECKPOINT_INTERVAL_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->CheckpointInterval()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Checkpoint period interval");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(WAL_FLUSH_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(FlushOptionTypeToString(global_config->FlushMethodAtCommit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write ahead log flush method");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(RESOURCE_DIR_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->ResourcePath());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity resource directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_RETRY_DELAY_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerRetryDelay()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Peer retry delay");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_RETRY_COUNT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerRetryCount()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Peer retry count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }
    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_CONNECT_TIMEOUT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerConnectTimeout()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Peer connect timeout");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }
    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_RECV_TIMEOUT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerRecvTimeout()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Peer receive timeout");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }
    {
        {
            // option name
            Value value = Value::MakeVarchar(PEER_SEND_TIMEOUT_OPTION_NAME);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->PeerSendTimeout()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Peer send timeout");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    Vector<SharedPtr<TableIndexInfo>> table_index_info_list;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    Vector<String> index_names;
    status = txn->ListIndex(db_name_, *object_name_, index_names);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
    }
    for (const auto &index_name : index_names) {
        auto [table_index_info, status] = txn->GetTableIndexInfo(db_name_, *object_name_, index_name);
        if (!status.ok()) {
            show_operator_state->status_ = status.clone();
            RecoverableError(status);
        }
        table_index_info_list.push_back(table_index_info);
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type};
    SizeT row_count = 0;
    output_block_ptr->Init(*output_types_);

    {
        for (const auto &table_index_info : table_index_info_list) {
            if (output_block_ptr.get() == nullptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            SizeT column_id = 0;
            {
                // Append index name to the first column
                Value value = Value::MakeVarchar(*table_index_info->index_name_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index_comment to output
                String comment = *table_index_info->index_comment_;
                Value value = Value::MakeVarchar(comment);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index method type to the second column
                Value value = Value::MakeVarchar(*table_index_info->index_type_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index column id
                Value value = Value::MakeVarchar(*table_index_info->index_column_ids_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index column names to the third column
                Value value = Value::MakeVarchar(*table_index_info->index_column_names_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index path
                Value value = Value::MakeVarchar(*table_index_info->index_entry_dir_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append Index segment
                SizeT segment_index_count = table_index_info->segment_index_count_;
                String result_value = fmt::format("{}", segment_index_count);
                Value value = Value::MakeVarchar(result_value);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index other parameters to the fourth column
                Value value = Value::MakeVarchar(*table_index_info->index_other_params_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }

            if (++row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                show_operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}

void PhysicalShow::ExecuteShowViewDetail(QueryContext *query_context,
                                         const SharedPtr<Vector<SharedPtr<DataType>>> &view_column_types,
                                         const SharedPtr<Vector<String>> &view_column_names) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", std::set<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", std::set<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Views"), nullptr, output_column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    SizeT column_count = view_column_types->size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(view_column_names->at(idx));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column type to the second column
            String column_type = view_column_types->at(idx)->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowSessionVariable(QueryContext *query_context, ShowOperatorState *operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> integer_type = MakeShared<DataType>(LogicalType::kBigInt);
    SharedPtr<DataType> bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    SessionVariable session_var = VarUtil::GetSessionVarByName(*object_name_);
    BaseSession *session_ptr = query_context->current_session();
    switch (session_var) {
        case SessionVariable::kQueryCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->query_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kTotalCommitCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->committed_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kTotalRollbackCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->rollbacked_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SessionVariable::kConnectedTime: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeVarchar(session_ptr->ConnectedTimeToStr());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            operator_state->status_ = Status::NoSysVar(*object_name_);
            RecoverableError(operator_state->status_);
            return;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowSessionVariables(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    BaseSession *session_ptr = query_context->current_session();
    for (auto &session_var_pair : VarUtil::session_name_map_) {
        const String &var_name = session_var_pair.first;
        SessionVariable session_var = session_var_pair.second;

        switch (session_var) {
            case SessionVariable::kQueryCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Accomplished query count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kTotalCommitCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Committed count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kTotalRollbackCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Rollbacked transaction count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case SessionVariable::kConnectedTime: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(session_ptr->ConnectedTimeToStr());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Connected timestamp of this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            default: {
                operator_state->status_ = Status::NoSysVar(var_name);
                RecoverableError(operator_state->status_);
                return;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowGlobalVariable(QueryContext *query_context, ShowOperatorState *operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> integer_type = MakeShared<DataType>(LogicalType::kBigInt);
    SharedPtr<DataType> double_type = MakeShared<DataType>(LogicalType::kDouble);
    SharedPtr<DataType> bool_type = MakeShared<DataType>(LogicalType::kBoolean);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    GlobalVariable global_var = VarUtil::GetGlobalVarByName(*object_name_);
    Config *config = query_context->global_config();

    switch (global_var) {
        case GlobalVariable::kResultCache: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeVarchar(config->ResultCache());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kCacheResultCapacity: {
            const String &result_cache_status = config->ResultCache();
            if (result_cache_status == "off") {
                operator_state->status_ = Status::NotSupport(fmt::format("Result cache is off"));
                RecoverableError(operator_state->status_);
            }
            ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();

            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(cache_mgr->cache_num_capacity());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kCacheResultNum: {
            const String &result_cache_status = config->ResultCache();
            if (result_cache_status == "off") {
                operator_state->status_ = Status::NotSupport(fmt::format("Result cache is off"));
                RecoverableError(operator_state->status_);
            }
            ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();

            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(cache_mgr->cache_num_used());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kMemoryCacheMiss: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 total_request_count = buffer_manager->TotalRequestCount();
            u64 cache_miss_count = buffer_manager->CacheMissCount();

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeVarchar(fmt::format("{}/{}", cache_miss_count, total_request_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kDiskCacheMiss: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeVarchar(fmt::format("{}/{}", VirtualStore::CacheMissCount(), VirtualStore::TotalRequestCount()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kQueryCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(query_context->session_manager()->total_query_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kSessionCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            SessionManager *session_manager = query_context->session_manager();
            u64 session_count = session_manager->GetSessionCount();
            Value value = Value::MakeBigInt(session_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kBufferPoolUsage: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 memory_limit = buffer_manager->memory_limit();
            u64 memory_usage = buffer_manager->memory_usage();
            Value value = Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kSchedulePolicy: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeVarchar("round robin");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kNextTxnID: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            auto *new_txn_mgr = query_context->storage()->new_txn_manager();
            Value value = Value::MakeBigInt(new_txn_mgr->current_transaction_id());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kBufferedObjectCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            Value value = Value::MakeBigInt(buffer_manager->BufferedObjectCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kUnusedBufferObjectCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            Vector<SizeT> size_list = buffer_manager->WaitingGCObjectCount();
            SizeT total_size = std::accumulate(size_list.begin(), size_list.end(), 0);
            Value value = Value::MakeBigInt(total_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kActiveTxnCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            SizeT active_txn_count = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            active_txn_count = new_txn_mgr->ActiveTxnCount();

            Value value = Value::MakeBigInt(active_txn_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kCurrentTs: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            TxnTimeStamp current_ts = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            current_ts = new_txn_mgr->CurrentTS();

            Value value = Value::MakeBigInt(current_ts);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kTotalCommitCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            u64 total_committed_txn_count = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            total_committed_txn_count = new_txn_mgr->total_committed_txn_count();

            Value value = Value::MakeBigInt(total_committed_txn_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kTotalRollbackCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            u64 total_rollbacked_txn_count = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            total_rollbacked_txn_count = new_txn_mgr->total_rollbacked_txn_count();

            Value value = Value::MakeBigInt(total_rollbacked_txn_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kActiveWALFilename: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            WalManager *wal_manager = query_context->storage()->wal_manager();
            Value value = Value::MakeVarchar(wal_manager->GetWalFilename());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kProfileRecordCapacity: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            auto *catalog_ptr = query_context->storage()->new_catalog();
            Value value = Value::MakeBigInt(catalog_ptr->ProfileHistorySize());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kBackgroundTaskCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
            SizeT running_task_count = bg_processor->RunningTaskCount();
            Value value = Value::MakeBigInt(running_task_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kRunningBGTask: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
            Value value = Value::MakeVarchar(bg_processor->RunningTaskText());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kRunningCompactTask: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            CompactionProcessor *compaction_processor = query_context->storage()->compaction_processor();
            Value value = Value::MakeBigInt(compaction_processor->RunningTaskCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kSystemMemoryUsage: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            i64 memory_usage = SystemInfo::MemoryUsage();
            Value value = Value::MakeBigInt(memory_usage);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kOpenFileCount: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            i64 open_file_count = SystemInfo::OpenFileCount();
            Value value = Value::MakeBigInt(open_file_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kCPUUsage: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, double_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                double_type,
            };

            output_block_ptr->Init(output_column_types);

            f64 cpu_usage = SystemInfo::CPUUsage();
            Value value = Value::MakeDouble(cpu_usage * 100);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kJeProf: {

            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);
#ifdef ENABLE_JEMALLOC_PROF
            // option value
            Value value = Value::MakeVarchar("off");
#else
            Value value = Value::MakeVarchar("on");
#endif
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case GlobalVariable::kFollowerNum: {
            SharedPtr<NodeInfo> this_node = InfinityContext::instance().cluster_manager()->ThisNode();
            if (this_node->node_role() == NodeRole::kLeader) {
                Vector<SharedPtr<ColumnDef>> output_column_defs = {
                    MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
                };

                SharedPtr<TableDef> table_def =
                    TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
                output_ = MakeShared<DataTable>(table_def, TableType::kResult);

                Vector<SharedPtr<DataType>> output_column_types{
                    integer_type,
                };

                output_block_ptr->Init(output_column_types);

                i64 follower_number = InfinityContext::instance().cluster_manager()->GetFollowerLimit();
                Value value = Value::MakeBigInt(follower_number);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            } else {
                operator_state->status_ = Status::NotSupport(fmt::format("follower_number isn't supported in non-leader node of cluster deployment"));
                RecoverableError(operator_state->status_);
            }
            break;
        }
        case GlobalVariable::kEnableProfile: {
            Vector<SharedPtr<ColumnDef>> output_column_defs = {
                MakeShared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            SharedPtr<TableDef> table_def =
                TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("variables"), nullptr, output_column_defs);
            output_ = MakeShared<DataTable>(table_def, TableType::kResult);

            Vector<SharedPtr<DataType>> output_column_types{
                bool_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBool(InfinityContext::instance().storage()->new_catalog()->GetProfile());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            operator_state->status_ = Status::NoSysVar(*object_name_);
            RecoverableError(operator_state->status_);
            return;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowGlobalVariables(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    for (auto &global_var_pair : VarUtil::global_name_map_) {
        const String &var_name = global_var_pair.first;
        GlobalVariable global_var_enum = global_var_pair.second;
        Config *config = query_context->global_config();
        switch (global_var_enum) {
            case GlobalVariable::kResultCache: {
                const String &result_cache_status = config->ResultCache();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(result_cache_status);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Result cache num");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kCacheResultCapacity: {
                const String &result_cache_status = config->ResultCache();
                if (result_cache_status == "off") {
                    break;
                }
                ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();
                SizeT cache_num_capacity = cache_mgr->cache_num_capacity();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(cache_num_capacity));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Result cache capacity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kCacheResultNum: {
                const String &result_cache_status = config->ResultCache();
                if (result_cache_status == "off") {
                    break;
                }
                ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();
                SizeT cache_num_used = cache_mgr->cache_num_used();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(cache_num_used));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Result cache num");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kMemoryCacheMiss: {
                BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                u64 total_request_count = buffer_manager->TotalRequestCount();
                u64 cache_miss_count = buffer_manager->CacheMissCount();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(fmt::format("{}/{}", cache_miss_count, total_request_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Memory cache miss");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kDiskCacheMiss: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(fmt::format("{}/{}", VirtualStore::CacheMissCount(), VirtualStore::TotalRequestCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Disk cache miss");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kQueryCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(query_context->session_manager()->total_query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Query count in total");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kSessionCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    SessionManager *session_manager = query_context->session_manager();
                    u64 session_count = session_manager->GetSessionCount();
                    Value value = Value::MakeVarchar(std::to_string(session_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Session count in total");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kBufferPoolUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    u64 memory_limit = buffer_manager->memory_limit();
                    u64 memory_usage = buffer_manager->memory_usage();
                    Value value =
                        Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Buffer manager usage");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kSchedulePolicy: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar("round robin");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Task scheduling policy");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kNextTxnID: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    auto *new_txn_mgr = query_context->storage()->new_txn_manager();
                    Value value = Value::MakeVarchar(std::to_string(new_txn_mgr->current_transaction_id()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Next transaction id of system");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kBufferedObjectCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    Value value = Value::MakeVarchar(std::to_string(buffer_manager->BufferedObjectCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Buffered object count in buffer manager");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kUnusedBufferObjectCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    Vector<SizeT> size_list = buffer_manager->WaitingGCObjectCount();
                    SizeT total_size = std::accumulate(size_list.begin(), size_list.end(), 0);
                    Value value = Value::MakeVarchar(std::to_string(total_size));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Unused object in buffer manager waiting for garbage collection");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kActiveTxnCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    SizeT active_txn_count = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    active_txn_count = new_txn_mgr->ActiveTxnCount();

                    Value value = Value::MakeVarchar(std::to_string(active_txn_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Active transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kCurrentTs: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    TxnTimeStamp current_ts = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    current_ts = new_txn_mgr->CurrentTS();

                    Value value = Value::MakeVarchar(std::to_string(current_ts));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current timestamp");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kTotalCommitCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    u64 total_committed_txn_count = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    total_committed_txn_count = new_txn_mgr->total_committed_txn_count();

                    Value value = Value::MakeVarchar(std::to_string(total_committed_txn_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Global committed transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kTotalRollbackCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    u64 total_rollbacked_txn_count = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    total_rollbacked_txn_count = new_txn_mgr->total_rollbacked_txn_count();

                    Value value = Value::MakeVarchar(std::to_string(total_rollbacked_txn_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Global rolled back transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kActiveWALFilename: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    WalManager *wal_manager = query_context->storage()->wal_manager();
                    Value value = Value::MakeVarchar(wal_manager->GetWalFilename());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Write ahead log filename");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kProfileRecordCapacity: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    auto *catalog_ptr = query_context->storage()->new_catalog();
                    Value value = Value::MakeVarchar(std::to_string(catalog_ptr->ProfileHistorySize()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Profile record history capacity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kBackgroundTaskCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
                    SizeT running_task_count = bg_processor->RunningTaskCount();
                    Value value = Value::MakeVarchar(std::to_string(running_task_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Background tasks count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kRunningBGTask: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
                    Value value = Value::MakeVarchar(bg_processor->RunningTaskText());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current running background task");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kRunningCompactTask: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    String task_count = "N/A";
                    if (query_context->storage()->GetStorageMode() == StorageMode::kWritable) {
                        CompactionProcessor *compaction_processor = query_context->storage()->compaction_processor();
                        task_count = std::to_string(compaction_processor->RunningTaskCount());
                    }
                    Value value = Value::MakeVarchar(task_count);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current running background task");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kSystemMemoryUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    i64 memory_usage = SystemInfo::MemoryUsage();
                    Value value = Value::MakeVarchar(std::to_string(memory_usage));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Infinity system memory usage.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kOpenFileCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    i64 open_file_count = SystemInfo::OpenFileCount();
                    Value value = Value::MakeVarchar(std::to_string(open_file_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("File description opened count by Infinity.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kCPUUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    f64 cpu_usage = SystemInfo::CPUUsage();
                    Value value = Value::MakeVarchar(fmt::format("{:.{}f}", cpu_usage * 100, 2));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Infinity system CPU usage.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kJeProf: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
#ifdef ENABLE_JEMALLOC_PROF
                    // option value
                    Value value = Value::MakeVarchar("off");
#else
                    Value value = Value::MakeVarchar("on");
#endif
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Use jemalloc to profile Infinity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            case GlobalVariable::kFollowerNum: {
                if (InfinityContext::instance().IsClusterRole()) {
                    SharedPtr<NodeInfo> this_node = InfinityContext::instance().cluster_manager()->ThisNode();
                    if (this_node->node_role() == NodeRole::kLeader) {
                        {
                            // option name
                            Value value = Value::MakeVarchar(var_name);
                            ValueExpression value_expr(value);
                            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                        }
                        {
                            // option value
                            SizeT follower_count = InfinityContext::instance().cluster_manager()->GetFollowerLimit();
                            Value value = Value::MakeVarchar(std::to_string(follower_count));
                            ValueExpression value_expr(value);
                            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                        }
                        {
                            // option description
                            Value value = Value::MakeVarchar("Follower number for Infinity cluster");
                            ValueExpression value_expr(value);
                            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                        }
                    }
                }
                break;
            }
            case GlobalVariable::kEnableProfile: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
                }
                {
                    // option value
                    bool enable_profile = query_context->storage()->new_catalog()->GetProfile();
                    String enable_profile_condition = enable_profile ? "true" : "false";
                    Value value = Value::MakeVarchar(enable_profile_condition);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Enable profile");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
                }
                break;
            }
            default: {
                operator_state->status_ = Status::NoSysVar(var_name);
                RecoverableError(operator_state->status_);
                return;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowConfig(QueryContext *query_context, ShowOperatorState *operator_state) {
    Config *global_config = query_context->global_config();
    auto [base_option, status] = global_config->GetConfigByName(*object_name_);
    if (!status.ok()) {
        operator_state->status_ = status;
        return;
    }
    if (*object_name_ == "time_zone_bias") {
        operator_state->status_ = Status::InvalidConfig(fmt::format("Option: {} doesn't exist.", *object_name_));
        return;
    }

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    auto double_type = MakeShared<DataType>(LogicalType::kDouble);
    auto bool_type = MakeShared<DataType>(LogicalType::kBoolean);

    switch (base_option->data_type_) {
        case BaseOptionDataType::kInteger: {
            Vector<SharedPtr<DataType>> output_column_types{
                bigint_type,
            };
            output_block_ptr->Init(output_column_types);

            IntegerOption *integer_option = static_cast<IntegerOption *>(base_option);
            Value value = Value::MakeBigInt(integer_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kFloat: {
            Vector<SharedPtr<DataType>> output_column_types{
                double_type,
            };
            output_block_ptr->Init(output_column_types);

            FloatOption *float_option = static_cast<FloatOption *>(base_option);
            Value value = Value::MakeDouble(float_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kString: {
            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            StringOption *string_option = static_cast<StringOption *>(base_option);
            String value_str;
            if (*object_name_ == "time_zone") {
                auto [time_zone_bias, _] = global_config->GetConfigByName("time_zone_bias");

                IntegerOption *time_zone_bias_int = static_cast<IntegerOption *>(time_zone_bias);
                if (time_zone_bias_int->value_ >= 0) {
                    value_str = fmt::format("{}+{}", string_option->value_, time_zone_bias_int->value_);
                } else {
                    value_str = fmt::format("{}-{}", string_option->value_, time_zone_bias_int->value_);
                }
            } else {
                value_str = string_option->value_;
            }
            Value value = Value::MakeVarchar(value_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kBoolean: {
            Vector<SharedPtr<DataType>> output_column_types{
                bool_type,
            };
            output_block_ptr->Init(output_column_types);

            BooleanOption *boolean_option = static_cast<BooleanOption *>(base_option);
            Value value = Value::MakeBool(boolean_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kLogLevel: {
            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            LogLevelOption *loglevel_option = static_cast<LogLevelOption *>(base_option);
            Value value = Value::MakeVarchar(LogLevel2Str(loglevel_option->value_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case BaseOptionDataType::kFlush: {
            Vector<SharedPtr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            FlushOption *flush_option = static_cast<FlushOption *>(base_option);
            Value value = Value::MakeVarchar(FlushOptionTypeToString(flush_option->value_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            String error_message = "Invalid option data type.";
            UnrecoverableError(error_message);
            break;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowBuffer(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        bigint_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    Vector<BufferObjectInfo> buffer_object_info_array = buffer_manager->GetBufferObjectsInfo();
    for (const auto &buffer_object_info : buffer_object_info_array) {

        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // path
            Value value = Value::MakeVarchar(buffer_object_info.object_path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // status
            Value value = Value::MakeVarchar(BufferStatusToString(buffer_object_info.buffered_status_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // size
            i64 buffer_object_size = static_cast<i64>(buffer_object_info.object_size_);
            Value value = Value::MakeBigInt(buffer_object_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
        {
            // buffered type
            Value value = Value::MakeVarchar(BufferTypeToString(buffer_object_info.buffered_type_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }
        {
            // type
            Value value = Value::MakeVarchar(FileWorkerType2Str(buffer_object_info.file_type_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowMemIndex(QueryContext *query_context, ShowOperatorState *operator_state) {

    Status status = Status::NotSupport("Show memindex is not supported in new catalog since BGMemIndexTracer has not yet been ported.");
    RecoverableError(status);

    //    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    //
    //    Vector<SharedPtr<DataType>> column_types{
    //        varchar_type,
    //        varchar_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //    };
    //    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(*output_types_);
    //    SizeT row_count = 0;

    //
    //    BGMemIndexTracer *mem_index_tracer = query_context->storage()->memindex_tracer();
    //    Txn *txn = query_context->GetTxn();
    //    Vector<MemIndexTracerInfo> mem_index_tracer_info_array = mem_index_tracer->GetMemIndexTracerInfo(txn);
    //    for (const auto &memindex_tracer_info : mem_index_tracer_info_array) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(*output_types_);
    //        }
    //
    //        {
    //            // index_name
    //            Value value = Value::MakeVarchar(*memindex_tracer_info.index_name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // table_name
    //            Value value = Value::MakeVarchar(*memindex_tracer_info.table_name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // db_name
    //            Value value = Value::MakeVarchar(*memindex_tracer_info.db_name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //        {
    //            // size
    //            Value value = Value::MakeBigInt(memindex_tracer_info.mem_used_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //        {
    //            // row_count
    //            Value value = Value::MakeBigInt(memindex_tracer_info.row_count_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        ++row_count;
    //        if (row_count == output_block_ptr->capacity()) {
    //            output_block_ptr->Finalize();
    //            operator_state->output_.emplace_back(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //            row_count = 0;
    //        }
    //    }
    //    output_block_ptr->Finalize();
    //    operator_state->output_.emplace_back(std::move(output_block_ptr));
    //    return;
}

void PhysicalShow::ExecuteShowQueries(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        bigint_type,
        varchar_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    SessionManager *session_manager = query_context->session_manager();
    Map<u64, SharedPtr<QueryInfo>> query_map = session_manager->QueryRecords();
    for (auto &query_pair : query_map) {
        u64 session_id = query_pair.first;
        QueryInfo &query_info = *query_pair.second;

        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // session_id
            Value value = Value::MakeBigInt(session_id);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // query_id
            Value value = Value::MakeBigInt(query_info.query_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // query_kind
            Value value = Value::MakeVarchar(query_info.query_kind_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
        {
            // start_time
            Value value = Value::MakeVarchar(query_info.profiler_.BeginTime());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }
        {
            // time_consumption
            Value value = Value::MakeVarchar(query_info.profiler_.ElapsedToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowQuery(QueryContext *query_context, ShowOperatorState *operator_state) {

    QueryInfo *query_info_ptr = query_context->session_manager()->GetQueryRecord(*session_id_);
    if (query_info_ptr == nullptr) {
        Status status = Status::SessionNotFound(*session_id_);
        RecoverableError(status);
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("session_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(*session_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("query_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(query_info_ptr->query_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("start_time");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(query_info_ptr->profiler_.BeginTime());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("time_consumption");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(query_info_ptr->profiler_.ElapsedToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("query_text");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(query_info_ptr->query_text_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowTransactions(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    Vector<TxnInfo> txn_info_array;
    NewTxnManager *new_txn_manager = query_context->storage()->new_txn_manager();
    txn_info_array = new_txn_manager->GetTxnInfoArray();

    for (const auto &txn_info : txn_info_array) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // txn_id
            Value value = Value::MakeBigInt(txn_info.txn_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // txn_text
            String txn_string;
            if (txn_info.txn_text_.get() != nullptr) {
                txn_string = *txn_info.txn_text_;
            }
            Value value = Value::MakeVarchar(txn_string);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowTransaction(QueryContext *query_context, ShowOperatorState *operator_state) {
    UniquePtr<TxnInfo> txn_info = nullptr;
    NewTxnManager *new_txn_manager = query_context->storage()->new_txn_manager();
    txn_info = new_txn_manager->GetTxnInfoByID(*txn_id_);

    if (txn_info.get() == nullptr) {
        Status status = Status::TransactionNotFound(*txn_id_);
        RecoverableError(status);
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("transaction_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(*txn_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("session_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(*session_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("transaction_text");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String txn_string;
            if (txn_info->txn_text_.get() != nullptr) {
                txn_string = *txn_info->txn_text_;
            }
            Value value = Value::MakeVarchar(txn_string);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowTransactionHistory(QueryContext *query_context, ShowOperatorState *operator_state) {
    Vector<SharedPtr<TxnContext>> txn_context_histories;
    TransactionID this_txn_id;
    NewTxnManager *new_txn_manager = query_context->storage()->new_txn_manager();
    txn_context_histories = new_txn_manager->GetTxnContextHistories();
    NewTxn *txn = query_context->GetNewTxn();
    this_txn_id = txn->TxnID();

    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        bigint_type,
        bigint_type,
        varchar_type,
        varchar_type,
        varchar_type,
    };
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    for (const auto &txn_context : txn_context_histories) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // txn id
            String txn_id_str;
            if (this_txn_id == txn_context->txn_id_) {
                txn_id_str = fmt::format("{}(this txn)", this_txn_id);
            } else {
                txn_id_str = fmt::format("{}", txn_context->txn_id_);
            }
            Value value = Value::MakeVarchar(txn_id_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // txn id
            String txn_text;
            if (txn_context->text_.get() != nullptr) {
                txn_text = *txn_context->text_;
            }
            Value value = Value::MakeVarchar(txn_text);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        {
            // txn begin_ts
            Value value = Value::MakeBigInt(txn_context->begin_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }

        {
            // txn commit_ts
            Value value = Value::MakeBigInt(txn_context->commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }

        {
            // txn state
            Value value = Value::MakeVarchar(TxnState2Str(txn_context->state_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        {
            // txn type
            String transaction_type_str = "read";
            if (txn_context->is_write_transaction_) {
                transaction_type_str = "write";
            }
            Value value = Value::MakeVarchar(transaction_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[5]);
        }

        {
            // txn operations
            std::stringstream ss;
            Vector<SharedPtr<String>> operations;
            for (const auto &ops : txn_context->operations_) {
                ss << *ops << std::endl;
            }
            Value value = Value::MakeVarchar(ss.str());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[6]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowLogs(QueryContext *query_context, ShowOperatorState *operator_state) {

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        bigint_type,
        varchar_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    WalManager *wal_manager = query_context->storage()->wal_manager();
    Vector<SharedPtr<WalEntry>> wal_entries = wal_manager->CollectWalEntries();
    LOG_TRACE(fmt::format("WAL Entries count: {}", wal_entries.size()));

    for (const auto &wal_entry_ref : wal_entries) {
        for (const auto &cmd_ref : wal_entry_ref->cmds_) {
            if (output_block_ptr.get() == nullptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            {
                // transaction_id
                Value value = Value::MakeBigInt(wal_entry_ref->txn_id_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            }

            {
                // commit_ts
                Value value = Value::MakeBigInt(wal_entry_ref->commit_ts_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }

            {
                // command_type
                String command_type = WalCmd::WalCommandTypeToString(cmd_ref->GetType());
                Value value = Value::MakeVarchar(command_type);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
            }

            {
                // command_text
                String command_text = cmd_ref->ToString();
                Value value = Value::MakeVarchar(command_text);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
            }

            ++row_count;
            if (row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowCatalog(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    auto *new_catalog = query_context->storage()->new_catalog();
    auto meta_tree_ptr = new_catalog->MakeMetaTree();
    auto meta_json = meta_tree_ptr->ToJson();

    constexpr int json_intent = 4;
    auto meta_str = meta_json.dump(json_intent);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{varchar_type};

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    {
        Value value = Value::MakeVarchar(meta_str);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    }
    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowCatalogToFile(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    String status_message;
    if (std::filesystem::exists(*file_path_)) {
        status_message = fmt::format("'{}' already exists", *file_path_);
    } else {
        std::ofstream file;
        file.open(*file_path_);
        if (!file.is_open()) {
            status_message = fmt::format("Failed to open '{}' as file", *file_path_);
        } else {
            auto *new_catalog = query_context->storage()->new_catalog();
            auto meta_tree_ptr = new_catalog->MakeMetaTree();
            auto meta_json = meta_tree_ptr->ToJson();

            constexpr int json_intent = 4;
            auto meta_str = meta_json.dump(json_intent);
            file << meta_str;
            file.close();
            status_message = "OK";
        }
    }

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{varchar_type};

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    {
        Value value = Value::MakeVarchar(status_message);
        ValueExpression value_expr(value);
        value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    }
    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowPersistenceFiles(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        bigint_type,
        bigint_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    PersistenceManager *persistence_manager = query_context->persistence_manager();
    if (persistence_manager == nullptr) {
        Status status = Status::FunctionIsDisable("Persistence");
        RecoverableError(status);
    }
    HashMap<String, ObjAddr> file_map = persistence_manager->GetAllFiles();

    for (const auto &file_pair : file_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // file_name
            Value value = Value::MakeVarchar(file_pair.first);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // object_name
            Value value = Value::MakeVarchar(file_pair.second.obj_key_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // file offset
            Value value = Value::MakeBigInt(file_pair.second.part_offset_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
        {
            // file size
            Value value = Value::MakeBigInt(file_pair.second.part_size_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowPersistenceObjects(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        bigint_type,
        bigint_type,
        bigint_type,
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    PersistenceManager *persistence_manager = query_context->persistence_manager();
    if (persistence_manager == nullptr) {
        Status status = Status::FunctionIsDisable("Persistence");
        RecoverableError(status);
    }

    HashMap<String, ObjStat> object_map = persistence_manager->GetAllObjects();
    for (const auto &object_pair : object_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // name
            Value value = Value::MakeVarchar(object_pair.first);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // reference count
            Value value = Value::MakeBigInt(object_pair.second.ref_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // size
            Value value = Value::MakeBigInt(object_pair.second.obj_size_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
        {
            // parts
            Value value = Value::MakeBigInt(object_pair.second.parts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }
        {
            // deleted ranges
            OStringStream oss;
            for (const auto &range : object_pair.second.deleted_ranges_) {
                oss << fmt::format("[{}, {}) ", range.start_, range.end_);
            }
            String deleted_ranges = oss.str();
            Value value = Value::MakeVarchar(deleted_ranges);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowPersistenceObject(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        bigint_type,
        bigint_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    SizeT row_count = 0;

    PersistenceManager *persistence_manager = query_context->persistence_manager();
    if (persistence_manager == nullptr) {
        Status status = Status::FunctionIsDisable("Persistence");
        RecoverableError(status);
    }

    HashMap<String, ObjStat> object_map = persistence_manager->GetAllObjects();
    auto iter = object_map.find(*object_name_);
    if (iter == object_map.end()) {
        Status status = Status::FileNotFound(*object_name_);
        RecoverableError(status);
    }

    Set<Range> &deleted_ranges = iter->second.deleted_ranges_;
    for (auto &range : deleted_ranges) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }
        {
            // start
            Value value = Value::MakeBigInt(range.start_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // end
            Value value = Value::MakeBigInt(range.end_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowMemory(QueryContext *query_context, ShowOperatorState *operator_state) {

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("memory_objects");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(GlobalResourceUsage::GetObjectCountInfo());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("memory_allocation");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(GlobalResourceUsage::GetRawMemoryInfo());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowMemoryObjects(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        bigint_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    std::unordered_map<String, i64> object_map = GlobalResourceUsage::GetObjectClones();
    SizeT row_count = 0;

    for (auto &object_pair : object_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }
        if (object_pair.second != 0) {

            {
                // object_name
                Value value = Value::MakeVarchar(object_pair.first);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            }
            {
                // object_count
                Value value = Value::MakeBigInt(object_pair.second);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
            }

            ++row_count;
        }
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowMemoryAllocation(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        bigint_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    std::unordered_map<String, i64> raw_memory_map = GlobalResourceUsage::GetRawMemoryClone();
    SizeT row_count = 0;

    for (auto &raw_memory_pair : raw_memory_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // name
            Value value = Value::MakeVarchar(raw_memory_pair.first);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // total_memory
            Value value = Value::MakeBigInt(raw_memory_pair.second);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowFunction(QueryContext *query_context, ShowOperatorState *operator_state) {

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // create data block for output state
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    String function_name = *function_name_;
    ToLower(function_name);
    if (function_name == "server_version") {

        String version_info =
            fmt::format("{}.{}.{} {} {} {}", version_major(), version_minor(), version_patch(), system_build_time(), build_type(), git_commit_id());
        {
            // name
            Value value = Value::MakeVarchar(version_info);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        fmt::print("Release: {}.{}.{} build on {} with {} mode from branch: {}, commit-id: {}\n",
                   version_major(),
                   version_minor(),
                   version_patch(),
                   system_build_time(),
                   build_type(),
                   git_branch_name(),
                   git_commit_id());
    } else {
        Status status = Status::Unknown(fmt::format("function: {}", function_name));
        RecoverableError(status);
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteListSnapshots(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, varchar_type, bigint_type, varchar_type};

    output_block_ptr->Init(*output_types_);

    String snapshot_dir = query_context->global_config()->SnapshotDir();
    Vector<SnapshotBrief> snapshot_list = SnapshotBrief::GetSnapshots(snapshot_dir);

    SizeT row_count = 0;
    for (auto &snapshot_brief : snapshot_list) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // snapshot name
            Value value = Value::MakeVarchar(snapshot_brief.snapshot_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }

        {
            // scope
            String scope_str;
            switch (snapshot_brief.scope_) {
                case SnapshotScope::kTable: {
                    scope_str = "Table";
                    break;
                }
                case SnapshotScope::kDatabase: {
                    scope_str = "Database";
                    break;
                }
                case SnapshotScope::kSystem: {
                    scope_str = "System";
                    break;
                }
                case SnapshotScope::kIgnore: {
                    scope_str = "Ignore";
                    break;
                }
                default: {
                    Status status = Status::Unknown("Invalid scope type");
                    RecoverableError(status);
                }
            }
            Value value = Value::MakeVarchar(scope_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }

        {
            // snapshot create time
            Value value = Value::MakeVarchar(snapshot_brief.create_time_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }

        {
            // snapshot commit ts
            Value value = Value::MakeBigInt(snapshot_brief.commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
        }

        {
            // snapshot size
            String snapshot_size_str = Utility::FormatByteSize(snapshot_brief.size_);
            Value value = Value::MakeVarchar(snapshot_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

void PhysicalShow::ExecuteShowSnapshot(QueryContext *query_context, ShowOperatorState *operator_state) {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(*output_types_);

    String snapshot_dir = query_context->global_config()->SnapshotDir();
    Vector<SnapshotBrief> snapshot_list = SnapshotBrief::GetSnapshots(snapshot_dir);

    SnapshotBrief snapshot_brief;
    for (const auto &ss_brief : snapshot_list) {
        if (ss_brief.snapshot_name_ == object_name_.value()) {
            snapshot_brief = ss_brief;
        }
    }

    if (snapshot_brief.scope_ == SnapshotScope::kInvalid) {
        Status status = Status::Unknown(fmt::format("can't find snapshot: {}", object_name_.value()));
        RecoverableError(status);
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("snapshot_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(snapshot_brief.snapshot_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("snapshot_scope");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String scope_str;
            switch (snapshot_brief.scope_) {
                case SnapshotScope::kTable: {
                    scope_str = "Table";
                    break;
                }
                case SnapshotScope::kDatabase: {
                    scope_str = "Database";
                    break;
                }
                case SnapshotScope::kSystem: {
                    scope_str = "System";
                    break;
                }
                case SnapshotScope::kIgnore: {
                    scope_str = "Ignore";
                    break;
                }
                default: {
                    Status status = Status::Unknown("Invalid scope type");
                    RecoverableError(status);
                }
            }

            Value value = Value::MakeVarchar(scope_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("create_time");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(snapshot_brief.create_time_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("commit_timestamp");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(snapshot_brief.commit_ts_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar("snapshot_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            String snapshot_size_str = Utility::FormatByteSize(snapshot_brief.size_);
            Value value = Value::MakeVarchar(snapshot_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}

} // namespace infinity
