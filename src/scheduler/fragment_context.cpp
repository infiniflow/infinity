//
// Created by jinhai on 23-9-8.
//

#include "fragment_context.h"
#include "src/executor/fragment/plan_fragment.h"
#include "fragment_data_queue.h"
#include "executor/operator/physical_aggregate.h"
#include "executor/operator/physical_table_scan.h"
#include "executor/operator/physical_knn_scan.h"
#include "expression/column_expression.h"
#include "expression/knn_expression.h"
#include "expression/value_expression.h"

namespace infinity {

template<typename InputStateType, typename OutputStateType>
void
BuildSerialTaskStateTemplate(Vector<PhysicalOperator*>& fragment_operators,
                             Vector<UniquePtr<FragmentTask>>& tasks,
                             i64 operator_id,
                             i64 operator_count) {
    if(tasks.size() != 1) {
        SchedulerError("Serial fragment type will only have one task")
    }

    FragmentTask* task_ptr = tasks.back().get();
    SourceState* source_ptr = tasks.back()->source_state_.get();
    SinkState* sink_ptr = tasks.back()->sink_state_.get();

    task_ptr->operator_input_state_[operator_id] = MakeUnique<InputStateType>();
    auto input_state = (InputStateType*)(task_ptr->operator_input_state_[operator_id].get());
    task_ptr->operator_output_state_[operator_id] = MakeUnique<OutputStateType>();
    auto output_state = (OutputStateType*)(task_ptr->operator_output_state_[operator_id].get());
    output_state->data_block_ = DataBlock::Make();
    output_state->data_block_->Init(*fragment_operators[operator_id]->GetOutputTypes());
    if(operator_id == 0) {
        // First operator must get data source node
        source_ptr->SetNextState(input_state);
    }

    if(operator_id > 0 && operator_id < operator_count) {
        OutputState* prev_output_state = task_ptr->operator_output_state_[operator_id - 1].get();
        input_state->input_data_block_ = prev_output_state->data_block_.get();
    }

    if(operator_id == operator_count - 1) {
        // Last operator
        sink_ptr->SetPrevState(output_state);
    }
}

template<>
void
BuildSerialTaskStateTemplate<ImportInputState, ImportOutputState>(Vector<PhysicalOperator*>& fragment_operators,
                                                                  Vector<UniquePtr<FragmentTask>>& tasks,
                                                                  i64 operator_id,
                                                                  i64 operator_count) {
    if(tasks.size() != 1) {
        SchedulerError("Serial fragment type will only have one task")
    }

    FragmentTask* task_ptr = tasks.back().get();
    SourceState* source_ptr = tasks.back()->source_state_.get();
    SinkState* sink_ptr = tasks.back()->sink_state_.get();

    task_ptr->operator_input_state_[operator_id] = MakeUnique<ImportInputState>();
    auto input_state = (ImportInputState*)(task_ptr->operator_input_state_[operator_id].get());
    task_ptr->operator_output_state_[operator_id] = MakeUnique<ImportOutputState>();
    auto output_state = (ImportOutputState*)(task_ptr->operator_output_state_[operator_id].get());
//    output_state->data_block_ = DataBlock::Make();
//    output_state->data_block_->Init(*fragment_operators[operator_id]->GetOutputTypes());
    if(operator_id == operator_count - 1) {
        // First operator must get data source node
        source_ptr->SetNextState(input_state);
    }

    if(operator_id >= 0 && operator_id < operator_count - 1) {
        OutputState* prev_output_state = task_ptr->operator_output_state_[operator_id + 1].get();
        input_state->input_data_block_ = prev_output_state->data_block_.get();
    }

    if(operator_id == 0) {
        // Last operator
        sink_ptr->SetPrevState(output_state);
    }
}

template<typename InputStateType, typename OutputStateType>
void
BuildParallelTaskStateTemplate(Vector<PhysicalOperator*>& fragment_operators,
                               Vector<UniquePtr<FragmentTask>>& tasks,
                               i64 operator_id,
                               i64 operator_count,
                               i64 real_parallel_size) {
    for(i64 task_id = 0; task_id < real_parallel_size; ++task_id) {
        FragmentTask* task_ptr = tasks[task_id].get();
        SourceState* source_ptr = tasks[task_id]->source_state_.get();
        SinkState* sink_ptr = tasks[task_id]->sink_state_.get();

        task_ptr->operator_input_state_[operator_id] = MakeUnique<InputStateType>();
        auto input_state = (InputStateType*)(task_ptr->operator_input_state_[operator_id].get());
        task_ptr->operator_output_state_[operator_id] = MakeUnique<OutputStateType>();
        auto output_state = (OutputStateType*)(task_ptr->operator_output_state_[operator_id].get());
        output_state->data_block_ = DataBlock::Make();
        output_state->data_block_->Init(*fragment_operators[operator_id]->GetOutputTypes());

        if(operator_id == operator_count - 1) {
            // First operator must get data source node
            source_ptr->SetNextState(input_state);
        }

        if(operator_id >= 0 && operator_id < operator_count - 1) {
            OutputState* prev_output_state = task_ptr->operator_output_state_[operator_id + 1].get();
            input_state->input_data_block_ = prev_output_state->data_block_.get();
        }

        if(operator_id == 0) {
            // Last operator
            sink_ptr->SetPrevState(output_state);
        }
    }
}

template<>
void
BuildParallelTaskStateTemplate<TableScanInputState, TableScanOutputState>(Vector<PhysicalOperator*>& fragment_operators,
                                                                          Vector<UniquePtr<FragmentTask>>& tasks,
                                                                          i64 operator_id,
                                                                          i64 operator_count,
                                                                          i64 real_parallel_size) {

    // TableScan must be the first operator of the fragment
    if(operator_id != operator_count - 1) {
        SchedulerError("Table scan operator must be the first operator of the fragment.")
    }

    if(operator_id == 0) {
        SchedulerError("Table scan shouldn't be the last operator of the fragment.")
    }

    PhysicalOperator* physical_op = fragment_operators[operator_id];
    if(physical_op->operator_type() != PhysicalOperatorType::kTableScan) {
        SchedulerError("Expect table scan physical operator")
    }
    auto* physical_table_scan = static_cast<PhysicalTableScan*>(physical_op);

    for(i64 task_id = 0; task_id < real_parallel_size; ++task_id) {
        FragmentTask* task_ptr = tasks[task_id].get();
        SourceState* source_ptr = tasks[task_id]->source_state_.get();
//        SinkState* sink_ptr = tasks[task_id]->sink_state_.get();

        if(source_ptr->state_type_ != SourceStateType::kTableScan) {
            SchedulerError("Expect table scan source state")
        }

        auto* table_scan_source_state = static_cast<TableScanSourceState*>(source_ptr);
        task_ptr->operator_input_state_[operator_id] = MakeUnique<TableScanInputState>();
        auto table_scan_input_state = (TableScanInputState*)(task_ptr->operator_input_state_[operator_id].get());

//        if(table_scan_source_state->segment_entry_ids_->empty()) {
//            SchedulerError("Empty segment entry ids")
//        }

        table_scan_input_state->table_scan_function_data_ = MakeUnique<TableScanFunctionData>(physical_table_scan->SegmentEntriesPtr(),
                                                                                              table_scan_source_state->segment_entry_ids_,
                                                                                              physical_table_scan->ColumnIDs());

        task_ptr->operator_output_state_[operator_id] = MakeUnique<TableScanOutputState>();
        auto output_state = (TableScanOutputState*)(task_ptr->operator_output_state_[operator_id].get());
        output_state->data_block_ = DataBlock::Make();
        output_state->data_block_->Init(*fragment_operators[operator_id]->GetOutputTypes());

        source_ptr->SetNextState(table_scan_input_state);
    }
}

template<>
void
BuildParallelTaskStateTemplate<KnnScanInputState, KnnScanOutputState>(Vector<PhysicalOperator*>& fragment_operators,
                                                                      Vector<UniquePtr<FragmentTask>>& tasks,
                                                                      i64 operator_id,
                                                                      i64 operator_count,
                                                                      i64 real_parallel_size) {

    // TableScan must be the first operator of the fragment
    if(operator_id != operator_count - 1) {
        SchedulerError("Knn scan operator must be the first operator of the fragment.")
    }

    if(operator_id == 0) {
        SchedulerError("Knn scan shouldn't be the last operator of the fragment.")
    }

    PhysicalOperator* physical_op = fragment_operators[operator_id];
    if(physical_op->operator_type() != PhysicalOperatorType::kKnnScan) {
        SchedulerError("Expect knn scan physical operator")
    }
    auto* physical_knn_scan = static_cast<PhysicalKnnScan*>(physical_op);

    for(i64 task_id = 0; task_id < real_parallel_size; ++task_id) {
        FragmentTask* task_ptr = tasks[task_id].get();
        SourceState* source_ptr = tasks[task_id]->source_state_.get();
//        SinkState* sink_ptr = tasks[task_id]->sink_state_.get();

        if(source_ptr->state_type_ != SourceStateType::kKnnScan) {
            SchedulerError("Expect knn scan source state")
        }

        auto* knn_scan_source_state = static_cast<KnnScanSourceState*>(source_ptr);
        task_ptr->operator_input_state_[operator_id] = MakeUnique<KnnScanInputState>();
        auto knn_scan_input_state = (KnnScanInputState*)(task_ptr->operator_input_state_[operator_id].get());

//        if(table_scan_source_state->segment_entry_ids_->empty()) {
//            SchedulerError("Empty segment entry ids")
//        }
        if(physical_knn_scan->knn_expressions_.size() == 1) {
            KnnExpression* knn_expr = static_cast<KnnExpression*>(physical_knn_scan->knn_expressions_[0].get());
            SchedulerAssert(knn_expr->arguments().size() == 1, "Expect one expression");
            ColumnExpression* column_expr = static_cast<ColumnExpression*>(knn_expr->arguments()[0].get());


            Vector<SizeT> knn_column_ids = {column_expr->binding().column_idx};
            ValueExpression* limit_expr = static_cast<ValueExpression*>(physical_knn_scan->limit_expression_.get());
            i64 topk = limit_expr->GetValue().GetValue<BigIntT>();

            knn_scan_input_state->knn_scan_function_data_ = MakeUnique<KnnScanFunctionData>(physical_knn_scan->SegmentEntriesPtr(),
                                                                                            knn_scan_source_state->segment_entry_ids_,
                                                                                            physical_knn_scan->ColumnIDs(),
                                                                                            knn_column_ids,
                                                                                            topk,
                                                                                            knn_expr->dimension_,
                                                                                            1,
                                                                                            knn_expr->query_embedding_.ptr,
                                                                                            knn_expr->embedding_data_type_,
                                                                                            knn_expr->distance_type_);
            knn_scan_input_state->knn_scan_function_data_->Init();
        } else {
            SchedulerError("Currently, we only support one knn column case")
        }

        task_ptr->operator_output_state_[operator_id] = MakeUnique<KnnScanOutputState>();
        auto output_state = (KnnScanOutputState*)(task_ptr->operator_output_state_[operator_id].get());
        output_state->data_block_ = DataBlock::Make();
        output_state->data_block_->Init(*fragment_operators[operator_id]->GetOutputTypes());

        source_ptr->SetNextState(knn_scan_input_state);
    }
}

void
FragmentContext::MakeFragmentContext(QueryContext* query_context,
                                     PlanFragment* fragment_ptr,
                                     Vector<FragmentTask*>& task_array) {
    Vector<PhysicalOperator*>& fragment_operators = fragment_ptr->GetOperators();
    i64 operator_count = fragment_operators.size();
    if(operator_count < 1) {
        SchedulerError("No opertors in the fragment.")
    }

    UniquePtr<FragmentContext> fragment_context;
    switch(fragment_ptr->GetFragmentType()) {
        case FragmentType::kInvalid: {
            SchedulerError("Invalid fragment type")
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
    Vector<UniquePtr<FragmentTask>>& tasks = fragment_context->Tasks();
    i64 real_parallel_size = tasks.size();

    for(i64 operator_id = operator_count - 1; operator_id >= 0; --operator_id) {

        switch(fragment_operators[operator_id]->operator_type()) {
            case PhysicalOperatorType::kInvalid: {
                PlannerError("Invalid physical operator type")
                break;
            }
            case PhysicalOperatorType::kAggregate: {
                BuildParallelTaskStateTemplate<AggregateInputState, AggregateOutputState>(fragment_operators,
                                                                                          tasks,
                                                                                          operator_id,
                                                                                          operator_count,
                                                                                          real_parallel_size);

                break;
            }
            case PhysicalOperatorType::kParallelAggregate: {
                BuildParallelTaskStateTemplate<ParallelAggregateInputState, ParallelAggregateOutputState>(
                        fragment_operators,
                        tasks,
                        operator_id,
                        operator_count,
                        real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kMergeParallelAggregate: {
                BuildSerialTaskStateTemplate<MergeParallelAggregateInputState, MergeParallelAggregateOutputState>(
                        fragment_operators,
                        tasks,
                        operator_id,
                        operator_count);
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
                SchedulerError(fmt::format("Not support {} now",
                                           PhysicalOperatorToString(fragment_operators[operator_id]->operator_type())));
                break;
            }
            case PhysicalOperatorType::kTableScan: {
                BuildParallelTaskStateTemplate<TableScanInputState, TableScanOutputState>(fragment_operators,
                                                                                          tasks,
                                                                                          operator_id,
                                                                                          operator_count,
                                                                                          real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kKnnScan: {
                BuildParallelTaskStateTemplate<KnnScanInputState, KnnScanOutputState>(fragment_operators,
                                                                                      tasks,
                                                                                      operator_id,
                                                                                      operator_count,
                                                                                      real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kMergeKnn: {
                BuildParallelTaskStateTemplate<MergeKnnInputState, MergeKnnOutputState>(fragment_operators,
                                                                                        tasks,
                                                                                        operator_id,
                                                                                        operator_count,
                                                                                        real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kFilter: {
                BuildParallelTaskStateTemplate<FilterInputState, FilterOutputState>(fragment_operators,
                                                                                    tasks,
                                                                                    operator_id,
                                                                                    operator_count,
                                                                                    real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kIndexScan: {
                BuildParallelTaskStateTemplate<IndexScanInputState, IndexScanOutputState>(fragment_operators,
                                                                                          tasks,
                                                                                          operator_id,
                                                                                          operator_count,
                                                                                          real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kHash: {
                BuildParallelTaskStateTemplate<HashInputState, HashOutputState>(fragment_operators,
                                                                                tasks,
                                                                                operator_id,
                                                                                operator_count,
                                                                                real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kMergeHash: {
                BuildSerialTaskStateTemplate<MergeHashInputState, MergeHashOutputState>(fragment_operators,
                                                                                        tasks,
                                                                                        operator_id,
                                                                                        operator_count);
                break;
            }
            case PhysicalOperatorType::kLimit: {
                BuildParallelTaskStateTemplate<LimitInputState, LimitOutputState>(fragment_operators,
                                                                                  tasks,
                                                                                  operator_id,
                                                                                  operator_count,
                                                                                  real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kMergeLimit: {
                BuildSerialTaskStateTemplate<MergeLimitInputState, MergeLimitOutputState>(fragment_operators,
                                                                                          tasks,
                                                                                          operator_id,
                                                                                          operator_count);
                break;
            }
            case PhysicalOperatorType::kTop: {
                BuildParallelTaskStateTemplate<TopInputState, TopOutputState>(fragment_operators,
                                                                              tasks,
                                                                              operator_id,
                                                                              operator_count,
                                                                              real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kMergeTop: {
                BuildSerialTaskStateTemplate<MergeTopInputState, MergeTopOutputState>(fragment_operators,
                                                                                      tasks,
                                                                                      operator_id,
                                                                                      operator_count);
                break;
            }
            case PhysicalOperatorType::kProjection: {
                BuildParallelTaskStateTemplate<ProjectionInputState, ProjectionOutputState>(fragment_operators,
                                                                                            tasks,
                                                                                            operator_id,
                                                                                            operator_count,
                                                                                            real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kSort: {
                BuildParallelTaskStateTemplate<SortInputState, SortOutputState>(fragment_operators,
                                                                                tasks,
                                                                                operator_id,
                                                                                operator_count,
                                                                                real_parallel_size);
                break;
            }
            case PhysicalOperatorType::kMergeSort: {
                BuildSerialTaskStateTemplate<MergeSortInputState, MergeSortOutputState>(fragment_operators,
                                                                                        tasks,
                                                                                        operator_id,
                                                                                        operator_count);
                break;
            }
            case PhysicalOperatorType::kDelete: {
                BuildSerialTaskStateTemplate<DeleteInputState, DeleteOutputState>(fragment_operators,
                                                                                  tasks,
                                                                                  operator_id,
                                                                                  operator_count);
                break;
            }
            case PhysicalOperatorType::kInsert: {
                BuildSerialTaskStateTemplate<InsertInputState, InsertOutputState>(fragment_operators,
                                                                                  tasks,
                                                                                  operator_id,
                                                                                  operator_count);
                break;
            }
            case PhysicalOperatorType::kImport: {
                BuildSerialTaskStateTemplate<ImportInputState, ImportOutputState>(fragment_operators,
                                                                                  tasks,
                                                                                  operator_id,
                                                                                  operator_count);
                break;
            }
            case PhysicalOperatorType::kExport: {
                BuildSerialTaskStateTemplate<ExportInputState, ExportOutputState>(fragment_operators,
                                                                                  tasks,
                                                                                  operator_id,
                                                                                  operator_count);
                break;
            }
            case PhysicalOperatorType::kCreateTable: {
                BuildSerialTaskStateTemplate<CreateTableInputState, CreateTableOutputState>(fragment_operators,
                                                                                            tasks,
                                                                                            operator_id,
                                                                                            operator_count);
                break;
            }
            case PhysicalOperatorType::kCreateCollection: {
                BuildSerialTaskStateTemplate<CreateCollectionInputState, CreateCollectionOutputState>(fragment_operators,
                                                                                                      tasks,
                                                                                                      operator_id,
                                                                                                      operator_count);
                break;
            }
            case PhysicalOperatorType::kCreateDatabase: {
                BuildSerialTaskStateTemplate<CreateDatabaseInputState, CreateDatabaseOutputState>(fragment_operators,
                                                                                                  tasks,
                                                                                                  operator_id,
                                                                                                  operator_count);
                break;
            }
            case PhysicalOperatorType::kCreateView: {
                BuildSerialTaskStateTemplate<CreateViewInputState, CreateViewOutputState>(fragment_operators,
                                                                                          tasks,
                                                                                          operator_id,
                                                                                          operator_count);
                break;
            }
            case PhysicalOperatorType::kDropTable: {
                BuildSerialTaskStateTemplate<DropTableInputState, DropTableOutputState>(fragment_operators,
                                                                                        tasks,
                                                                                        operator_id, operator_count);
                break;
            }
            case PhysicalOperatorType::kDropCollection: {
                BuildSerialTaskStateTemplate<DropCollectionInputState, DropCollectionOutputState>(fragment_operators,
                                                                                                  tasks,
                                                                                                  operator_id,
                                                                                                  operator_count);
                break;
            }
            case PhysicalOperatorType::kDropDatabase: {
                BuildSerialTaskStateTemplate<DropDatabaseInputState, DropDatabaseOutputState>(fragment_operators,
                                                                                              tasks,
                                                                                              operator_id,
                                                                                              operator_count);
                break;
            }
            case PhysicalOperatorType::kDropView: {
                BuildSerialTaskStateTemplate<DropViewInputState, DropViewOutputState>(fragment_operators,
                                                                                      tasks,
                                                                                      operator_id,
                                                                                      operator_count);
                break;
            }
            case PhysicalOperatorType::kExplain: {
                BuildSerialTaskStateTemplate<ExplainInputState, ExplainOutputState>(fragment_operators,
                                                                                    tasks,
                                                                                    operator_id,
                                                                                    operator_count);
                break;
            }
            case PhysicalOperatorType::kShow: {
                BuildSerialTaskStateTemplate<ShowInputState, ShowOutputState>(fragment_operators,
                                                                              tasks,
                                                                              operator_id,
                                                                              operator_count);
                break;
            }
        }
    }

    for(const auto& task: tasks) {
        task_array.emplace_back(task.get());
    }

    fragment_ptr->SetContext(std::move(fragment_context));
}

FragmentContext::FragmentContext(PlanFragment* fragment_ptr, QueryContext* query_context) :
        fragment_ptr_(fragment_ptr), fragment_type_(fragment_ptr->GetFragmentType()), query_context_(query_context) {};

Vector<PhysicalOperator*>&
FragmentContext::GetOperators() {
    return fragment_ptr_->GetOperators();
}

bool
FragmentContext::HasChild() const {
    return fragment_ptr_->ChildPtr() != nullptr;
}

i64
FragmentContext::GetChildParallelCount() const {
    return fragment_ptr_->ChildPtr()->GetContext()->GetParallelCount();
}

i64
FragmentContext::GetParallelCount() const {
    return tasks_.size();
}

PhysicalSink*
FragmentContext::GetSinkOperator() const {
    return fragment_ptr_->GetSinkNode();
}

PhysicalSource*
FragmentContext::GetSourceOperator() const {
    return fragment_ptr_->GetSourceNode();
}

void
FragmentContext::CreateTasks(i64 cpu_count, i64 operator_count) {
    i64 parallel_count = cpu_count;
    if(HasChild()) {
        parallel_count = std::min(GetChildParallelCount(), parallel_count);
    } else {
        PhysicalOperator* first_operator = this->GetOperators().back();
        switch(first_operator->operator_type()) {
            case PhysicalOperatorType::kTableScan: {
                auto* table_scan_operator = static_cast<PhysicalTableScan*>(first_operator);
                parallel_count = std::min(parallel_count,
                                          (i64)(table_scan_operator->SegmentEntryCount()));
                if(parallel_count == 0) {
                    parallel_count = 1;
                }
                break;
            }
            case PhysicalOperatorType::kKnnScan: {
                auto* knn_scan_operator = static_cast<PhysicalKnnScan*>(first_operator);
                parallel_count = std::min(parallel_count,
                                          (i64)(knn_scan_operator->SegmentEntryCount()));
                if(parallel_count == 0) {
                    parallel_count = 1;
                }
                break;
            }
            case PhysicalOperatorType::kProjection: {
                // Serial Materialize
                parallel_count = 1;
                break;
            }
            default: {
                break;
            }
        }
    }

    switch(fragment_type_) {
        case FragmentType::kInvalid: {
            SchedulerError("Invalid fragment type")
        }
        case FragmentType::kSerialMaterialize: {
            std::unique_lock<std::mutex> locker(locker_);
            tasks_.reserve(1);
            tasks_.emplace_back(MakeUnique<FragmentTask>(this, 0, operator_count));
            break;
        }
        case FragmentType::kParallelMaterialize:
        case FragmentType::kParallelStream: {
            std::unique_lock<std::mutex> locker(locker_);
            tasks_.reserve(parallel_count);
            for(i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_.emplace_back(MakeUnique<FragmentTask>(this, task_id, operator_count));
            }
            break;
        }
    }

    // Determine which type of source state.
    PhysicalOperator* first_operator = this->GetOperators().back();
    switch(first_operator->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            SchedulerError("Unexpected operator type")
        }
        case PhysicalOperatorType::kAggregate: {
            if(fragment_type_ != FragmentType::kParallelMaterialize) {
                SchedulerError(fmt::format("{} should in parallel materialized fragment",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            if(tasks_.size() != parallel_count) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            // Partition the hash range to each source state
            auto* aggregate_operator = (PhysicalAggregate*)first_operator;
            Vector<HashRange> hash_range = aggregate_operator->GetHashRanges(parallel_count);
            for(i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<AggregateSourceState>(hash_range[task_id].start_,
                                                                                  hash_range[task_id].end_);
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
            SchedulerError(fmt::format("{} shouldn't be the first operator of the fragment",
                                       PhysicalOperatorToString(first_operator->operator_type())));
        }
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeKnn: {
            if(fragment_type_ != FragmentType::kSerialMaterialize) {
                SchedulerError(fmt::format("{} should in serial materialized fragment",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            if(tasks_.size() != 1) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            tasks_[0]->source_state_ = MakeUnique<CommonSourceState>();
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
            SchedulerError(fmt::format("Not support {} now",
                                       PhysicalOperatorToString(first_operator->operator_type())));
        }
        case PhysicalOperatorType::kTableScan: {
            if(fragment_type_ != FragmentType::kParallelMaterialize &&
               fragment_type_ != FragmentType::kParallelStream) {
                SchedulerError(fmt::format("{} should in parallel materialized/stream fragment",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            if(tasks_.size() != parallel_count) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            // Partition the hash range to each source state
            auto* table_scan_operator = (PhysicalTableScan*)first_operator;
            Vector<SharedPtr<Vector<u64>>> segment_id_group = table_scan_operator->PlanSegmentEntries(parallel_count);
            for(i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<TableScanSourceState>(segment_id_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            if(fragment_type_ != FragmentType::kParallelMaterialize &&
               fragment_type_ != FragmentType::kParallelStream) {
                SchedulerError(fmt::format("{} should in parallel materialized/stream fragment",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            if(tasks_.size() != parallel_count) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            // Partition the hash range to each source state
            auto* knn_scan_operator = (PhysicalKnnScan*)first_operator;
            Vector<SharedPtr<Vector<u64>>> segment_id_group = knn_scan_operator->PlanSegmentEntries(parallel_count);
            for(i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<KnnScanSourceState>(segment_id_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView:
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kShow: {
            if(fragment_type_ != FragmentType::kSerialMaterialize) {
                SchedulerError(fmt::format("{} should in serial materialized fragment",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            if(tasks_.size() != 1) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(first_operator->operator_type())));
            }

            tasks_[0]->source_state_ = MakeUnique<EmptySourceState>();
            break;
        }

        default: {
            SchedulerError(fmt::format("Unexpected operator type: {}",
                                       PhysicalOperatorToString(first_operator->operator_type())));
        }
    }

    // Determine which type of the sink state.
    PhysicalOperator* last_operator = this->GetOperators().front();
    switch(last_operator->operator_type()) {

        case PhysicalOperatorType::kInvalid: {
            SchedulerError("Unexpected operator type")
        }
        case PhysicalOperatorType::kAggregate:
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort: {
            if(fragment_type_ != FragmentType::kParallelMaterialize) {
                SchedulerError(fmt::format("{} should in parallel materialized fragment",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            if(tasks_.size() != parallel_count) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            for(i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<GeneralSinkState>();
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
            if(fragment_type_ != FragmentType::kSerialMaterialize) {
                SchedulerError(fmt::format("{} should in serial materialized fragment",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            if(tasks_.size() != 1) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = MakeUnique<GeneralSinkState>();
            GeneralSinkState* sink_state_ptr = static_cast<GeneralSinkState*>(tasks_[0]->sink_state_.get());
            sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
            sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            break;
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kKnnScan:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kIndexScan:
        case PhysicalOperatorType::kProjection: {
            if(fragment_type_ == FragmentType::kSerialMaterialize) {
                SchedulerError(fmt::format("{} should in parallel materialized/stream fragment",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            if(tasks_.size() != parallel_count) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            for(i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<GeneralSinkState>();
                GeneralSinkState* sink_state_ptr = static_cast<GeneralSinkState*>(tasks_[task_id]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
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
            SchedulerError(fmt::format("Not support {} now",
                                       PhysicalOperatorToString(last_operator->operator_type())));
        }
        case PhysicalOperatorType::kDelete:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport: {
            if(fragment_type_ != FragmentType::kSerialMaterialize) {
                SchedulerError(fmt::format("{} should in serial materialized fragment",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            if(tasks_.size() != 1) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = MakeUnique<MessageSinkState>();
            break;
        }
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView: {
            if(fragment_type_ != FragmentType::kSerialMaterialize) {
                SchedulerError(fmt::format("{} should in serial materialized fragment",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            if(tasks_.size() != 1) {
                SchedulerError(fmt::format("{} task count isn't correct.",
                                           PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = MakeUnique<ResultSinkState>();
            break;
        }
        default: {
            SchedulerError(fmt::format("Unexpected operator type: {}",
                                       PhysicalOperatorToString(last_operator->operator_type())));
        }
    }
}

SharedPtr<Table>
SerialMaterializedFragmentCtx::GetResultInternal() {

    // Only one sink state
    if(tasks_.size() != 1) {
        SchedulerError("There should be one sink state in serial materialized fragment")
    }

    switch(tasks_[0]->sink_state_->state_type()) {
        case SinkStateType::kInvalid: {
            SchedulerError("Invalid sink state type")
            break;
        }
        case SinkStateType::kGeneral: {
            auto* common_sink_state = static_cast<GeneralSinkState*>(tasks_[0]->sink_state_.get());

            Vector<SharedPtr<ColumnDef>> column_defs;
            SizeT column_count = common_sink_state->column_names_->size();
            column_defs.reserve(column_count);
            for(SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
                column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                               common_sink_state->column_types_->at(col_idx),
                                                               common_sink_state->column_names_->at(col_idx),
                                                               HashSet<ConstraintType>()));
            }

            SharedPtr<Table> result_table = Table::MakeResultTable(column_defs);
            result_table->data_blocks_ = std::move(common_sink_state->data_block_array_);
            return result_table;
        }
        case SinkStateType::kResult: {
            auto* result_sink_state = static_cast<ResultSinkState*>(tasks_[0]->sink_state_.get());
            if(result_sink_state->error_message_ == nullptr) {
                SharedPtr<Table> result_table = Table::MakeResultTable({result_sink_state->result_def_});
                return result_table;
            }
            ExecutorError(*result_sink_state->error_message_);
        }
        case SinkStateType::kMessage: {
            auto* message_sink_state = static_cast<MessageSinkState*>(tasks_[0]->sink_state_.get());
            if(message_sink_state->error_message_ != nullptr) {
                throw Exception(*message_sink_state->error_message_);
            }

            if(message_sink_state->message_ == nullptr) {
                SchedulerError("No response message")
            }

            SharedPtr<Table> result_table = Table::MakeEmptyResultTable();
            result_table->SetResultMsg(std::move(message_sink_state->message_));
            return result_table;
        }
    }
    SchedulerError("Unreachable")
}

SharedPtr<Table>
ParallelMaterializedFragmentCtx::GetResultInternal() {

    SharedPtr<Table> result_table = nullptr;

    auto* first_common_sink_state = static_cast<GeneralSinkState*>(tasks_[0]->sink_state_.get());
    if(first_common_sink_state->error_message_ != nullptr) {
        ExecutorError(*first_common_sink_state->error_message_)
    }

    Vector<SharedPtr<ColumnDef>> column_defs;
    SizeT column_count = first_common_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for(SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                       first_common_sink_state->column_types_->at(col_idx),
                                                       first_common_sink_state->column_names_->at(col_idx),
                                                       HashSet<ConstraintType>()));
    }

    for(const auto& task: tasks_) {
        if(task->sink_state_->state_type() != SinkStateType::kGeneral) {
            SchedulerError("Parallel materialized fragment will only have common sink stte")
        }

        auto* common_sink_state = static_cast<GeneralSinkState*>(task->sink_state_.get());
        if(common_sink_state->error_message_ != nullptr) {
            ExecutorError(*common_sink_state->error_message_)
        }

        if(result_table == nullptr) {
            result_table = Table::MakeResultTable(column_defs);
        }

        for(const auto& result_data_block: common_sink_state->data_block_array_) {
            result_table->Append(result_data_block);
        }
    }

    return result_table;
}

SharedPtr<Table>
ParallelStreamFragmentCtx::GetResultInternal() {
    SharedPtr<Table> result_table = nullptr;

    auto* common_sink_state = static_cast<GeneralSinkState*>(tasks_[0]->sink_state_.get());

    Vector<SharedPtr<ColumnDef>> column_defs;
    SizeT column_count = common_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for(SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                       common_sink_state->column_types_->at(col_idx),
                                                       common_sink_state->column_names_->at(col_idx),
                                                       HashSet<ConstraintType>()));
    }

    for(const auto& task: tasks_) {
        if(task->sink_state_->state_type() != SinkStateType::kGeneral) {
            SchedulerError("Parallel materialized fragment will only have common sink stte")
        }

        auto* common_sink_state = static_cast<GeneralSinkState*>(task->sink_state_.get());


        if(result_table == nullptr) {
            result_table = Table::MakeResultTable(column_defs);
        }

        for(const auto& result_data_block: common_sink_state->data_block_array_) {
            result_table->Append(result_data_block);
        }
    }

    return result_table;
}

}
