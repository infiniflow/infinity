//
// Created by JinHai on 2022/9/15.
//


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

export struct OutputState {
    inline explicit OutputState(PhysicalOperatorType operator_type) : operator_type_(operator_type) {}

    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    SharedPtr<DataBlock> data_block_{};
    UniquePtr<String> error_message_{};

    bool complete_{false};

    inline void SetComplete() { complete_ = true; }

    inline bool Complete() const { return complete_; }
};

export struct InputState {
    inline explicit InputState(PhysicalOperatorType operator_type) : operator_type_(operator_type) {}

    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    DataBlock *input_data_block_{nullptr}; // this is pointed to the previous operator output
    bool *input_complete_ptr_{nullptr};
    i64 received_data_count_{0};
    i64 total_data_count_{0};

    inline bool Complete() const { return *input_complete_ptr_ == true; }

    inline void ConnectToPrevOutputOpState(OutputState *output_state) {
        input_data_block_ = output_state->data_block_.get();
        input_complete_ptr_ = &output_state->complete_;
    }
};

// Aggregate
export struct AggregateInputState : public InputState {
    inline explicit AggregateInputState() : InputState(PhysicalOperatorType::kAggregate) {}
};

export struct AggregateOutputState : public OutputState {
    inline explicit AggregateOutputState() : OutputState(PhysicalOperatorType::kAggregate) {}
};

// Merge Parallel Aggregate
export struct MergeParallelAggregateInputState : public InputState {
    inline explicit MergeParallelAggregateInputState() : InputState(PhysicalOperatorType::kMergeParallelAggregate) {}
};

export struct MergeParallelAggregateOutputState : public OutputState {
    inline explicit MergeParallelAggregateOutputState() : OutputState(PhysicalOperatorType::kMergeParallelAggregate) {}
};

// Parallel Aggregate
export struct ParallelAggregateInputState : public InputState {
    inline explicit ParallelAggregateInputState() : InputState(PhysicalOperatorType::kParallelAggregate) {}
};

export struct ParallelAggregateOutputState : public OutputState {
    inline explicit ParallelAggregateOutputState() : OutputState(PhysicalOperatorType::kParallelAggregate) {}
};

// UnionAll
export struct UnionAllInputState : public InputState {
    inline explicit UnionAllInputState() : InputState(PhysicalOperatorType::kUnionAll) {}
};

export struct UnionAllOutputState : public OutputState {
    inline explicit UnionAllOutputState() : OutputState(PhysicalOperatorType::kUnionAll) {}
};

// TableScan
export struct TableScanInputState : public InputState {
    inline explicit TableScanInputState() : InputState(PhysicalOperatorType::kTableScan) {}

    SharedPtr<TableScanFunctionData> table_scan_function_data_{};
};

export struct TableScanOutputState : public OutputState {
    inline explicit TableScanOutputState() : OutputState(PhysicalOperatorType::kTableScan) {}
};

// KnnScan
export struct KnnScanInputState : public InputState {
    inline explicit KnnScanInputState() : InputState(PhysicalOperatorType::kKnnScan) {}

    SharedPtr<KnnScanFunctionData> knn_scan_function_data_{};
};

export struct KnnScanOutputState : public OutputState {
    inline explicit KnnScanOutputState() : OutputState(PhysicalOperatorType::kKnnScan) {}
};

// Merge Knn
export struct MergeKnnInputState : public InputState {
    inline explicit MergeKnnInputState() : InputState(PhysicalOperatorType::kMergeKnn) {}

    SharedPtr<MergeKnnFunctionData> merge_knn_function_data_{};
};

export struct MergeKnnOutputState : public OutputState {
    inline explicit MergeKnnOutputState() : OutputState(PhysicalOperatorType::kMergeKnn) {}
};

// Filter
export struct FilterInputState : public InputState {
    inline explicit FilterInputState() : InputState(PhysicalOperatorType::kFilter) {}
};

export struct FilterOutputState : public OutputState {
    inline explicit FilterOutputState() : OutputState(PhysicalOperatorType::kFilter) {}
};

// IndexScan
export struct IndexScanInputState : public InputState {
    inline explicit IndexScanInputState() : InputState(PhysicalOperatorType::kIndexScan) {}
};

