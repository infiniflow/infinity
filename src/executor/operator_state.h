//
// Created by JinHai on 2022/9/15.
//

#pragma once


#include "storage/table.h"
#include "physical_operator_type.h"
#include "scheduler/fragment_data_queue.h"
#include "storage/meta/entry/segment_entry.h"
#include "function/table/table_scan_data.h"
#include "function/table/knn_scan_data.h"

namespace infinity {

struct OutputState {
    inline explicit
    OutputState(PhysicalOperatorType operator_type) : operator_type_(operator_type) {}

    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    SharedPtr<DataBlock> data_block_{};
    UniquePtr<String> error_message_{};
};

struct InputState {
    inline explicit
    InputState(PhysicalOperatorType operator_type) : operator_type_(operator_type) {}

    PhysicalOperatorType operator_type_{PhysicalOperatorType::kInvalid};
    DataBlock* input_data_block_{};
};

// Aggregate
struct AggregateInputState : public InputState {
    inline explicit
    AggregateInputState() : InputState(PhysicalOperatorType::kAggregate) {}
};

struct AggregateOutputState : public OutputState {
    inline explicit
    AggregateOutputState() : OutputState(PhysicalOperatorType::kAggregate) {}
};

// Merge Parallel Aggregate
struct MergeParallelAggregateInputState : public InputState {
    inline explicit
    MergeParallelAggregateInputState() : InputState(PhysicalOperatorType::kMergeParallelAggregate) {}
};

struct MergeParallelAggregateOutputState : public OutputState {
    inline explicit
    MergeParallelAggregateOutputState() : OutputState(PhysicalOperatorType::kMergeParallelAggregate) {}
};

// Parallel Aggregate
struct ParallelAggregateInputState : public InputState {
    inline explicit
    ParallelAggregateInputState() : InputState(PhysicalOperatorType::kParallelAggregate) {}
};

struct ParallelAggregateOutputState : public OutputState {
    inline explicit
    ParallelAggregateOutputState() : OutputState(PhysicalOperatorType::kParallelAggregate) {}
};

// UnionAll
struct UnionAllInputState : public InputState {
    inline explicit
    UnionAllInputState() : InputState(PhysicalOperatorType::kUnionAll) {}
};

struct UnionAllOutputState : public OutputState {
    inline explicit
    UnionAllOutputState() : OutputState(PhysicalOperatorType::kUnionAll) {}
};

// TableScan
struct TableScanInputState : public InputState {
    inline explicit
    TableScanInputState() : InputState(PhysicalOperatorType::kTableScan) {}

    UniquePtr<TableScanFunctionData> table_scan_function_data_{};
};

struct TableScanOutputState : public OutputState {
    inline explicit
    TableScanOutputState() : OutputState(PhysicalOperatorType::kTableScan) {}
};

// KnnScan
struct KnnScanInputState : public InputState {
    inline explicit
    KnnScanInputState() : InputState(PhysicalOperatorType::kTableScan) {}

    UniquePtr<KnnScanFunctionData> knn_scan_function_data_{};
};

struct KnnScanOutputState : public OutputState {
    inline explicit
    KnnScanOutputState() : OutputState(PhysicalOperatorType::kTableScan) {}
};

// Merge Knn
struct MergeKnnInputState : public InputState {
    inline explicit
    MergeKnnInputState() : InputState(PhysicalOperatorType::kMergeKnn) {}
};

struct MergeKnnOutputState : public OutputState {
    inline explicit
    MergeKnnOutputState() : OutputState(PhysicalOperatorType::kMergeKnn) {}
};

// Filter
struct FilterInputState : public InputState {
    inline explicit
    FilterInputState() : InputState(PhysicalOperatorType::kFilter) {}
};

struct FilterOutputState : public OutputState {
    inline explicit
    FilterOutputState() : OutputState(PhysicalOperatorType::kFilter) {}
};

// IndexScan
struct IndexScanInputState : public InputState {
    inline explicit
    IndexScanInputState() : InputState(PhysicalOperatorType::kIndexScan) {}
};

struct IndexScanOutputState : public OutputState {
    inline explicit
    IndexScanOutputState() : OutputState(PhysicalOperatorType::kIndexScan) {}
};

// Hash
struct HashInputState : public InputState {
    inline explicit
    HashInputState() : InputState(PhysicalOperatorType::kHash) {}
};

struct HashOutputState : public OutputState {
    inline explicit
    HashOutputState() : OutputState(PhysicalOperatorType::kHash) {}
};

// Merge Hash
struct MergeHashInputState : public InputState {
    inline explicit
    MergeHashInputState() : InputState(PhysicalOperatorType::kMergeHash) {}
};

struct MergeHashOutputState : public OutputState {
    inline explicit
    MergeHashOutputState() : OutputState(PhysicalOperatorType::kMergeHash) {}
};

// Hash Join
struct HashJoinInputState : public InputState {
    inline explicit
    HashJoinInputState() : InputState(PhysicalOperatorType::kJoinHash) {}
};

struct HashJoinOutputState : public OutputState {
    inline explicit
    HashJoinOutputState() : OutputState(PhysicalOperatorType::kJoinHash) {}
};

// Nested Loop
struct NestedLoopInputState : public InputState {
    inline explicit
    NestedLoopInputState() : InputState(PhysicalOperatorType::kJoinNestedLoop) {}
};

struct NestedLoopOutputState : public OutputState {
    inline explicit
    NestedLoopOutputState() : OutputState(PhysicalOperatorType::kJoinNestedLoop) {}
};

// Merge Join
struct MergeJoinInputState : public InputState {
    inline explicit
    MergeJoinInputState() : InputState(PhysicalOperatorType::kJoinMerge) {}
};

struct MergeJoinOutputState : public OutputState {
    inline explicit
    MergeJoinOutputState() : OutputState(PhysicalOperatorType::kJoinMerge) {}
};

// Index Join
struct IndexJoinInputState : public InputState {
    inline explicit
    IndexJoinInputState() : InputState(PhysicalOperatorType::kJoinIndex) {}
};

struct IndexJoinOutputState : public OutputState {
    inline explicit
    IndexJoinOutputState() : OutputState(PhysicalOperatorType::kJoinIndex) {}
};

// Cross Product
struct CrossProductInputState : public InputState {
    inline explicit
    CrossProductInputState() : InputState(PhysicalOperatorType::kCrossProduct) {}
};

struct CrossProductOutputState : public OutputState {
    inline explicit
    CrossProductOutputState() : OutputState(PhysicalOperatorType::kCrossProduct) {}
};

// Limit
struct LimitInputState : public InputState {
    inline explicit
    LimitInputState() : InputState(PhysicalOperatorType::kLimit) {}
};

struct LimitOutputState : public OutputState {
    inline explicit
    LimitOutputState() : OutputState(PhysicalOperatorType::kLimit) {}
};

// Merge Limit
struct MergeLimitInputState : public InputState {
    inline explicit
    MergeLimitInputState() : InputState(PhysicalOperatorType::kMergeLimit) {}
};

struct MergeLimitOutputState : public OutputState {
    inline explicit
    MergeLimitOutputState() : OutputState(PhysicalOperatorType::kMergeLimit) {}
};

// Merge Top
struct MergeTopInputState : public InputState {
    inline explicit
    MergeTopInputState() : InputState(PhysicalOperatorType::kMergeTop) {}
};

struct MergeTopOutputState : public OutputState {
    inline explicit
    MergeTopOutputState() : OutputState(PhysicalOperatorType::kMergeTop) {}
};

// Top
struct TopInputState : public InputState {
    inline explicit
    TopInputState() : InputState(PhysicalOperatorType::kTop) {}
};

struct TopOutputState : public OutputState {
    inline explicit
    TopOutputState() : OutputState(PhysicalOperatorType::kTop) {}
};

// Projection
struct ProjectionInputState : public InputState {
    inline explicit
    ProjectionInputState() : InputState(PhysicalOperatorType::kProjection) {}
};

struct ProjectionOutputState : public OutputState {
    inline explicit
    ProjectionOutputState() : OutputState(PhysicalOperatorType::kProjection) {}
};

// Sort
struct SortInputState : public InputState {
    inline explicit
    SortInputState() : InputState(PhysicalOperatorType::kSort) {}
};

struct SortOutputState : public OutputState {
    inline explicit
    SortOutputState() : OutputState(PhysicalOperatorType::kSort) {}
};

// Merge Sort
struct MergeSortInputState : public InputState {
    inline explicit
    MergeSortInputState() : InputState(PhysicalOperatorType::kMergeSort) {}
};

struct MergeSortOutputState : public OutputState {
    inline explicit
    MergeSortOutputState() : OutputState(PhysicalOperatorType::kMergeSort) {}
};

// Delete
struct DeleteInputState : public InputState {
    inline explicit
    DeleteInputState() : InputState(PhysicalOperatorType::kDelete) {}
};

struct DeleteOutputState : public OutputState {
    inline explicit
    DeleteOutputState() : OutputState(PhysicalOperatorType::kDelete) {}
};

// Update
struct UpdateInputState : public InputState {
    inline explicit
    UpdateInputState() : InputState(PhysicalOperatorType::kUpdate) {}
};

struct UpdateOutputState : public OutputState {
    inline explicit
    UpdateOutputState() : OutputState(PhysicalOperatorType::kUpdate) {}
};

// Insert
struct InsertInputState : public InputState {
    inline explicit
    InsertInputState() : InputState(PhysicalOperatorType::kInsert) {}
};

struct InsertOutputState : public OutputState {
    inline explicit
    InsertOutputState() : OutputState(PhysicalOperatorType::kInsert) {}
};

// Import
struct ImportInputState : public InputState {
    inline explicit
    ImportInputState() : InputState(PhysicalOperatorType::kImport) {}
};

struct ImportOutputState : public OutputState {
    inline explicit
    ImportOutputState() : OutputState(PhysicalOperatorType::kImport) {}

