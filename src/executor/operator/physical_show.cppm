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

export module infinity_core:physical_show;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :logical_show;
import :load_meta;
import :infinity_exception;
import :variables;
import :data_block;
import :logger;

import show_statement;
import internal_types;
import column_def;
import data_type;

namespace infinity {

export class PhysicalShow : public PhysicalOperator {
public:
    explicit PhysicalShow(u64 id,
                          ShowStmtType type,
                          std::string db_name,
                          std::optional<std::string> object_name,
                          u64 table_index,
                          std::optional<std::string> file_path,
                          std::optional<SegmentID> segment_id,
                          std::optional<BlockID> block_id,
                          std::optional<ChunkID> chunk_id,
                          std::optional<ColumnID> column_id,
                          std::optional<std::string> index_name,
                          std::optional<u64> session_id,
                          std::optional<TransactionID> txn_id,
                          std::optional<std::string> function_name,
                          bool show_nullable,
                          std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kShow, nullptr, nullptr, id, load_metas), show_type_(type), db_name_(std::move(db_name)),
          object_name_(std::move(object_name)), table_index_(table_index), file_path_(std::move(file_path)), segment_id_(segment_id),
          block_id_(block_id), chunk_id_(chunk_id), column_id_(column_id), index_name_(index_name), session_id_(session_id), txn_id_(txn_id),
          function_name_(function_name), show_nullable_(show_nullable) {}

    ~PhysicalShow() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ShowStmtType show_type() const { return show_type_; }

    inline const std::string &db_name() const { return db_name_; };

    inline const std::optional<std::string> object_name() const { return object_name_; };

private:
    void ExecuteShowDatabase(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTable(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndex(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndexSegment(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndexChunks(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndexChunk(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTables(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTasks(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowColumns(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowSegments(QueryContext *query_context, ShowOperatorState *show_operator_state);

    void ExecuteShowSegmentDetail(QueryContext *query_context, ShowOperatorState *show_operator_state);

    void ExecuteShowBlocks(QueryContext *query_context, ShowOperatorState *show_operator_state);

    void ExecuteShowBlockDetail(QueryContext *query_context, ShowOperatorState *show_operator_state);

    void ExecuteShowBlockColumn(QueryContext *query_context, ShowOperatorState *show_operator_state);

    void ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowProfiles(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowConfigs(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowSessionVariable(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowSessionVariables(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowGlobalVariable(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowGlobalVariables(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowConfig(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemIndex(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowQueries(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowQuery(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTransactions(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTransaction(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTransactionHistory(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowLogs(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowCatalog(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListCatalogKey(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowCatalogToFile(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowPersistenceFiles(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowPersistenceObjects(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowPersistenceObject(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemory(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemoryObjects(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemoryAllocation(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowFunction(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListSnapshots(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowSnapshot(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListCaches(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowCache(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListCompact(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListCheckpoint(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowCheckpoint(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListOptimize(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListImport(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteListClean(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowClean(QueryContext *query_context, ShowOperatorState *operator_state);

private:
    ShowStmtType show_type_{ShowStmtType::kInvalid};
    std::string db_name_{};
    std::optional<std::string> object_name_{};
    u64 table_index_{};

    std::optional<std::string> file_path_{};
    std::optional<SegmentID> segment_id_{};
    std::optional<BlockID> block_id_{};
    std::optional<ChunkID> chunk_id_{};
    std::optional<ColumnID> column_id_{};
    std::optional<std::string> index_name_{};
    std::optional<u64> session_id_{};
    std::optional<TransactionID> txn_id_{};
    std::optional<std::string> function_name_{};
    bool show_nullable_{true};

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity
