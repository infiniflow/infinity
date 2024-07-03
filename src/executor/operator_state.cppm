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

export module operator_state;

import stl;
import global_block_id;
import physical_operator_type;
import fragment_data;
import data_block;
import table_scan_function_data;
import knn_scan_data;
import match_tensor_scan_function_data;
import match_sparse_scan_function_data;
import compact_state_data;
import table_def;

import merge_knn_data;
import create_index_data;
import blocking_queue;
import expression_state;
import status;
import internal_types;
import column_def;
import data_type;
import segment_entry;

namespace infinity {

export struct OperatorState {
    inline explicit OperatorState(PhysicalOperatorType operator_type) : operator_type_(operator_type) {}
    virtual ~OperatorState() = default;

    // Input status
    OperatorState *prev_op_state_{nullptr};

    inline void ConnectToPrevOutputOpState(OperatorState *prev_op_state) { prev_op_state_ = prev_op_state; }

    inline bool Ok() const { return status_.ok(); }

    // Output status
    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    Vector<UniquePtr<DataBlock>> data_block_array_{};
    //    UniquePtr<String> error_message_{};
    Status status_{};
    bool empty_source_{false};

    bool complete_{false};

    inline void SetComplete() { complete_ = true; }

    inline bool Complete() const { return complete_; }
};

// Aggregate
export struct AggregateOperatorState : public OperatorState {
    inline explicit AggregateOperatorState(Vector<UniquePtr<char[]>> states)
        : OperatorState(PhysicalOperatorType::kAggregate), states_(std::move(states)) {}

    Vector<UniquePtr<char[]>> states_;
};

// Merge Aggregate
export struct MergeAggregateOperatorState : public OperatorState {
    inline explicit MergeAggregateOperatorState() : OperatorState(PhysicalOperatorType::kMergeAggregate) {}

    /// Since merge agg is the first op, no previous operator state. This ptr is to get input data.
    // Vector<UniquePtr<DataBlock>> input_data_blocks_{nullptr};
    UniquePtr<DataBlock> input_data_block_{nullptr};
    bool input_complete_{false};
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

    UniquePtr<TableScanFunctionData> table_scan_function_data_{};
};

// MatchTensorScan
export struct MatchTensorScanOperatorState : public OperatorState {
    inline explicit MatchTensorScanOperatorState() : OperatorState(PhysicalOperatorType::kMatchTensorScan) {}

    UniquePtr<MatchTensorScanFunctionData> match_tensor_scan_function_data_{};
};

// MergeMatchTensor
export struct MergeMatchTensorOperatorState : public OperatorState {
    inline explicit MergeMatchTensorOperatorState() : OperatorState(PhysicalOperatorType::kMergeMatchTensor) {}

    Vector<UniquePtr<DataBlock>> middle_sorted_data_blocks_; // middle result
    u32 middle_result_count_{};
    Vector<UniquePtr<DataBlock>> input_data_blocks_;
    bool input_complete_{false};
};

// MatchSparseScan
export struct MatchSparseScanOperatorState : public OperatorState {
    inline explicit MatchSparseScanOperatorState() : OperatorState(PhysicalOperatorType::kMatchSparseScan) {}

    MatchSparseScanFunctionData match_sparse_scan_function_data_{};
};

// MergeMatchSparse
export struct MergeMatchSparseOperatorState : public OperatorState {
    inline explicit MergeMatchSparseOperatorState() : OperatorState(PhysicalOperatorType::kMergeMatchSparse) {}

    UniquePtr<DataBlock> input_data_block_{nullptr};
    bool input_complete_{false};
    MergeSparseFunctionData merge_sparse_function_data_{};
};

// KnnScan
export struct KnnScanOperatorState : public OperatorState {
    inline explicit KnnScanOperatorState() : OperatorState(PhysicalOperatorType::kKnnScan) {}

    //    Vector<SharedPtr<DataBlock>> output_data_blocks_{};
    UniquePtr<KnnScanFunctionData> knn_scan_function_data_{};
};

// Merge Knn
export struct MergeKnnOperatorState : public OperatorState {
    inline explicit MergeKnnOperatorState() : OperatorState(PhysicalOperatorType::kMergeKnn) {}