    Vector<SharedPtr<DataBlock>> output_{};
    SharedPtr<TableDef> table_def_{};
    // For insert, update, delete, update
    UniquePtr<String> result_msg_{};
};

// Export
struct ExportInputState : public InputState {
    inline explicit
    ExportInputState() : InputState(PhysicalOperatorType::kExport) {}
};

struct ExportOutputState : public OutputState {
    inline explicit
    ExportOutputState() : OutputState(PhysicalOperatorType::kExport) {}
};

// Alter
struct AlterInputState : public InputState {
    inline explicit
    AlterInputState() : InputState(PhysicalOperatorType::kAlter) {}
};

struct AlterOutputState : public OutputState {
    inline explicit
    AlterOutputState() : OutputState(PhysicalOperatorType::kAlter) {}
};

// Create Table
struct CreateTableInputState : public InputState {
    inline explicit
    CreateTableInputState() : InputState(PhysicalOperatorType::kCreateTable) {}
};

struct CreateTableOutputState : public OutputState {
    inline explicit
    CreateTableOutputState() : OutputState(PhysicalOperatorType::kCreateTable) {}
};

// Create Collection
struct CreateCollectionInputState : public InputState {
    inline explicit
    CreateCollectionInputState() : InputState(PhysicalOperatorType::kCreateCollection) {}
};

struct CreateCollectionOutputState : public OutputState {
    inline explicit
    CreateCollectionOutputState() : OutputState(PhysicalOperatorType::kCreateCollection) {}
};

// Create Database
struct CreateDatabaseInputState : public InputState {
    inline explicit
    CreateDatabaseInputState() : InputState(PhysicalOperatorType::kCreateDatabase) {}
};

struct CreateDatabaseOutputState : public OutputState {
    inline explicit
    CreateDatabaseOutputState() : OutputState(PhysicalOperatorType::kCreateDatabase) {}
};

// Create View
struct CreateViewInputState : public InputState {
    inline explicit
    CreateViewInputState() : InputState(PhysicalOperatorType::kCreateView) {}
};

struct CreateViewOutputState : public OutputState {
    inline explicit
    CreateViewOutputState() : OutputState(PhysicalOperatorType::kCreateView) {}
};

// Drop Table
struct DropTableInputState : public InputState {
    inline explicit
    DropTableInputState() : InputState(PhysicalOperatorType::kDropTable) {}
};

struct DropTableOutputState : public OutputState {
    inline explicit
    DropTableOutputState() : OutputState(PhysicalOperatorType::kDropTable) {}
};

// Drop Collection
struct DropCollectionInputState : public InputState {
    inline explicit
    DropCollectionInputState() : InputState(PhysicalOperatorType::kDropCollection) {}
};

struct DropCollectionOutputState : public OutputState {
    inline explicit
    DropCollectionOutputState() : OutputState(PhysicalOperatorType::kDropCollection) {}
};

// Drop Database
struct DropDatabaseInputState : public InputState {
    inline explicit
    DropDatabaseInputState() : InputState(PhysicalOperatorType::kDropDatabase) {}
};

struct DropDatabaseOutputState : public OutputState {
    inline explicit
    DropDatabaseOutputState() : OutputState(PhysicalOperatorType::kDropDatabase) {}
};

// Drop View
struct DropViewInputState : public InputState {
    inline explicit
    DropViewInputState() : InputState(PhysicalOperatorType::kDropView) {}
};

struct DropViewOutputState : public OutputState {
    inline explicit
    DropViewOutputState() : OutputState(PhysicalOperatorType::kDropView) {}
};

// Explain
struct ExplainInputState : public InputState {
    inline explicit
    ExplainInputState() : InputState(PhysicalOperatorType::kExplain) {}
};

struct ExplainOutputState : public OutputState {
    inline explicit
    ExplainOutputState() : OutputState(PhysicalOperatorType::kExplain) {}
};

// Show
struct ShowInputState : public InputState {
    inline explicit
    ShowInputState() : InputState(PhysicalOperatorType::kShow) {}
};

struct ShowOutputState : public OutputState {
    inline explicit
    ShowOutputState() : OutputState(PhysicalOperatorType::kShow) {}

