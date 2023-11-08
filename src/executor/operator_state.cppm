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

import stl;
import global_block_id;
import physical_operator_type;
import fragment_data_queue;
import fragment_data;
import data_block;
import table_scan_function_data;
import knn_scan_data;
import table_def;
import parser;
import merge_knn_data;

export module operator_state;

namespace infinity {

export struct OperatorState {
    inline explicit OperatorState(PhysicalOperatorType operator_type) : operator_type_(operator_type) {}

    // Input status
    OperatorState *prev_op_state_{nullptr};
    i64 received_data_count_{0};
    i64 total_data_count_{0};

    inline void ConnectToPrevOutputOpState(OperatorState *prev_op_state) {
        prev_op_state_ = prev_op_state;
    }

    // Output status
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    SharedPtr<DataBlock> data_block_{};
    UniquePtr<String> error_message_{};

    bool complete_{false};

    inline void SetComplete() { complete_ = true; }

    inline bool Complete() const { return complete_; }
};

// Aggregate
export struct AggregateOperatorState : public OperatorState {
    inline explicit AggregateOperatorState() : OperatorState(PhysicalOperatorType::kAggregate) {}
};

// Merge Parallel Aggregate
export struct MergeParallelAggregateOperatorState : public OperatorState {
    inline explicit MergeParallelAggregateOperatorState() : OperatorState(PhysicalOperatorType::kMergeParallelAggregate) {}
};

// Parallel Aggregate
export struct ParallelAggregateOperatorState : public OperatorState {
    inline explicit ParallelAggregateOperatorState() : OperatorState(PhysicalOperatorType::kParallelAggregate) {}
};

// UnionAll
export struct UnionAllOperatorState : public OperatorState {
    inline explicit UnionAllOperatorState() : OperatorState(PhysicalOperatorType::kUnionAll) {}
};

// TableScan
export struct TableScanOperatorState : public OperatorState {
    inline explicit TableScanOperatorState() : OperatorState(PhysicalOperatorType::kTableScan) {}

    SharedPtr<TableScanFunctionData> table_scan_function_data_{};
};

// KnnScan
export struct KnnScanOperatorState : public OperatorState {
    inline explicit KnnScanOperatorState() : OperatorState(PhysicalOperatorType::kKnnScan) {}

    SharedPtr<KnnScanFunctionData> knn_scan_function_data_{};
};

// Merge Knn
export struct MergeKnnOperatorState : public OperatorState {
    inline explicit MergeKnnOperatorState() : OperatorState(PhysicalOperatorType::kMergeKnn) {}

    DataBlock* input_data_block_{nullptr}; // Since merge knn is the first op, no previous operator state. This ptr is to get input data.
    bool input_complete_{false};
    SharedPtr<MergeKnnFunctionData> merge_knn_function_data_{};
};

// Filter
export struct FilterOperatorState : public OperatorState {
    inline explicit FilterOperatorState() : OperatorState(PhysicalOperatorType::kFilter) {}
};
// IndexScan
export struct IndexScanOperatorState : public OperatorState {
    inline explicit IndexScanOperatorState() : OperatorState(PhysicalOperatorType::kIndexScan) {}
};

// Hash
export struct HashOperatorState : public OperatorState {
    inline explicit HashOperatorState() : OperatorState(PhysicalOperatorType::kHash) {}
};

// Merge Hash
export struct MergeHashOperatorState : public OperatorState {
    inline explicit MergeHashOperatorState() : OperatorState(PhysicalOperatorType::kMergeHash) {}
};

// Hash Join
export struct HashJoinOperatorState : public OperatorState {
    inline explicit HashJoinOperatorState() : OperatorState(PhysicalOperatorType::kJoinHash) {}
};

// Nested Loop
export struct NestedLoopOperatorState : public OperatorState {
    inline explicit NestedLoopOperatorState() : OperatorState(PhysicalOperatorType::kJoinNestedLoop) {}
};

// Merge Join
export struct MergeJoinOperatorState : public OperatorState {
    inline explicit MergeJoinOperatorState() : OperatorState(PhysicalOperatorType::kJoinMerge) {}
};

// Index Join
export struct IndexJoinOperatorState : public OperatorState {
    inline explicit IndexJoinOperatorState() : OperatorState(PhysicalOperatorType::kJoinIndex) {}
};

// Cross Product
export struct CrossProductOperatorState : public OperatorState {
    inline explicit CrossProductOperatorState() : OperatorState(PhysicalOperatorType::kCrossProduct) {}
};

// Limit
export struct LimitOperatorState : public OperatorState {
    inline explicit LimitOperatorState() : OperatorState(PhysicalOperatorType::kLimit) {}
};

// Merge Limit
export struct MergeLimitOperatorState : public OperatorState {
    inline explicit MergeLimitOperatorState() : OperatorState(PhysicalOperatorType::kMergeLimit) {}
};

// Merge Top
export struct MergeTopOperatorState : public OperatorState {
    inline explicit MergeTopOperatorState() : OperatorState(PhysicalOperatorType::kMergeTop) {}
};

// Top
export struct TopOperatorState : public OperatorState {
    inline explicit TopOperatorState() : OperatorState(PhysicalOperatorType::kTop) {}
};

// Projection
export struct ProjectionOperatorState : public OperatorState {
    inline explicit ProjectionOperatorState() : OperatorState(PhysicalOperatorType::kProjection) {}
};

// Sort
export struct SortOperatorState : public OperatorState {
    inline explicit SortOperatorState() : OperatorState(PhysicalOperatorType::kSort) {}
};

// Merge Sort
export struct MergeSortOperatorState : public OperatorState {
    inline explicit MergeSortOperatorState() : OperatorState(PhysicalOperatorType::kMergeSort) {}
};

// Delete
export struct DeleteOperatorState : public OperatorState {
    inline explicit DeleteOperatorState() : OperatorState(PhysicalOperatorType::kDelete) {}