    UniquePtr<DataBlock> input_data_block_{nullptr}; // Since merge knn is the first op, no previous operator state. This ptr is to get input data.
    bool input_complete_{false};
    SharedPtr<MergeKnnFunctionData> merge_knn_function_data_{};
};

// Filter
export struct FilterOperatorState : public OperatorState {
    inline explicit FilterOperatorState() : OperatorState(PhysicalOperatorType::kFilter) {}
};

// IndexScan
export struct IndexScanOperatorState : public OperatorState {
    inline explicit IndexScanOperatorState(UniquePtr<Vector<SegmentID>> &&segment_ids)
        : OperatorState(PhysicalOperatorType::kIndexScan), segment_ids_(std::move(segment_ids)) {}
    UniquePtr<Vector<SegmentID>> segment_ids_; // moved from IndexScanSourceState
    u32 next_idx_{};
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

    Vector<UniquePtr<DataBlock>> input_data_blocks_{}; // Since merge knn is the first op, no previous operator state. This ptr is to get input data.
    bool input_complete_{false};
};

// Merge Top
export struct MergeTopOperatorState : public OperatorState {
    inline explicit MergeTopOperatorState() : OperatorState(PhysicalOperatorType::kMergeTop) {}
    Vector<SharedPtr<ExpressionState>> expr_states_;         // expression states
    Vector<UniquePtr<DataBlock>> middle_sorted_data_blocks_; // middle result
    u32 middle_result_count_{};
    Vector<UniquePtr<DataBlock>> input_data_blocks_;
    bool input_complete_{false};
};

// Top
export struct TopOperatorState : public OperatorState {
    inline explicit TopOperatorState() : OperatorState(PhysicalOperatorType::kTop) {}
    Vector<SharedPtr<ExpressionState>> expr_states_; // expression states
};

// Projection
export struct ProjectionOperatorState : public OperatorState {
    inline explicit ProjectionOperatorState() : OperatorState(PhysicalOperatorType::kProjection) {}
};

// Sort
export struct SortOperatorState : public OperatorState {
    inline explicit SortOperatorState() : OperatorState(PhysicalOperatorType::kSort) {}
    Vector<SharedPtr<ExpressionState>> expr_states_; // expression states
    Vector<UniquePtr<DataBlock>> unmerge_sorted_blocks_{};
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

    //    Vector<SharedPtr<DataBlock>> output_{};
    SharedPtr<TableDef> table_def_{};
    // For insert, update, delete, update
    UniquePtr<String> result_msg_{};
};

// Export
export struct ExportOperatorState : public OperatorState {
    inline explicit ExportOperatorState() : OperatorState(PhysicalOperatorType::kExport) {}
    UniquePtr<String> result_msg_{};
};

// Alter
export struct AlterOperatorState : public OperatorState {
    inline explicit AlterOperatorState() : OperatorState(PhysicalOperatorType::kAlter) {}
};

// Create Table
export struct CreateTableOperatorState : public OperatorState {
    inline explicit CreateTableOperatorState() : OperatorState(PhysicalOperatorType::kCreateTable) {}
};

export struct CreateIndexPrepareOperatorState : public OperatorState {
    inline explicit CreateIndexPrepareOperatorState() : OperatorState(PhysicalOperatorType::kCreateIndexPrepare) {}

    UniquePtr<String> result_msg_{};
};

export struct CreateIndexDoOperatorState : public OperatorState {
    inline explicit CreateIndexDoOperatorState() : OperatorState(PhysicalOperatorType::kCreateIndexDo) {}

    UniquePtr<String> result_msg_{};
    CreateIndexSharedData *create_index_shared_data_;
};

export struct CreateIndexFinishOperatorState : public OperatorState {
    inline explicit CreateIndexFinishOperatorState() : OperatorState(PhysicalOperatorType::kCreateIndexFinish) {}

    UniquePtr<String> error_message_{};
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

    Vector<UniquePtr<DataBlock>> output_{};
};

// Flush
export struct FlushOperatorState : public OperatorState {
    inline explicit FlushOperatorState() : OperatorState(PhysicalOperatorType::kFlush) {}
};

// Optimize
export struct OptimizeOperatorState : public OperatorState {
    inline explicit OptimizeOperatorState() : OperatorState(PhysicalOperatorType::kOptimize) {}
};

// Match
export struct MatchOperatorState : public OperatorState {
    inline explicit MatchOperatorState() : OperatorState(PhysicalOperatorType::kMatch) {}
};

// Fusion
export struct FusionOperatorState : public OperatorState {
    inline explicit FusionOperatorState() : OperatorState(PhysicalOperatorType::kFusion) {}