    Vector<SharedPtr<DataBlock>> output_{};
};

// Flush
struct FlushInputState : public InputState {
    inline explicit
    FlushInputState() : InputState(PhysicalOperatorType::kFlush) {}
};

struct FlushOutputState : public OutputState {
    inline explicit
    FlushOutputState() : OutputState(PhysicalOperatorType::kFlush) {}
};

// Sink
enum class SinkStateType {
    kInvalid,
    kGeneral,
    kResult,
    kMessage,
};


struct SinkState {
    inline explicit
    SinkState(SinkStateType state_type) : state_type_(state_type) {}

    inline void
    SetPrevState(OutputState* prev_state) {
        prev_output_state_ = prev_state;
    }

    inline SinkStateType
    state_type() const {
        return state_type_;
    }

    OutputState* prev_output_state_{};
    SinkStateType state_type_{SinkStateType::kInvalid};
    UniquePtr<String> error_message_{};
};

struct GeneralSinkState : public SinkState {
    inline explicit
    GeneralSinkState() : SinkState(SinkStateType::kGeneral) {}

    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<String>> column_names_{};
    Vector<SharedPtr<DataBlock>> data_block_array_{};
};

struct ResultSinkState : public SinkState {
    inline explicit
    ResultSinkState() : SinkState(SinkStateType::kResult) {}

    SharedPtr<ColumnDef> result_def_{};
};

struct MessageSinkState : public SinkState {
    inline explicit
    MessageSinkState() : SinkState(SinkStateType::kMessage) {}

    UniquePtr<String> message_{};
};

// Source
enum class SourceStateType {
    kInvalid,
    kCommon,
    kAggregate,
    kTableScan,
    kKnnScan,
    kEmpty
};

struct SourceState {
    inline explicit
    SourceState(SourceStateType state_type) : state_type_(state_type) {}

    inline void
    SetNextState(InputState* next_state) {
        next_input_state_ = next_state;
    }

    InputState* next_input_state_{};
    SourceStateType state_type_{SourceStateType::kInvalid};
};

struct CommonSourceState : public SourceState {
    inline explicit
    CommonSourceState() : SourceState(SourceStateType::kCommon) {}

    FragmentDataQueue source_queue_;
};

struct AggregateSourceState : public SourceState {
    explicit
    AggregateSourceState(i64 hash_start, i64 hash_end)
            : SourceState(SourceStateType::kAggregate), hash_start_(hash_start), hash_end_(hash_end) {}


    inline SharedPtr<DataBlock>
    GetNextDataBlock() {
        return nullptr;
    }

    i64 hash_start_{};
    i64 hash_end_{};

    FragmentDataQueue source_queue_;
};

struct TableScanSourceState : public SourceState {
    explicit
    TableScanSourceState(SharedPtr<Vector<u64>> segment_ids)
            : SourceState(SourceStateType::kTableScan), segment_entry_ids_(std::move(segment_ids)) {}

    SharedPtr<Vector<u64>> segment_entry_ids_;
};

struct KnnScanSourceState : public SourceState {
    explicit
    KnnScanSourceState(SharedPtr<Vector<u64>> segment_ids)
            : SourceState(SourceStateType::kKnnScan), segment_entry_ids_(std::move(segment_ids)) {}

    SharedPtr<Vector<u64>> segment_entry_ids_;
};

struct EmptySourceState : public SourceState {
    explicit
    EmptySourceState() : SourceState(SourceStateType::kEmpty) {}
};

}

