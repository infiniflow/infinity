//
// Created by jinhai on 23-9-8.
//

module;

#include <memory>

import stl;
import parser;
import fragment_task;
import infinity_assert;
import infinity_exception;
import operator_state;
import physical_operator;
import physical_operator_type;

import table_scan_function_data;
import knn_scan_data;
import physical_table_scan;
import physical_knn_scan;
import physical_aggregate;

import global_block_id;
import knn_expression;
import value_expression;
import column_expression;
import third_party;
import query_context;
import physical_source;
import physical_sink;
import data_table;
import data_block;
import physical_merge_knn;
import merge_knn_data;

import plan_fragment;

module fragment_context;

namespace infinity {

template <typename InputStateType, typename OutputStateType>
void MakeTaskStateTemplate(UniquePtr<InputState> &input_state, UniquePtr<OutputState> &output_state, PhysicalOperator *physical_op) {
    input_state.reset(static_cast<InputState *>(new InputStateType()));
    output_state.reset(static_cast<OutputState *>(new OutputStateType()));
}

void MakeTableScanState(UniquePtr<InputState> &input_state,
                        UniquePtr<OutputState> &output_state,
                        PhysicalTableScan *physical_table_scan,
                        FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();

    if (source_state->state_type_ != SourceStateType::kTableScan) {
        Error<SchedulerException>("Expect table scan source state", __FILE_NAME__, __LINE__);
    }

    auto *table_scan_source_state = static_cast<TableScanSourceState *>(source_state);

    auto table_scan_input_state = new TableScanInputState();
    table_scan_input_state->table_scan_function_data_ = MakeShared<TableScanFunctionData>(physical_table_scan->GetBlockIndex(),
                                                                                          table_scan_source_state->global_ids_,
                                                                                          physical_table_scan->ColumnIDs());
    input_state.reset(static_cast<InputState *>(table_scan_input_state));

    auto table_scan_output_state = new TableScanOutputState();
    output_state.reset(static_cast<OutputState *>(table_scan_output_state));
}

void MakeKnnScanState(UniquePtr<InputState> &input_state,
                      UniquePtr<OutputState> &output_state,
                      PhysicalKnnScan *physical_knn_scan,
                      FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kKnnScan) {
        Error<SchedulerException>("Expect knn scan source state", __FILE_NAME__, __LINE__);
    }
    auto *knn_scan_source_state = static_cast<KnnScanSourceState *>(source_state);

    if (physical_knn_scan->knn_expressions_.size() != 1) {
        Error<SchedulerException>("Currently, we only support one knn column scenario", __FILE_NAME__, __LINE__);
    }
    KnnExpression *knn_expr = static_cast<KnnExpression *>(physical_knn_scan->knn_expressions_[0].get());
    ColumnExpression *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());

    Vector<SizeT> knn_column_ids = {column_expr->binding().column_idx};

    ValueExpression *limit_expr = static_cast<ValueExpression *>(physical_knn_scan->limit_expression_.get());
    if (limit_expr == nullptr) {
        Error<SchedulerException>("No limit in KNN select is not supported now", __FILE_NAME__, __LINE__);
    }
    i64 topk = limit_expr->GetValue().GetValue<BigIntT>();

    auto knn_scan_input_state = new KnnScanInputState();
    knn_scan_input_state->knn_scan_function_data_ = MakeShared<KnnScanFunctionData>(physical_knn_scan->GetBlockIndex(),
                                                                                    knn_scan_source_state->global_ids_,
                                                                                    physical_knn_scan->ColumnIDs(),
                                                                                    knn_column_ids,
                                                                                    topk,
                                                                                    knn_expr->dimension_,
                                                                                    1,
                                                                                    knn_expr->query_embedding_.ptr,
                                                                                    knn_expr->embedding_data_type_,
                                                                                    knn_expr->distance_type_);
    knn_scan_input_state->knn_scan_function_data_->Init();
    input_state.reset(static_cast<InputState *>(knn_scan_input_state));

    auto knn_scan_output_state = new KnnScanOutputState();
    output_state.reset(static_cast<OutputState *>(knn_scan_output_state));
}