    // Fusion is the first op, no previous operator state.
    // This is to tell op that source is drained.
    bool input_complete_{false};
    // This is to cache all input data before calculation.
    Map<u64, Vector<UniquePtr<DataBlock>>> input_data_blocks_{};
};

// Compact
export struct CompactOperatorState : public OperatorState {
    inline explicit CompactOperatorState(Vector<Vector<SegmentEntry *>> segment_groups, SharedPtr<CompactStateData> compact_state_data)
        : OperatorState(PhysicalOperatorType::kCompact), segment_groups_(std::move(segment_groups)), compact_state_data_(compact_state_data) {}

    SizeT compact_idx_{};
    Vector<Vector<SegmentEntry *>> segment_groups_;
    SharedPtr<CompactStateData> compact_state_data_{};
};

export struct CompactIndexPrepareOperatorState : public OperatorState {
    inline explicit CompactIndexPrepareOperatorState(SharedPtr<CompactStateData> compact_state_data,
                                                     SharedPtr<Vector<UniquePtr<CreateIndexSharedData>>> create_index_shared_data)
        : OperatorState(PhysicalOperatorType::kCompactIndexPrepare), compact_state_data_(compact_state_data),
          create_index_shared_data_(create_index_shared_data) {}

    SharedPtr<CompactStateData> compact_state_data_{};
    SharedPtr<Vector<UniquePtr<CreateIndexSharedData>>> create_index_shared_data_{};
    SizeT create_index_idx_{};
};

export struct CompactIndexDoOperatorState : public OperatorState {
    inline explicit CompactIndexDoOperatorState(SharedPtr<CompactStateData> compact_state_data,
                                                SharedPtr<Vector<UniquePtr<CreateIndexSharedData>>> create_index_shared_data)
        : OperatorState(PhysicalOperatorType::kCompactIndexDo), compact_state_data_(compact_state_data),
          create_index_shared_data_(create_index_shared_data) {}

    SharedPtr<CompactStateData> compact_state_data_{};
    SharedPtr<Vector<UniquePtr<CreateIndexSharedData>>> create_index_shared_data_{};
    SizeT create_index_idx_{};
};

export struct CompactFinishOperatorState : public OperatorState {
    explicit CompactFinishOperatorState(SharedPtr<CompactStateData> compact_state_data)
        : OperatorState(PhysicalOperatorType::kCompactFinish), compact_state_data_(compact_state_data) {}

    SharedPtr<CompactStateData> compact_state_data_{};
};

// Source
export enum class SourceStateType {
    kInvalid,
    kQueue,
    kAggregate,
    kTableScan,
    kIndexScan,
    kKnnScan,
    kMatchTensorScan,
    kMatchSparseScan,
    kCompact,
    kEmpty,
};

export struct SourceState {
    inline explicit SourceState(SourceStateType state_type) : state_type_(state_type) {}

    virtual ~SourceState() = default;

    inline void SetNextOpState(OperatorState *op_state) { next_op_state_ = op_state; }

    inline bool Complete() const { return complete_; }

    bool complete_{false};
    OperatorState *next_op_state_{};
    SourceStateType state_type_{SourceStateType::kInvalid};
    //    UniquePtr<String> error_message_{};
    Status status_{};
};

export struct QueueSourceState : public SourceState {
    inline explicit QueueSourceState() : SourceState(SourceStateType::kQueue) {}

    inline void SetTaskNum(u64 fragment_id, u64 num_tasks) { num_tasks_[fragment_id] = num_tasks; }

    bool GetData();

    BlockingQueue<SharedPtr<FragmentDataBase>> source_queue_{};

    Map<u64, u64> num_tasks_; // fragment_id -> number of pending tasks

private:
    void MarkCompletedTask(u64 fragment_id);
};

export struct AggregateSourceState : public SourceState {
    explicit AggregateSourceState(i64 hash_start, i64 hash_end)
        : SourceState(SourceStateType::kAggregate), hash_start_(hash_start), hash_end_(hash_end) {}

    inline SharedPtr<DataBlock> GetNextDataBlock() { return nullptr; }

