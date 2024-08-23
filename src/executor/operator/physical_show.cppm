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

export module physical_show;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import logical_show;
import load_meta;
import infinity_exception;
import internal_types;
import column_def;
import data_type;
import variables;
import data_block;
import logger;

namespace infinity {

export class PhysicalShow : public PhysicalOperator {
public:
    explicit PhysicalShow(u64 id,
                          ShowType type,
                          String db_name,
                          String object_name,
                          u64 table_index,
                          Optional<SegmentID> segment_id,
                          Optional<BlockID> block_id,
                          Optional<ChunkID> chunk_id,
                          Optional<ColumnID> column_id,
                          Optional<String> index_name,
                          Optional<u64> session_id,
                          Optional<TransactionID> txn_id,
                          SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kShow, nullptr, nullptr, id, load_metas), show_type_(type), db_name_(std::move(db_name)),
          object_name_(std::move(object_name)), table_index_(table_index), segment_id_(segment_id), block_id_(block_id), chunk_id_(chunk_id), column_id_(column_id),
          index_name_(index_name), session_id_(session_id), txn_id_(txn_id) {}

    ~PhysicalShow() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

    inline ShowType show_type() const { return show_type_; }

    inline const String &db_name() const { return db_name_; };

    inline const String &object_name() const { return object_name_; };

private:
    void ExecuteShowViewDetail(QueryContext *query_context,
                               const SharedPtr<Vector<SharedPtr<DataType>>> &column_types,
                               const SharedPtr<Vector<String>> &column_names);

    void ExecuteShowDatabase(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTable(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndex(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndexSegment(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowIndexChunk(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTables(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowViews(QueryContext *query_context, ShowOperatorState *operator_state);

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

    void ExecuteShowBuffer(QueryContext *query_context, ShowOperatorState *operator_state);
    
    void ExecuteShowMemIndex(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowQueries(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowQuery(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTransactions(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowTransaction(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowLogs(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowDeltaLogs(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowCatalogs(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowPersistenceFiles(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowPersistenceObjects(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowPersistenceObject(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemory(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemoryObjects(QueryContext *query_context, ShowOperatorState *operator_state);

    void ExecuteShowMemoryAllocation(QueryContext *query_context, ShowOperatorState *operator_state);

private:
    ShowType show_type_{ShowType::kInvalid};
    String db_name_{};
    String object_name_{};
    u64 table_index_{};

    Optional<SegmentID> segment_id_{};
    Optional<BlockID> block_id_{};
    Optional<ChunkID> chunk_id_{};
    Optional<ColumnID> column_id_{};
    Optional<String> index_name_{};
    Optional<u64> session_id_{};
    Optional<TransactionID> txn_id_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