void MakeMergeKnnState(UniquePtr<InputState> &input_state,
                       UniquePtr<OutputState> &output_state,
                       PhysicalMergeKnn *physical_merge_knn,
                       FragmentTask *task) {
    // if (physical_merge_knn->knn_expressions_.size() != 1) {
    //     Error<SchedulerException>("Currently, we only support one knn column scenario", __FILE_NAME__, __LINE__);
    // }
    KnnExpression *knn_expr = static_cast<KnnExpression *>(physical_merge_knn->knn_expressions_[0].get());

    ValueExpression *limit_expr = static_cast<ValueExpression *>(physical_merge_knn->limit_expression_.get());
    if (limit_expr == nullptr) {
        Error<SchedulerException>("No limit in KNN select is not supported now", __FILE_NAME__, __LINE__);
    }
    i64 topk = limit_expr->GetValue().GetValue<BigIntT>();

    auto merge_knn_input_state = new MergeKnnInputState();
    // Set fake parallel number here. It will be set in SetMergeKnnState
    merge_knn_input_state->merge_knn_function_data_ =
        MakeShared<MergeKnnFunctionData>(0, 1, topk, knn_expr->embedding_data_type_, knn_expr->distance_type_, physical_merge_knn->table_ref_);
    input_state.reset(static_cast<InputState *>(merge_knn_input_state));

    auto merge_knn_output_state = new MergeKnnOutputState();
    output_state.reset(static_cast<OutputState *>(merge_knn_output_state));
}