export struct IndexScanOutputState : public OutputState {
    inline explicit IndexScanOutputState() : OutputState(PhysicalOperatorType::kIndexScan) {}
};

// Hash
export struct HashInputState : public InputState {
    inline explicit HashInputState() : InputState(PhysicalOperatorType::kHash) {}
};

export struct HashOutputState : public OutputState {
    inline explicit HashOutputState() : OutputState(PhysicalOperatorType::kHash) {}
};

// Merge Hash
export struct MergeHashInputState : public InputState {
    inline explicit MergeHashInputState() : InputState(PhysicalOperatorType::kMergeHash) {}
};

export struct MergeHashOutputState : public OutputState {
    inline explicit MergeHashOutputState() : OutputState(PhysicalOperatorType::kMergeHash) {}
};

// Hash Join
export struct HashJoinInputState : public InputState {
    inline explicit HashJoinInputState() : InputState(PhysicalOperatorType::kJoinHash) {}
};

export struct HashJoinOutputState : public OutputState {
    inline explicit HashJoinOutputState() : OutputState(PhysicalOperatorType::kJoinHash) {}
};

// Nested Loop
export struct NestedLoopInputState : public InputState {
    inline explicit NestedLoopInputState() : InputState(PhysicalOperatorType::kJoinNestedLoop) {}
};

export struct NestedLoopOutputState : public OutputState {
    inline explicit NestedLoopOutputState() : OutputState(PhysicalOperatorType::kJoinNestedLoop) {}
};

// Merge Join
export struct MergeJoinInputState : public InputState {
    inline explicit MergeJoinInputState() : InputState(PhysicalOperatorType::kJoinMerge) {}
};

export struct MergeJoinOutputState : public OutputState {
    inline explicit MergeJoinOutputState() : OutputState(PhysicalOperatorType::kJoinMerge) {}
};

// Index Join
export struct IndexJoinInputState : public InputState {
    inline explicit IndexJoinInputState() : InputState(PhysicalOperatorType::kJoinIndex) {}
};

export struct IndexJoinOutputState : public OutputState {
    inline explicit IndexJoinOutputState() : OutputState(PhysicalOperatorType::kJoinIndex) {}
};

// Cross Product
export struct CrossProductInputState : public InputState {
    inline explicit CrossProductInputState() : InputState(PhysicalOperatorType::kCrossProduct) {}
};

export struct CrossProductOutputState : public OutputState {
    inline explicit CrossProductOutputState() : OutputState(PhysicalOperatorType::kCrossProduct) {}
};

// Limit
export struct LimitInputState : public InputState {
    inline explicit LimitInputState() : InputState(PhysicalOperatorType::kLimit) {}
};

export struct LimitOutputState : public OutputState {
    inline explicit LimitOutputState() : OutputState(PhysicalOperatorType::kLimit) {}
};

// Merge Limit
export struct MergeLimitInputState : public InputState {
    inline explicit MergeLimitInputState() : InputState(PhysicalOperatorType::kMergeLimit) {}
};

export struct MergeLimitOutputState : public OutputState {
    inline explicit MergeLimitOutputState() : OutputState(PhysicalOperatorType::kMergeLimit) {}
};

// Merge Top
export struct MergeTopInputState : public InputState {
    inline explicit MergeTopInputState() : InputState(PhysicalOperatorType::kMergeTop) {}
};

export struct MergeTopOutputState : public OutputState {
    inline explicit MergeTopOutputState() : OutputState(PhysicalOperatorType::kMergeTop) {}
};

// Top
export struct TopInputState : public InputState {
    inline explicit TopInputState() : InputState(PhysicalOperatorType::kTop) {}
};

export struct TopOutputState : public OutputState {
    inline explicit TopOutputState() : OutputState(PhysicalOperatorType::kTop) {}
};

// Projection
export struct ProjectionInputState : public InputState {
    inline explicit ProjectionInputState() : InputState(PhysicalOperatorType::kProjection) {}
};

export struct ProjectionOutputState : public OutputState {
    inline explicit ProjectionOutputState() : OutputState(PhysicalOperatorType::kProjection) {}
};

// Sort
export struct SortInputState : public InputState {
    inline explicit SortInputState() : InputState(PhysicalOperatorType::kSort) {}
};