    u64 count_{0};
    u64 sum_{0};
};
// Update
export struct UpdateOperatorState : public OperatorState {
    inline explicit UpdateOperatorState() : OperatorState(PhysicalOperatorType::kUpdate) {}

    u64 count_{0};
    u64 sum_{0};
};

// Insert
export struct InsertOperatorState : public OperatorState {
    inline explicit InsertOperatorState() : OperatorState(PhysicalOperatorType::kInsert) {}

    UniquePtr<String> result_msg_{};
};

// Import
export struct ImportOperatorState : public OperatorState {
    inline explicit ImportOperatorState() : OperatorState(PhysicalOperatorType::kImport) {}

    Vector<SharedPtr<DataBlock>> output_{};
    SharedPtr<TableDef> table_def_{};
    // For insert, update, delete, update
    UniquePtr<String> result_msg_{};
};

// Export
export struct ExportOperatorState : public OperatorState {
    inline explicit ExportOperatorState() : OperatorState(PhysicalOperatorType::kExport) {}
};

// Alter
export struct AlterOperatorState : public OperatorState {
    inline explicit AlterOperatorState() : OperatorState(PhysicalOperatorType::kAlter) {}
};

// Create Table
export struct CreateTableOperatorState : public OperatorState {
    inline explicit CreateTableOperatorState() : OperatorState(PhysicalOperatorType::kCreateTable) {}
};

export struct CreateIndexOperatorState : public OperatorState {
    inline explicit CreateIndexOperatorState() : OperatorState(PhysicalOperatorType::kCreateIndex) {}
};

// Create Collection
export struct CreateCollectionOperatorState : public OperatorState {
    inline explicit CreateCollectionOperatorState() : OperatorState(PhysicalOperatorType::kCreateCollection) {}
};

// Create Database
export struct CreateDatabaseOperatorState : public OperatorState {
    inline explicit CreateDatabaseOperatorState() : OperatorState(PhysicalOperatorType::kCreateDatabase) {}
};

// Create View
export struct CreateViewOperatorState : public OperatorState {
    inline explicit CreateViewOperatorState() : OperatorState(PhysicalOperatorType::kCreateView) {}
};

// Drop Table
export struct DropTableOperatorState : public OperatorState {
    inline explicit DropTableOperatorState() : OperatorState(PhysicalOperatorType::kDropTable) {}
};

export struct DropIndexOperatorState : public OperatorState {
    inline explicit DropIndexOperatorState() : OperatorState(PhysicalOperatorType::kDropIndex) {}
};

// Drop Collection
export struct DropCollectionOperatorState : public OperatorState {
    inline explicit DropCollectionOperatorState() : OperatorState(PhysicalOperatorType::kDropCollection) {}
};

// Drop Database
export struct DropDatabaseOperatorState : public OperatorState {
    inline explicit DropDatabaseOperatorState() : OperatorState(PhysicalOperatorType::kDropDatabase) {}
};

// Drop View
export struct DropViewOperatorState : public OperatorState {
    inline explicit DropViewOperatorState() : OperatorState(PhysicalOperatorType::kDropView) {}
};

// Command
export struct CommandOperatorState : public OperatorState {
    inline explicit CommandOperatorState() : OperatorState(PhysicalOperatorType::kCommand) {}
};

// Explain
export struct ExplainOperatorState : public OperatorState {
    inline explicit ExplainOperatorState() : OperatorState(PhysicalOperatorType::kExplain) {}
};

// Show
export struct ShowOperatorState : public OperatorState {
    inline explicit ShowOperatorState() : OperatorState(PhysicalOperatorType::kShow) {}