    i64 hash_start_{};
    i64 hash_end_{};

    //    BlockingQueue<UniquePtr<FragmentDataBase>> source_queue_{};
};

export struct TableScanSourceState : public SourceState {
    explicit TableScanSourceState(SharedPtr<Vector<GlobalBlockID>> global_ids)
        : SourceState(SourceStateType::kTableScan), global_ids_(std::move(global_ids)) {}

    SharedPtr<Vector<GlobalBlockID>> global_ids_;
};

export struct MatchTensorScanSourceState : public SourceState {
    explicit MatchTensorScanSourceState() : SourceState(SourceStateType::kMatchTensorScan) {}
};

export struct MatchSparseScanSourceState : public SourceState {
    explicit MatchSparseScanSourceState(SharedPtr<Vector<GlobalBlockID>> global_ids, SharedPtr<Vector<SegmentID>> segment_ids)
        : SourceState(SourceStateType::kMatchSparseScan), global_ids_(std::move(global_ids)), segment_ids_(std::move(segment_ids)) {}

    SharedPtr<Vector<GlobalBlockID>> global_ids_;
    SharedPtr<Vector<SegmentID>> segment_ids_;
};

export struct IndexScanSourceState : public SourceState {
    explicit IndexScanSourceState(UniquePtr<Vector<SegmentID>> &&segment_ids)
        : SourceState(SourceStateType::kIndexScan), segment_ids_(std::move(segment_ids)) {}

    UniquePtr<Vector<SegmentID>> segment_ids_; // will be moved into IndexScanOperatorState
};

export struct KnnScanSourceState : public SourceState {
    explicit KnnScanSourceState() : SourceState(SourceStateType::kKnnScan) {}
};

export struct CompactSourceState : public SourceState {
    explicit CompactSourceState(Vector<Vector<SegmentEntry *>> segment_groups)
        : SourceState(SourceStateType::kCompact), segment_groups_(std::move(segment_groups)) {}

    Vector<Vector<SegmentEntry *>> segment_groups_;
};

export struct EmptySourceState : public SourceState {
    explicit EmptySourceState() : SourceState(SourceStateType::kEmpty) {}
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
    virtual ~SinkState() {}
    inline explicit SinkState(SinkStateType state_type, u64 fragment_id, u64 task_id)
        : fragment_id_(fragment_id), task_id_(task_id), state_type_(state_type) {}

    inline void SetPrevOpState(OperatorState *prev_op_state) { prev_op_state_ = prev_op_state; }

    [[nodiscard]] inline SinkStateType state_type() const { return state_type_; }

    inline bool Error() const { return !status_.ok(); }

    bool Ignore() const { return status_.code() == ErrorCode::kIgnore; }

    u64 fragment_id_{};
    u64 task_id_{};
    OperatorState *prev_op_state_{};
    SinkStateType state_type_{SinkStateType::kInvalid};
    //    UniquePtr<String> error_message_{};
    Status status_{};
};

export struct QueueSinkState : public SinkState {
    inline explicit QueueSinkState(u64 fragment_id, u64 task_id) : SinkState(SinkStateType::kQueue, fragment_id, task_id) {}

    Vector<UniquePtr<DataBlock>> data_block_array_{};
    Vector<BlockingQueue<SharedPtr<FragmentDataBase>> *> fragment_data_queues_;
};

export struct MaterializeSinkState : public SinkState {
    inline explicit MaterializeSinkState(u64 fragment_id, u64 task_id) : SinkState(SinkStateType::kMaterialize, fragment_id, task_id) {}

    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<String>> column_names_{};
    Vector<UniquePtr<DataBlock>> data_block_array_{};

    bool empty_result_{false};
};

export struct ResultSinkState : public SinkState {
    inline explicit ResultSinkState() : SinkState(SinkStateType::kResult, 0, 0) {}

    SharedPtr<ColumnDef> result_def_{};
};

export struct MessageSinkState : public SinkState {
    inline explicit MessageSinkState() : SinkState(SinkStateType::kMessage, 0, 0) {}

    UniquePtr<String> message_{};
};

export struct SummarySinkState : public SinkState {
    inline explicit SummarySinkState() : SinkState(SinkStateType::kSummary, 0, 0), count_(0), sum_(0) {}

    u64 count_;
    u64 sum_;
};

} // namespace infinity