void MakeTaskState(UniquePtr<InputState> &input_state,
                   UniquePtr<OutputState> &output_state,
                   SizeT operator_id,
                   const Vector<PhysicalOperator *> &physical_ops,
                   FragmentTask *task) {
    switch (physical_ops[operator_id]->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            Error<SchedulerException>("Invalid physical operator type", __FILE_NAME__, __LINE__);
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            if (operator_id != physical_ops.size() - 1) {
                Error<SchedulerException>("Table scan operator must be the first operator of the fragment.", __FILE_NAME__, __LINE__);
            }

            if (operator_id == 0) {
                Error<SchedulerException>("Table scan shouldn't be the last operator of the fragment.", __FILE_NAME__, __LINE__);
            }
            auto physical_table_scan = static_cast<PhysicalTableScan *>(physical_ops[operator_id]);
            MakeTableScanState(input_state, output_state, physical_table_scan, task);
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            auto physical_knn_scan = static_cast<PhysicalKnnScan *>(physical_ops[operator_id]);
            MakeKnnScanState(input_state, output_state, physical_knn_scan, task);
            break;
        }
        case PhysicalOperatorType::kAggregate: {
            MakeTaskStateTemplate<AggregateInputState, AggregateOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kParallelAggregate: {
            MakeTaskStateTemplate<ParallelAggregateInputState, ParallelAggregateOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate: {
            MakeTaskStateTemplate<MergeParallelAggregateInputState, MergeParallelAggregateOutputState>(input_state,
                                                                                                       output_state,
                                                                                                       physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kFilter: {
            MakeTaskStateTemplate<FilterInputState, FilterOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kIndexScan: {
            MakeTaskStateTemplate<IndexScanInputState, IndexScanOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }

        case PhysicalOperatorType::kMergeKnn: {
            auto physical_merge_knn = static_cast<PhysicalMergeKnn *>(physical_ops[operator_id]);
            MakeMergeKnnState(input_state, output_state, physical_merge_knn, task);
            break;
        }
        case PhysicalOperatorType::kHash: {
            MakeTaskStateTemplate<HashInputState, HashOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kMergeHash: {
            MakeTaskStateTemplate<MergeHashInputState, MergeHashOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kLimit: {
            MakeTaskStateTemplate<LimitInputState, LimitOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kMergeLimit: {
            MakeTaskStateTemplate<MergeLimitInputState, MergeLimitOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kTop: {
            MakeTaskStateTemplate<TopInputState, TopOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kMergeTop: {
            MakeTaskStateTemplate<MergeTopInputState, MergeTopOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kProjection: {
            MakeTaskStateTemplate<ProjectionInputState, ProjectionOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kSort: {
            MakeTaskStateTemplate<SortInputState, SortOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kMergeSort: {
            MakeTaskStateTemplate<MergeSortInputState, MergeSortOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kDelete: {
            MakeTaskStateTemplate<DeleteInputState, DeleteOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kInsert: {
            MakeTaskStateTemplate<InsertInputState, InsertOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kImport: {
            MakeTaskStateTemplate<ImportInputState, ImportOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kExport: {
            MakeTaskStateTemplate<ExportInputState, ExportOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kCreateTable: {
            MakeTaskStateTemplate<CreateTableInputState, CreateTableOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kCreateIndex: {
            MakeTaskStateTemplate<CreateIndexInputState, CreateIndexOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kCreateCollection: {
            MakeTaskStateTemplate<CreateCollectionInputState, CreateCollectionOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kCreateDatabase: {
            MakeTaskStateTemplate<CreateDatabaseInputState, CreateDatabaseOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kCreateView: {
            MakeTaskStateTemplate<CreateViewInputState, CreateViewOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kDropTable: {
            MakeTaskStateTemplate<DropTableInputState, DropTableOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kDropIndex: {
            MakeTaskStateTemplate<DropIndexInputState, DropIndexOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kDropCollection: {
            MakeTaskStateTemplate<DropCollectionInputState, DropCollectionOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kDropDatabase: {
            MakeTaskStateTemplate<DropDatabaseInputState, DropDatabaseOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kDropView: {
            MakeTaskStateTemplate<DropViewInputState, DropViewOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kCommand: {
            MakeTaskStateTemplate<CommandInputState, CommandOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kExplain: {
            MakeTaskStateTemplate<ExplainInputState, ExplainOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kShow: {
            MakeTaskStateTemplate<ShowInputState, ShowOutputState>(input_state, output_state, physical_ops[operator_id]);
            break;
        }
        case PhysicalOperatorType::kUnionAll:
        case PhysicalOperatorType::kIntersect:
        case PhysicalOperatorType::kExcept:
        case PhysicalOperatorType::kDummyScan:
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kPreparedPlan:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kFlush:
        case PhysicalOperatorType::kSink:
        case PhysicalOperatorType::kSource: {
            Error<SchedulerException>(Format("Not support {} now", PhysicalOperatorToString(physical_ops[operator_id]->operator_type())),
                                      __FILE_NAME__,
                                      __LINE__);
            break;
        }
    }
}

void SetMergeKnnState(MergeKnnInputState &input_state,
                      PhysicalMergeKnn *physical_merge_knn,
                      const Vector<UniquePtr<PlanFragment>> &children_fragment) {
    if (children_fragment.size() != 1) {
        Error<SchedulerException>("Merge Knn child number must 1", __FILE_NAME__, __LINE__);
    }
    auto child_fragment = children_fragment[0]->GetContext();
    if (child_fragment->GetOperators().back()->operator_type() != PhysicalOperatorType::kKnnScan) {
        Error<SchedulerException>("Merge Knn child must be KnnScan", __FILE_NAME__, __LINE__);
    }
    auto real_parallel_count = child_fragment->Tasks().size();
    input_state.merge_knn_function_data_->total_parallel_n_ = real_parallel_count;
}

// Set the state after child is finished.
void SetTaskState(InputState &input_state, PhysicalOperator *physical_op, const Vector<UniquePtr<PlanFragment>> &children_context) {
    switch (physical_op->operator_type()) {
        case PhysicalOperatorType::kMergeKnn: {
            auto physical_merge_knn = static_cast<PhysicalMergeKnn *>(physical_op);
            auto &merge_knn_input = static_cast<MergeKnnInputState &>(input_state);
            SetMergeKnnState(merge_knn_input, physical_merge_knn, children_context);
            break;
        }
        // TODO: add other operator like kMergeAgg
        default: {
            break;
        }
    }
}

void FragmentContext::MakeFragmentContext(QueryContext *query_context,
                                          FragmentContext *parent_context,
                                          PlanFragment *fragment_ptr,
                                          Vector<FragmentTask *> &task_array) {
    Vector<PhysicalOperator *> &fragment_operators = fragment_ptr->GetOperators();
    i64 operator_count = fragment_operators.size();
    if (operator_count < 1) {
        Error<SchedulerException>("No operators in the fragment.", __FILE_NAME__, __LINE__);
    }

    UniquePtr<FragmentContext> fragment_context = nullptr;
    switch (fragment_ptr->GetFragmentType()) {
        case FragmentType::kInvalid: {
            Error<SchedulerException>("Invalid fragment type", __FILE_NAME__, __LINE__);
        }
        case FragmentType::kSerialMaterialize: {
            fragment_context = MakeUnique<SerialMaterializedFragmentCtx>(fragment_ptr, query_context);
            break;
        }
        case FragmentType::kParallelMaterialize: {
            fragment_context = MakeUnique<ParallelMaterializedFragmentCtx>(fragment_ptr, query_context);
            break;
        }
        case FragmentType::kParallelStream: {
            fragment_context = MakeUnique<ParallelStreamFragmentCtx>(fragment_ptr, query_context);
            break;
        }
    }

    // Set parallel size
    i64 parallel_size = static_cast<i64>(query_context->cpu_number_limit());
    fragment_context->CreateTasks(parallel_size, operator_count);

    Vector<UniquePtr<FragmentTask>> &tasks = fragment_context->Tasks();
    i64 real_parallel_size = tasks.size();

    for (i64 operator_id = operator_count - 1; operator_id >= 0; --operator_id) {

        // PhysicalOperator *physical_op = fragment_operators[operator_id];
        for (i64 task_id = 0; task_id < tasks.size(); ++task_id) {
            FragmentTask *task = tasks[task_id].get();

            UniquePtr<InputState> input_state = nullptr;
            UniquePtr<OutputState> output_state = nullptr;

            // build the input and output state of each opeartor
            MakeTaskState(input_state, output_state, operator_id, fragment_operators, task);

            // Connect the input, output state. Connect fragment to its parent if needed
            if (operator_id == operator_count - 1) {
                // first operator, set first operator input to source
                SourceState *source_state = task->source_state_.get();
                source_state->SetNextState(input_state.get());
            } else {
                // not the first operator, set current input to previous output
                auto prev_operator_output = task->operator_output_state_[operator_id + 1].get();
                input_state->ConnectToPrevOutputOpState(prev_operator_output);
            }

            if (operator_id == 0) {
                // Set last operator output as the input of sink operator
                SinkState *sink_state = tasks[task_id]->sink_state_.get();
                sink_state->SetPrevState(output_state.get());

                if (parent_context != nullptr) {
                    // this fragment has parent fragment, which means the sink node of current fragment
                    // will sink data to the parent fragment.
                    switch (sink_state->state_type_) {
                        case SinkStateType::kQueue: {
                            auto *queue_sink_state = static_cast<QueueSinkState *>(sink_state);
                            for (const auto &next_fragment_task : parent_context->Tasks()) {
                                auto *next_fragment_source_state = static_cast<QueueSourceState *>(next_fragment_task->source_state_.get());
                                queue_sink_state->fragment_data_queues_.emplace_back(&next_fragment_source_state->source_queue_);
                            }

                            break;
                        }
                        case SinkStateType::kInvalid: {
                            Error<SchedulerException>("Invalid sink operator state type.", __FILE_NAME__, __LINE__);
                        }
                        default: {
                            break;
                        }
                    }
                }
            }

            auto output_types = fragment_operators[operator_id]->GetOutputTypes();
            if (output_types != nullptr) {
                output_state->data_block_ = DataBlock::Make();
                output_state->data_block_->Init(*output_types);
            }

            task->operator_input_state_[operator_id] = Move(input_state);
            task->operator_output_state_[operator_id] = Move(output_state);
        }
    }

    if (fragment_ptr->HasChild()) {
        // current fragment have children
        for (const auto &child_fragment : fragment_ptr->Children()) {
            FragmentContext::MakeFragmentContext(query_context, fragment_context.get(), child_fragment.get(), task_array);
        }
    }

    for (i64 task_id = 0; task_id < tasks.size(); task_id++) {
        FragmentTask *task = tasks[task_id].get();
        auto &first_input_state = *task->operator_input_state_.back();
        SetTaskState(first_input_state, fragment_operators.back(), fragment_ptr->Children());
    }

    for (const auto &task : tasks) {
        task_array.emplace_back(task.get());
    }

    fragment_ptr->SetContext(Move(fragment_context));
}

FragmentContext::FragmentContext(PlanFragment *fragment_ptr, QueryContext *query_context)
    : fragment_ptr_(fragment_ptr), fragment_type_(fragment_ptr->GetFragmentType()), query_context_(query_context){};

Vector<PhysicalOperator *> &FragmentContext::GetOperators() { return fragment_ptr_->GetOperators(); }

PhysicalSink *FragmentContext::GetSinkOperator() const { return fragment_ptr_->GetSinkNode(); }

PhysicalSource *FragmentContext::GetSourceOperator() const { return fragment_ptr_->GetSourceNode(); }

// Allocate tasks for the fragment and determine the sink and source
void FragmentContext::CreateTasks(i64 cpu_count, i64 operator_count) {
    i64 parallel_count = cpu_count;
    PhysicalOperator *first_operator = this->GetOperators().back();
    switch (first_operator->operator_type()) {
        case PhysicalOperatorType::kTableScan: {
            auto *table_scan_operator = static_cast<PhysicalTableScan *>(first_operator);
            parallel_count = Min(parallel_count, (i64)(table_scan_operator->BlockEntryCount()));
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            auto *knn_scan_operator = static_cast<PhysicalKnnScan *>(first_operator);
            parallel_count = Min(parallel_count, (i64)(knn_scan_operator->BlockEntryCount()));
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kMergeKnn:
        case PhysicalOperatorType::kProjection: {
            // Serial Materialize
            parallel_count = 1;
            break;
        }
        default: {
            break;
        }
    }

    switch (fragment_type_) {
        case FragmentType::kInvalid: {
            Error<SchedulerException>("Invalid fragment type", __FILE_NAME__, __LINE__);
        }
        case FragmentType::kSerialMaterialize: {
            UniqueLock<std::mutex> locker(locker_);
            tasks_.reserve(parallel_count);
            tasks_.emplace_back(MakeUnique<FragmentTask>(this, 0, operator_count));
            IncreaseTask();
            break;
        }
        case FragmentType::kParallelMaterialize:
        case FragmentType::kParallelStream: {
            UniqueLock<std::mutex> locker(locker_);
            tasks_.reserve(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_.emplace_back(MakeUnique<FragmentTask>(this, task_id, operator_count));
                IncreaseTask();
            }
            break;
        }
    }

    // Determine which type of source state.
    switch (first_operator->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            Error<SchedulerException>("Unexpected operator type", __FILE_NAME__, __LINE__);
        }
        case PhysicalOperatorType::kAggregate: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in parallel materialized fragment", PhysicalOperatorToString(first_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != parallel_count) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            // Partition the hash range to each source state
            auto *aggregate_operator = (PhysicalAggregate *)first_operator;
            Vector<HashRange> hash_range = aggregate_operator->GetHashRanges(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<AggregateSourceState>(hash_range[task_id].start_, hash_range[task_id].end_);
            }
            break;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kProjection:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort:
        case PhysicalOperatorType::kDelete: {
            Error<SchedulerException>(
                Format("{} shouldn't be the first operator of the fragment", PhysicalOperatorToString(first_operator->operator_type())),
                __FILE_NAME__,
                __LINE__);
        }
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeKnn: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should be serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != 1) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            tasks_[0]->source_state_ = MakeUnique<QueueSourceState>();
            break;
        }
        case PhysicalOperatorType::kUnionAll:
        case PhysicalOperatorType::kIntersect:
        case PhysicalOperatorType::kExcept:
        case PhysicalOperatorType::kDummyScan:
        case PhysicalOperatorType::kIndexScan:
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kPreparedPlan:
        case PhysicalOperatorType::kFlush: {
            Error<SchedulerException>(Format("Not support {} now", PhysicalOperatorToString(first_operator->operator_type())),
                                      __FILE_NAME__,
                                      __LINE__);
        }
        case PhysicalOperatorType::kTableScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kParallelStream) {
                Error<SchedulerException>(
                    Format("{} should in parallel materialized/stream fragment", PhysicalOperatorToString(first_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != parallel_count) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            // Partition the hash range to each source state
            auto *table_scan_operator = (PhysicalTableScan *)first_operator;
            Vector<SharedPtr<Vector<GlobalBlockID>>> blocks_group = table_scan_operator->PlanBlockEntries(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<TableScanSourceState>(blocks_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kParallelStream) {
                Error<SchedulerException>(
                    Format("{} should in parallel materialized/stream fragment", PhysicalOperatorToString(first_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != parallel_count) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            // Partition the hash range to each source state
            auto *knn_scan_operator = (PhysicalKnnScan *)first_operator;
            Vector<SharedPtr<Vector<GlobalBlockID>>> blocks_group = knn_scan_operator->PlanBlockEntries(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<KnnScanSourceState>(blocks_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateIndex:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropIndex:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView:
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kShow: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != 1) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            tasks_[0]->source_state_ = MakeUnique<EmptySourceState>();
            break;
        }

        default: {
            Error<SchedulerException>(Format("Unexpected operator type: {}", PhysicalOperatorToString(first_operator->operator_type())),
                                      __FILE_NAME__,
                                      __LINE__);
        }
    }

    // Determine which type of the sink state.
    PhysicalOperator *last_operator = this->GetOperators().front();
    switch (last_operator->operator_type()) {

        case PhysicalOperatorType::kInvalid: {
            Error<SchedulerException>("Unexpected operator type", __FILE_NAME__, __LINE__);
        }
        case PhysicalOperatorType::kAggregate:
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in parallel materialized fragment", PhysicalOperatorToString(last_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != parallel_count) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<MaterializeSinkState>();
            }
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeKnn:
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kShow: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != 1) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            tasks_[0]->sink_state_ = MakeUnique<MaterializeSinkState>();
            MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
            sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
            sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in parallel materialized/stream fragment", PhysicalOperatorToString(last_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != parallel_count) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<QueueSinkState>();
                //                QueueSinkState* sink_state_ptr = static_cast<QueueSinkState*>(tasks_[task_id]->sink_state_.get());
            }
            break;
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kIndexScan: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in parallel materialized/stream fragment", PhysicalOperatorToString(last_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != parallel_count) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<MaterializeSinkState>();
                MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[task_id]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            }
            break;
        }
        case PhysicalOperatorType::kProjection: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                if (tasks_.size() != 1) {
                    Error<SchedulerException>("SerialMaterialize type fragment should only have 1 task.", __FILE_NAME__, __LINE__);
                }

                tasks_[0]->sink_state_ = MakeUnique<MaterializeSinkState>();
                MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            } else {
                if (tasks_.size() != parallel_count) {
                    Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                              __FILE_NAME__,
                                              __LINE__);
                }

                for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                    tasks_[task_id]->sink_state_ = MakeUnique<MaterializeSinkState>();
                    MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[task_id]->sink_state_.get());
                    sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                    sink_state_ptr->column_names_ = last_operator->GetOutputNames();
                }
            }
            break;
        }
        case PhysicalOperatorType::kUnionAll:
        case PhysicalOperatorType::kIntersect:
        case PhysicalOperatorType::kExcept:
        case PhysicalOperatorType::kDummyScan:
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kPreparedPlan:
        case PhysicalOperatorType::kFlush: {
            Error<SchedulerException>(Format("Not support {} now", PhysicalOperatorToString(last_operator->operator_type())),
                                      __FILE_NAME__,
                                      __LINE__);
        }
        case PhysicalOperatorType::kDelete:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != 1) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            tasks_[0]->sink_state_ = MakeUnique<MessageSinkState>();
            break;
        }
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateIndex:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropIndex:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                Error<SchedulerException>(
                    Format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())),
                    __FILE_NAME__,
                    __LINE__);
            }

            if (tasks_.size() != 1) {
                Error<SchedulerException>(Format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())),
                                          __FILE_NAME__,
                                          __LINE__);
            }

            tasks_[0]->sink_state_ = MakeUnique<ResultSinkState>();
            break;
        }
        default: {
            Error<SchedulerException>(Format("Unexpected operator type: {}", PhysicalOperatorToString(last_operator->operator_type())),
                                      __FILE_NAME__,
                                      __LINE__);
        }
    }
}

SharedPtr<DataTable> SerialMaterializedFragmentCtx::GetResultInternal() {

    // Only one sink state
    if (tasks_.size() != 1) {
        Error<SchedulerException>("There should be one sink state in serial materialized fragment", __FILE_NAME__, __LINE__);
    }

    switch (tasks_[0]->sink_state_->state_type()) {
        case SinkStateType::kInvalid: {
            Error<SchedulerException>("Invalid sink state type", __FILE_NAME__, __LINE__);
            break;
        }
        case SinkStateType::kMaterialize: {
            auto *materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());

            Vector<SharedPtr<ColumnDef>> column_defs;
            SizeT column_count = materialize_sink_state->column_names_->size();
            column_defs.reserve(column_count);
            for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
                column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                               materialize_sink_state->column_types_->at(col_idx),
                                                               materialize_sink_state->column_names_->at(col_idx),
                                                               HashSet<ConstraintType>()));
            }

            SharedPtr<DataTable> result_table = DataTable::MakeResultTable(column_defs);
            result_table->data_blocks_ = Move(materialize_sink_state->data_block_array_);
            return result_table;
        }
        case SinkStateType::kResult: {
            auto *result_sink_state = static_cast<ResultSinkState *>(tasks_[0]->sink_state_.get());
            if (result_sink_state->error_message_ == nullptr) {
                SharedPtr<DataTable> result_table = DataTable::MakeResultTable({result_sink_state->result_def_});
                return result_table;
            }
            Error<ExecutorException>(*result_sink_state->error_message_, __FILE_NAME__, __LINE__);
        }
        case SinkStateType::kMessage: {
            auto *message_sink_state = static_cast<MessageSinkState *>(tasks_[0]->sink_state_.get());
            if (message_sink_state->error_message_ != nullptr) {
                throw Exception(*message_sink_state->error_message_);
            }

            if (message_sink_state->message_ == nullptr) {
                Error<SchedulerException>("No response message", __FILE_NAME__, __LINE__);
            }

            SharedPtr<DataTable> result_table = DataTable::MakeEmptyResultTable();
            result_table->SetResultMsg(Move(message_sink_state->message_));
            return result_table;
        }
        case SinkStateType::kQueue: {
            Error<SchedulerException>("Can't get result from Queue sink type.", __FILE_NAME__, __LINE__);
        }
    }
    Error<SchedulerException>("Unreachable", __FILE_NAME__, __LINE__);
}

SharedPtr<DataTable> ParallelMaterializedFragmentCtx::GetResultInternal() {

    SharedPtr<DataTable> result_table = nullptr;

    auto *first_materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
    if (first_materialize_sink_state->error_message_ != nullptr) {
        Error<ExecutorException>(*first_materialize_sink_state->error_message_, __FILE_NAME__, __LINE__);
    }

    Vector<SharedPtr<ColumnDef>> column_defs;
    SizeT column_count = first_materialize_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                       first_materialize_sink_state->column_types_->at(col_idx),
                                                       first_materialize_sink_state->column_names_->at(col_idx),
                                                       HashSet<ConstraintType>()));
    }

    for (const auto &task : tasks_) {
        if (task->sink_state_->state_type() != SinkStateType::kMaterialize) {
            Error<SchedulerException>("Parallel materialized fragment will only have common sink stte", __FILE_NAME__, __LINE__);
        }

        auto *materialize_sink_state = static_cast<MaterializeSinkState *>(task->sink_state_.get());
        if (materialize_sink_state->error_message_ != nullptr) {
            Error<ExecutorException>(*materialize_sink_state->error_message_, __FILE_NAME__, __LINE__);
        }

        if (result_table == nullptr) {
            result_table = DataTable::MakeResultTable(column_defs);
        }

        for (const auto &result_data_block : materialize_sink_state->data_block_array_) {
            result_table->Append(result_data_block);
        }
    }

    return result_table;
}

SharedPtr<DataTable> ParallelStreamFragmentCtx::GetResultInternal() {
    SharedPtr<DataTable> result_table = nullptr;

    auto *first_materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());

    Vector<SharedPtr<ColumnDef>> column_defs;
    SizeT column_count = first_materialize_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                       first_materialize_sink_state->column_types_->at(col_idx),
                                                       first_materialize_sink_state->column_names_->at(col_idx),
                                                       HashSet<ConstraintType>()));
    }

    for (const auto &task : tasks_) {
        if (task->sink_state_->state_type() != SinkStateType::kMaterialize) {
            Error<SchedulerException>("Parallel materialized fragment will only have common sink state", __FILE_NAME__, __LINE__);
        }

        auto *materialize_sink_state = static_cast<MaterializeSinkState *>(task->sink_state_.get());

        if (result_table == nullptr) {
            result_table = DataTable::MakeResultTable(column_defs);
        }

        for (const auto &result_data_block : materialize_sink_state->data_block_array_) {
            result_table->Append(result_data_block);
        }
    }

    return result_table;
}

} // namespace infinity