    Vector<SharedPtr<DataBlock>> output_{};
};

// Flush
export struct FlushOperatorState : public OperatorState {
    inline explicit FlushOperatorState() : OperatorState(PhysicalOperatorType::kFlush) {}
};

// Sink
export enum class SinkStateType {
    kInvalid,
    kMaterialize,
    kResult,
    kMessage,
    kSummary,
    kQueue,
};

export struct SinkState {
    virtual ~SinkState(){};
    inline explicit SinkState(SinkStateType state_type) : state_type_(state_type) {}

    inline void SetPrevOpState(OperatorState *prev_op_state) { prev_op_state_ = prev_op_state; }

    inline SinkStateType state_type() const { return state_type_; }

    OperatorState *prev_op_state_{};
    SinkStateType state_type_{SinkStateType::kInvalid};
    UniquePtr<String> error_message_{};
};

export struct QueueSinkState : public SinkState {
    inline explicit QueueSinkState() : SinkState(SinkStateType::kQueue) {}

    Vector<SharedPtr<DataBlock>> data_block_array_{};
    Vector<FragmentDataQueue *> fragment_data_queues_;
};

export struct MaterializeSinkState : public SinkState {
    inline explicit MaterializeSinkState() : SinkState(SinkStateType::kMaterialize) {}

    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<String>> column_names_{};
    Vector<SharedPtr<DataBlock>> data_block_array_{};
};

export struct ResultSinkState : public SinkState {
    inline explicit ResultSinkState() : SinkState(SinkStateType::kResult) {}

    SharedPtr<ColumnDef> result_def_{};
};

export struct MessageSinkState : public SinkState {
    inline explicit MessageSinkState() : SinkState(SinkStateType::kMessage) {}

    UniquePtr<String> message_{};
};

export struct SummarySinkState : public SinkState {
    inline explicit SummarySinkState() : SinkState(SinkStateType::kSummary), count_(0), sum_(0) {}

    u64 count_;
    u64 sum_;
};

// Source
export enum class SourceStateType { kInvalid, kQueue, kAggregate, kTableScan, kKnnScan, kEmpty };

export struct SourceState {
    inline explicit SourceState(SourceStateType state_type) : state_type_(state_type) {}

    inline void SetNextOpState(OperatorState *op_state) {
        next_op_state_ = op_state;
    }

    bool complete_{false};
    OperatorState *next_op_state_{};
    SourceStateType state_type_{SourceStateType::kInvalid};
};

export struct QueueSourceState : public SourceState {
    inline explicit QueueSourceState() : SourceState(SourceStateType::kQueue) {}

    inline void SetTotalDataCount(i64 data_count) {
        if (next_op_state_->total_data_count_ == 0) {
            next_op_state_->total_data_count_ = data_count;
        }
    }

    void PushData(DataBlock *input_data_block);

    FragmentDataQueue source_queue_;

    SharedPtr<FragmentData> current_fragment_data_{};
};

export struct AggregateSourceState : public SourceState {
    explicit AggregateSourceState(i64 hash_start, i64 hash_end)
        : SourceState(SourceStateType::kAggregate), hash_start_(hash_start), hash_end_(hash_end) {}

    inline SharedPtr<DataBlock> GetNextDataBlock() { return nullptr; }

    i64 hash_start_{};
    i64 hash_end_{};

    FragmentDataQueue source_queue_;
};

export struct TableScanSourceState : public SourceState {
    explicit TableScanSourceState(SharedPtr<Vector<GlobalBlockID>> global_ids)
        : SourceState(SourceStateType::kTableScan), global_ids_(Move(global_ids)) {}

    SharedPtr<Vector<GlobalBlockID>> global_ids_;
};

export struct KnnScanSourceState : public SourceState {
    explicit KnnScanSourceState(SharedPtr<Vector<GlobalBlockID>> global_ids)
        : SourceState(SourceStateType::kKnnScan), global_ids_(Move(global_ids)) {}

    SharedPtr<Vector<GlobalBlockID>> global_ids_;
};

export struct EmptySourceState : public SourceState {
    explicit EmptySourceState() : SourceState(SourceStateType::kEmpty) {}
};

} // namespace infinity