export struct SortOutputState : public OutputState {
    inline explicit SortOutputState() : OutputState(PhysicalOperatorType::kSort) {}
};

// Merge Sort
export struct MergeSortInputState : public InputState {
    inline explicit MergeSortInputState() : InputState(PhysicalOperatorType::kMergeSort) {}
};

export struct MergeSortOutputState : public OutputState {
    inline explicit MergeSortOutputState() : OutputState(PhysicalOperatorType::kMergeSort) {}
};

// Delete
export struct DeleteInputState : public InputState {
    inline explicit DeleteInputState() : InputState(PhysicalOperatorType::kDelete) {}
};

export struct DeleteOutputState : public OutputState {
    inline explicit DeleteOutputState() : OutputState(PhysicalOperatorType::kDelete) {}
};

// Update
export struct UpdateInputState : public InputState {
    inline explicit UpdateInputState() : InputState(PhysicalOperatorType::kUpdate) {}
};

export struct UpdateOutputState : public OutputState {
    inline explicit UpdateOutputState() : OutputState(PhysicalOperatorType::kUpdate) {}
};

// Insert
export struct InsertInputState : public InputState {
    inline explicit InsertInputState() : InputState(PhysicalOperatorType::kInsert) {}
};

export struct InsertOutputState : public OutputState {
    inline explicit InsertOutputState() : OutputState(PhysicalOperatorType::kInsert) {}
    UniquePtr<String> result_msg_{};
};

// Import
export struct ImportInputState : public InputState {
    inline explicit ImportInputState() : InputState(PhysicalOperatorType::kImport) {}
};

export struct ImportOutputState : public OutputState {
    inline explicit ImportOutputState() : OutputState(PhysicalOperatorType::kImport) {}

    Vector<SharedPtr<DataBlock>> output_{};
    SharedPtr<TableDef> table_def_{};
    // For insert, update, delete, update
    UniquePtr<String> result_msg_{};
};

// Export
export struct ExportInputState : public InputState {
    inline explicit ExportInputState() : InputState(PhysicalOperatorType::kExport) {}
};

export struct ExportOutputState : public OutputState {
    inline explicit ExportOutputState() : OutputState(PhysicalOperatorType::kExport) {}
};

// Alter
export struct AlterInputState : public InputState {
    inline explicit AlterInputState() : InputState(PhysicalOperatorType::kAlter) {}
};

export struct AlterOutputState : public OutputState {
    inline explicit AlterOutputState() : OutputState(PhysicalOperatorType::kAlter) {}
};

// Create Table
export struct CreateTableInputState : public InputState {
    inline explicit CreateTableInputState() : InputState(PhysicalOperatorType::kCreateTable) {}
};

export struct CreateTableOutputState : public OutputState {
    inline explicit CreateTableOutputState() : OutputState(PhysicalOperatorType::kCreateTable) {}
};

export struct CreateIndexInputState : public InputState {
    inline explicit
    CreateIndexInputState() : InputState(PhysicalOperatorType::kCreateIndex) {}
};

export struct CreateIndexOutputState : public OutputState {
    inline explicit
    CreateIndexOutputState() : OutputState(PhysicalOperatorType::kCreateIndex) {}
};

// Create Collection
export struct CreateCollectionInputState : public InputState {
    inline explicit CreateCollectionInputState() : InputState(PhysicalOperatorType::kCreateCollection) {}
};

export struct CreateCollectionOutputState : public OutputState {
    inline explicit CreateCollectionOutputState() : OutputState(PhysicalOperatorType::kCreateCollection) {}
};

// Create Database
export struct CreateDatabaseInputState : public InputState {
    inline explicit CreateDatabaseInputState() : InputState(PhysicalOperatorType::kCreateDatabase) {}
};

export struct CreateDatabaseOutputState : public OutputState {
    inline explicit CreateDatabaseOutputState() : OutputState(PhysicalOperatorType::kCreateDatabase) {}
};

// Create View
export struct CreateViewInputState : public InputState {
    inline explicit CreateViewInputState() : InputState(PhysicalOperatorType::kCreateView) {}
};

export struct CreateViewOutputState : public OutputState {
    inline explicit CreateViewOutputState() : OutputState(PhysicalOperatorType::kCreateView) {}
};

// Drop Table
export struct DropTableInputState : public InputState {
    inline explicit DropTableInputState() : InputState(PhysicalOperatorType::kDropTable) {}
};

export struct DropTableOutputState : public OutputState {
    inline explicit DropTableOutputState() : OutputState(PhysicalOperatorType::kDropTable) {}
};

export struct DropIndexInputState : public InputState {
    inline explicit DropIndexInputState() : InputState(PhysicalOperatorType::kDropIndex) {}
};

export struct DropIndexOutputState : public OutputState {
    inline explicit DropIndexOutputState() : OutputState(PhysicalOperatorType::kDropIndex) {}
};

// Drop Collection
export struct DropCollectionInputState : public InputState {
    inline explicit DropCollectionInputState() : InputState(PhysicalOperatorType::kDropCollection) {}
};

export struct DropCollectionOutputState : public OutputState {
    inline explicit DropCollectionOutputState() : OutputState(PhysicalOperatorType::kDropCollection) {}
};

// Drop Database
export struct DropDatabaseInputState : public InputState {
    inline explicit DropDatabaseInputState() : InputState(PhysicalOperatorType::kDropDatabase) {}
};

export struct DropDatabaseOutputState : public OutputState {
    inline explicit DropDatabaseOutputState() : OutputState(PhysicalOperatorType::kDropDatabase) {}
};

// Drop View
export struct DropViewInputState : public InputState {
    inline explicit DropViewInputState() : InputState(PhysicalOperatorType::kDropView) {}
};

export struct DropViewOutputState : public OutputState {
    inline explicit DropViewOutputState() : OutputState(PhysicalOperatorType::kDropView) {}
};

// Command
export struct CommandInputState : public InputState {
    inline explicit CommandInputState() : InputState(PhysicalOperatorType::kCommand) {}
};

export struct CommandOutputState : public OutputState {
    inline explicit CommandOutputState() : OutputState(PhysicalOperatorType::kCommand) {}
};

// Explain
export struct ExplainInputState : public InputState {
    inline explicit ExplainInputState() : InputState(PhysicalOperatorType::kExplain) {}
};

export struct ExplainOutputState : public OutputState {
    inline explicit ExplainOutputState() : OutputState(PhysicalOperatorType::kExplain) {}
};

// Show
export struct ShowInputState : public InputState {
    inline explicit ShowInputState() : InputState(PhysicalOperatorType::kShow) {}
};

export struct ShowOutputState : public OutputState {
    inline explicit ShowOutputState() : OutputState(PhysicalOperatorType::kShow) {}

    Vector<SharedPtr<DataBlock>> output_{};
};

// Flush
export struct FlushInputState : public InputState {
    inline explicit FlushInputState() : InputState(PhysicalOperatorType::kFlush) {}
};

export struct FlushOutputState : public OutputState {
    inline explicit FlushOutputState() : OutputState(PhysicalOperatorType::kFlush) {}
};

// Sink
export enum class SinkStateType {
    kInvalid,
    kMaterialize,
    kResult,
    kMessage,
    kQueue,
};

export struct SinkState {
    inline explicit SinkState(SinkStateType state_type) : state_type_(state_type) {}

    inline void SetPrevState(OutputState *prev_state) { prev_output_state_ = prev_state; }

    inline SinkStateType state_type() const { return state_type_; }

    OutputState *prev_output_state_{};
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

// Source
export enum class SourceStateType { kInvalid, kQueue, kAggregate, kTableScan, kKnnScan, kEmpty };

export struct SourceState {
    inline explicit SourceState(SourceStateType state_type) : state_type_(state_type) {}

    inline void SetNextState(InputState *next_state) {
        next_input_state_ = next_state;
        next_input_state_->input_complete_ptr_ = &complete_;
    }

    bool complete_{false};
    InputState *next_input_state_{};
    SourceStateType state_type_{SourceStateType::kInvalid};
};

export struct QueueSourceState : public SourceState {
    inline explicit QueueSourceState() : SourceState(SourceStateType::kQueue) {}

    inline void SetTotalDataCount(i64 data_count) {
        if (next_input_state_->total_data_count_ == 0) {
            next_input_state_->total_data_count_ = data_count;
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
