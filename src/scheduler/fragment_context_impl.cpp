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

module infinity_core:fragment_context.impl;

import :fragment_context;
import :fragment_task;
import :infinity_exception;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :table_scan_function_data;
import :match_tensor_scan_function_data;
import :match_sparse_scan_function_data;
import :knn_scan_data;
import :physical_table_scan;
import :physical_index_scan;
import :physical_knn_scan;
import :physical_aggregate;
import :physical_explain;
import :physical_create_index_prepare;
import :physical_sort;
import :physical_top;
import :physical_merge_top;
import :physical_match_tensor_scan;
import :physical_match_sparse_scan;
import :physical_compact;
import :global_block_id;
import :knn_expression;
import :value_expression;
import :column_expression;
import :query_context;
import :physical_source;
import :physical_sink;
import :data_table;
import :data_block;
import :physical_merge_knn;
import :merge_knn_data;
import :create_index_data;
import :compact_state_data;
import :logger;
import :task_scheduler;
import :plan_fragment;
import :aggregate_expression;
import :expression_state;
import :block_index;
import :table_index_meeta;
import :segment_index_meta;
import :block_meta;

import std;
import third_party;

import column_def;
import explain_statement;
import global_resource_usage;

namespace infinity {

template <typename OperatorStateType>
std::unique_ptr<OperatorState> MakeTaskStateTemplate(PhysicalOperator *physical_op) {

    return std::make_unique<OperatorStateType>();
}

std::unique_ptr<OperatorState> MakeTableScanState(PhysicalTableScan *physical_table_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();

    if (source_state->state_type_ != SourceStateType::kTableScan) {
        UnrecoverableError("Expect table scan source state");
    }

    auto *table_scan_source_state = static_cast<TableScanSourceState *>(source_state);

    std::unique_ptr<OperatorState> operator_state = std::make_unique<TableScanOperatorState>();
    auto *table_scan_op_state_ptr = static_cast<TableScanOperatorState *>(operator_state.get());
    table_scan_op_state_ptr->table_scan_function_data_ = std::make_unique<TableScanFunctionData>(physical_table_scan->GetBlockIndex(),
                                                                                                 table_scan_source_state->global_ids_,
                                                                                                 physical_table_scan->ColumnIDs());
    return operator_state;
}

std::unique_ptr<OperatorState> MakeMatchTensorScanState(const PhysicalMatchTensorScan *physical_match_tensor_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kMatchTensorScan) {
        UnrecoverableError("Expect MatchTensorScan source state");
    }
    auto operator_state = std::make_unique<MatchTensorScanOperatorState>();
    operator_state->match_tensor_scan_function_data_ =
        std::make_unique<MatchTensorScanFunctionData>(physical_match_tensor_scan->GetTopN(), physical_match_tensor_scan->GetKnnThreshold());
    return operator_state;
}

std::unique_ptr<OperatorState> MakeMergeMatchTensorState(PhysicalOperator *physical_op) {
    auto operator_state = std::make_unique<MergeMatchTensorOperatorState>();
    return operator_state;
}

std::unique_ptr<OperatorState> MakeMatchSparseScanState(const PhysicalMatchSparseScan *physical_match_sparse_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    auto operator_state = std::make_unique<MatchSparseScanOperatorState>();
    auto *match_sparse_scan_source_state = static_cast<MatchSparseScanSourceState *>(source_state);
    operator_state->match_sparse_scan_function_data_ =
        MatchSparseScanFunctionData(match_sparse_scan_source_state->global_ids_, match_sparse_scan_source_state->segment_ids_);
    return operator_state;
}

std::unique_ptr<OperatorState> MakeMergeMatchSparseState(PhysicalOperator *physical_op) {
    auto operator_state = std::make_unique<MergeMatchSparseOperatorState>();
    return operator_state;
}

std::unique_ptr<OperatorState> MakeIndexScanState(PhysicalIndexScan *physical_index_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kIndexScan) {
        UnrecoverableError("Expect index scan source state");
    }
    auto *index_scan_source_state = static_cast<IndexScanSourceState *>(source_state);
    return std::make_unique<IndexScanOperatorState>(std::move(index_scan_source_state->segment_ids_));
}

std::unique_ptr<OperatorState> MakeKnnScanState(PhysicalKnnScan *physical_knn_scan, FragmentTask *task, FragmentContext *fragment_ctx) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kKnnScan) {
        UnrecoverableError("Expect knn scan source state");
    }

    std::unique_ptr<OperatorState> operator_state = std::make_unique<KnnScanOperatorState>();
    auto *knn_scan_op_state_ptr = static_cast<KnnScanOperatorState *>(operator_state.get());
    const bool execute_block_scan_job = task->TaskID() < static_cast<i64>(physical_knn_scan->BlockScanTaskCount());
    switch (fragment_ctx->ContextType()) {
        case FragmentType::kSerialMaterialize: {
            auto *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_ctx);
            knn_scan_op_state_ptr->knn_scan_function_data_ =
                std::make_unique<KnnScanFunctionData>(serial_materialize_fragment_ctx->knn_scan_shared_data_.get(),
                                                      task->TaskID(),
                                                      execute_block_scan_job);
            break;
        }
        case FragmentType::kParallelMaterialize: {
            auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
            knn_scan_op_state_ptr->knn_scan_function_data_ =
                std::make_unique<KnnScanFunctionData>(parallel_materialize_fragment_ctx->knn_scan_shared_data_.get(),
                                                      task->TaskID(),
                                                      execute_block_scan_job);
            break;
        }
        default: {
            UnrecoverableError("Invalid fragment type.");
        }
    }

    return operator_state;
}

std::unique_ptr<OperatorState> MakeCompactState(PhysicalCompact *physical_compact, FragmentTask *task, FragmentContext *fragment_ctx) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kCompact) {
        UnrecoverableError("Expect compact source state");
    }
    if (fragment_ctx->ContextType() != FragmentType::kParallelMaterialize) {
        UnrecoverableError("Compact operator should be in parallel materialized fragment.");
    }
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
    auto compact_operator_state = std::make_unique<CompactOperatorState>(parallel_materialize_fragment_ctx->compact_state_data_);
    return compact_operator_state;
}

std::unique_ptr<OperatorState> MakeAggregateState(PhysicalAggregate *physical_aggregate, FragmentTask *task) {
    std::vector<std::unique_ptr<char[]>> states;
    for (auto &expr : physical_aggregate->aggregates_) {
        auto agg_expr = std::static_pointer_cast<AggregateExpression>(expr);
        states.push_back(agg_expr->aggregate_function_.InitState());
    }
    return std::make_unique<AggregateOperatorState>(std::move(states));
}

std::unique_ptr<OperatorState> MakeMergeKnnState(PhysicalMergeKnn *physical_merge_knn, FragmentTask *task) {
    KnnExpression *knn_expr = physical_merge_knn->knn_expression_.get();
    auto operator_state = std::make_unique<MergeKnnOperatorState>();
    auto *merge_knn_op_state_ptr = static_cast<MergeKnnOperatorState *>(operator_state.get());
    // Set fake parallel number here. It will be set in SetMergeKnnState
    merge_knn_op_state_ptr->merge_knn_function_data_ = std::make_shared<MergeKnnFunctionData>(1,
                                                                                              knn_expr->topn_,
                                                                                              knn_expr->embedding_data_type_,
                                                                                              knn_expr->distance_type_,
                                                                                              physical_merge_knn->base_table_ref_);

    return operator_state;
}

std::unique_ptr<OperatorState> MakeSortState(PhysicalOperator *physical_op) {
    auto operator_state = std::make_unique<SortOperatorState>();
    auto &expr_states = operator_state->expr_states_;
    auto &sort_expressions = (static_cast<PhysicalSort *>(physical_op))->GetSortExpressions();
    expr_states.reserve(sort_expressions.size());
    for (auto &expr : sort_expressions) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    return operator_state;
}

std::unique_ptr<OperatorState> MakeTopState(PhysicalOperator *physical_op) {
    auto operator_state = std::make_unique<TopOperatorState>();
    auto &expr_states = operator_state->expr_states_;
    auto &sort_expressions = (static_cast<PhysicalTop *>(physical_op))->GetSortExpressions();
    expr_states.reserve(sort_expressions.size());
    for (auto &expr : sort_expressions) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    return operator_state;
}

std::unique_ptr<OperatorState> MakeMergeTopState(PhysicalOperator *physical_op) {
    auto operator_state = std::make_unique<MergeTopOperatorState>();
    auto &expr_states = operator_state->expr_states_;
    auto &sort_expressions = (static_cast<PhysicalMergeTop *>(physical_op))->GetSortExpressions();
    expr_states.reserve(sort_expressions.size());
    for (auto &expr : sort_expressions) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    return operator_state;
}

std::unique_ptr<OperatorState>
MakeTaskState(size_t operator_id, const std::vector<PhysicalOperator *> &physical_ops, FragmentTask *task, FragmentContext *fragment_ctx) {
    switch (physical_ops[operator_id]->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            UnrecoverableError("Invalid physical operator type");
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            if (operator_id != physical_ops.size() - 1) {
                UnrecoverableError("Table scan operator must be the first operator of the fragment.");
            }

            if (operator_id == 0) {
                UnrecoverableError("Table scan shouldn't be the last operator of the fragment.");
            }
            auto *physical_table_scan = static_cast<PhysicalTableScan *>(physical_ops[operator_id]);
            return MakeTableScanState(physical_table_scan, task);
        }
        case PhysicalOperatorType::kKnnScan: {
            auto *physical_knn_scan = static_cast<PhysicalKnnScan *>(physical_ops[operator_id]);
            return MakeKnnScanState(physical_knn_scan, task, fragment_ctx);
        }
        case PhysicalOperatorType::kAggregate: {
            auto *physical_aggregate = static_cast<PhysicalAggregate *>(physical_ops[operator_id]);
            return MakeAggregateState(physical_aggregate, task);
        }
        case PhysicalOperatorType::kMergeAggregate: {
            return MakeTaskStateTemplate<MergeAggregateOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kParallelAggregate: {
            return MakeTaskStateTemplate<ParallelAggregateOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMergeParallelAggregate: {
            return MakeTaskStateTemplate<MergeParallelAggregateOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kFilter: {
            return MakeTaskStateTemplate<FilterOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kUnnest: {
            return MakeTaskStateTemplate<UnnestOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kUnnestAggregate: {
            return MakeTaskStateTemplate<UnnestAggregateOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kIndexScan: {
            if (operator_id != physical_ops.size() - 1) {
                UnrecoverableError("Table scan operator must be the first operator of the fragment.");
            }

            if (operator_id == 0) {
                UnrecoverableError("Table scan shouldn't be the last operator of the fragment.");
            }
            auto physical_index_scan = static_cast<PhysicalIndexScan *>(physical_ops[operator_id]);
            return MakeIndexScanState(physical_index_scan, task);
        }
        case PhysicalOperatorType::kMergeKnn: {
            auto *physical_merge_knn = static_cast<PhysicalMergeKnn *>(physical_ops[operator_id]);
            return MakeMergeKnnState(physical_merge_knn, task);
        }
        case PhysicalOperatorType::kHash: {
            return MakeTaskStateTemplate<HashOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMergeHash: {
            return MakeTaskStateTemplate<MergeHashOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kLimit: {
            return MakeTaskStateTemplate<LimitOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMergeLimit: {
            return MakeTaskStateTemplate<MergeLimitOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kTop: {
            return MakeTopState(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMergeTop: {
            return MakeMergeTopState(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMatchTensorScan: {
            auto *physical_match_tensor_scan = static_cast<PhysicalMatchTensorScan *>(physical_ops[operator_id]);
            return MakeMatchTensorScanState(physical_match_tensor_scan, task);
        }
        case PhysicalOperatorType::kMergeMatchTensor: {
            return MakeMergeMatchTensorState(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMatchSparseScan: {
            auto *physical_match_sparse_scan = static_cast<PhysicalMatchSparseScan *>(physical_ops[operator_id]);
            return MakeMatchSparseScanState(physical_match_sparse_scan, task);
        }
        case PhysicalOperatorType::kMergeMatchSparse: {
            return MakeMergeMatchSparseState(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kProjection: {
            return MakeTaskStateTemplate<ProjectionOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kSort: {
            return MakeSortState(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMergeSort: {
            return MakeTaskStateTemplate<MergeSortOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kDelete: {
            return MakeTaskStateTemplate<DeleteOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kUpdate: {
            return MakeTaskStateTemplate<UpdateOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kInsert: {
            return MakeTaskStateTemplate<InsertOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kImport: {
            return MakeTaskStateTemplate<ImportOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kExport: {
            return MakeTaskStateTemplate<ExportOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kFlush: {
            return MakeTaskStateTemplate<FlushOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kOptimize: {
            return MakeTaskStateTemplate<OptimizeOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCreateTable: {
            return MakeTaskStateTemplate<CreateTableOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            return MakeTaskStateTemplate<CreateIndexPrepareOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCreateCollection: {
            return MakeTaskStateTemplate<CreateCollectionOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCreateDatabase: {
            return MakeTaskStateTemplate<CreateDatabaseOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCreateView: {
            return MakeTaskStateTemplate<CreateViewOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kDropTable: {
            return MakeTaskStateTemplate<DropTableOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kDropIndex: {
            return MakeTaskStateTemplate<DropIndexOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kDropCollection: {
            return MakeTaskStateTemplate<DropCollectionOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kDropDatabase: {
            return MakeTaskStateTemplate<DropDatabaseOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kDropView: {
            return MakeTaskStateTemplate<DropViewOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCommand: {
            return MakeTaskStateTemplate<CommandOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCompact: {
            auto *physical_compact = static_cast<PhysicalCompact *>(physical_ops[operator_id]);
            return MakeCompactState(physical_compact, task, fragment_ctx);
        }
        case PhysicalOperatorType::kExplain: {
            return MakeTaskStateTemplate<ExplainOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kShow: {
            return MakeTaskStateTemplate<ShowOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMatch: {
            return MakeTaskStateTemplate<MatchOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kFusion: {
            return MakeTaskStateTemplate<FusionOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kAlter: {
            return MakeTaskStateTemplate<AlterOperatorState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kReadCache: {
            return MakeTaskStateTemplate<ReadCacheState>(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kCheck: {
            return MakeTaskStateTemplate<CheckOperatorState>(physical_ops[operator_id]);
        }
        default: {
            UnrecoverableError(fmt::format("Not support {} now", PhysicalOperatorToString(physical_ops[operator_id]->operator_type())));
        }
    }
    return nullptr;
}

void CollectTasks(std::vector<std::shared_ptr<std::string>> &result, PlanFragment *plan_fragment_ptr) {
    if (plan_fragment_ptr->GetContext() == nullptr) {
        return;
    }

    auto tasks = &plan_fragment_ptr->GetContext()->Tasks();
    {
        std::string fragment_header = "Fragment #";

        fragment_header += std::to_string(plan_fragment_ptr->FragmentID());
        fragment_header += " * ";
        fragment_header += std::to_string(tasks->size());
        fragment_header += " Task";

        result.emplace_back(std::make_shared<std::string>(fragment_header));
    }
    for (const auto &task : *tasks) {
        result.emplace_back(std::make_shared<std::string>(fmt::format("-> Task #{}", task->TaskID())));
    }
    // NOTE: Insert blank elements after each Fragment for alignment
    result.emplace_back(std::make_shared<std::string>());

    if (plan_fragment_ptr->HasChild()) {
        // current fragment have children
        for (const auto &child_fragment : plan_fragment_ptr->Children()) {
            CollectTasks(result, child_fragment.get());
        }
    }
}

void FragmentContext::BuildTask(QueryContext *query_context, FragmentContext *parent_context, PlanFragment *plan_fragment_ptr, Notifier *notifier) {
    std::vector<PhysicalOperator *> &fragment_operators = plan_fragment_ptr->GetOperators();
    i64 operator_count = fragment_operators.size();
    if (operator_count < 1) {
        UnrecoverableError("No operators in the fragment.");
    }

    std::unique_ptr<FragmentContext> fragment_context = nullptr;
    switch (plan_fragment_ptr->GetFragmentType()) {
        case FragmentType::kInvalid: {
            UnrecoverableError("Invalid fragment type");
        }
        case FragmentType::kSerialMaterialize: {
            fragment_context = std::make_unique<SerialMaterializedFragmentCtx>(plan_fragment_ptr, query_context, notifier);
            break;
        }
        case FragmentType::kParallelMaterialize: {
            fragment_context = std::make_unique<ParallelMaterializedFragmentCtx>(plan_fragment_ptr, query_context, notifier);
            break;
        }
        case FragmentType::kParallelStream: {
            fragment_context = std::make_unique<ParallelStreamFragmentCtx>(plan_fragment_ptr, query_context, notifier);
            break;
        }
    }

    // Set parallel size
    i64 parallel_size = static_cast<i64>(query_context->cpu_number_limit());
    // i64 parallel_size = 1;

    fragment_context->CreateTasks(parallel_size, operator_count, parent_context);

    std::vector<std::unique_ptr<FragmentTask>> &tasks = fragment_context->Tasks();
    i64 real_parallel_size = tasks.size();

    for (i64 operator_id = operator_count - 1; operator_id >= 0; --operator_id) {

        // PhysicalOperator *physical_op = fragment_operators[operator_id];
        for (size_t task_id = 0; task_id < tasks.size(); ++task_id) {
            FragmentTask *task = tasks[task_id].get();

            // build the input and output state of each operator
            std::unique_ptr<OperatorState> operator_state = MakeTaskState(operator_id, fragment_operators, task, fragment_context.get());

            // Connect the input, output state. Connect fragment to its parent if needed
            if (operator_id == operator_count - 1) {
                // first operator, set first operator input to source
                SourceState *source_state = task->source_state_.get();
                source_state->SetNextOpState(operator_state.get());
            } else {
                // not the first operator, set current input to previous output
                OperatorState *prev_operator_output = task->operator_states_[operator_id + 1].get();
                operator_state->ConnectToPrevOutputOpState(prev_operator_output);
            }

            if (operator_id == 0) {
                // Set last operator output as the input of sink operator
                SinkState *sink_state = tasks[task_id]->sink_state_.get();
                sink_state->SetPrevOpState(operator_state.get());

                if (parent_context != nullptr) {
                    // this fragment has parent fragment, which means the sink node of current fragment
                    // will sink data to the parent fragment.
                    switch (sink_state->state_type_) {
                        case SinkStateType::kQueue: {
                            auto *queue_sink_state = static_cast<QueueSinkState *>(sink_state);
                            for (const auto &next_fragment_task : parent_context->Tasks()) {
                                auto *next_fragment_source_state = static_cast<QueueSourceState *>(next_fragment_task->source_state_.get());
                                next_fragment_source_state->SetTaskNum(fragment_context->plan_fragment_ptr_->FragmentID(), real_parallel_size);
                                queue_sink_state->fragment_data_queues_.emplace_back(&next_fragment_source_state->source_queue_);
                            }
                            break;
                        }
                        case SinkStateType::kInvalid: {
                            UnrecoverableError("Invalid sink operator state type.");
                        }
                        default: {
                            break;
                        }
                    }
                }
            }

            task->operator_states_[operator_id] = std::move(operator_state);
        }
    }

    if (plan_fragment_ptr->HasChild()) {
        // current fragment have children
        for (const auto &child_fragment : plan_fragment_ptr->Children()) {
            FragmentContext::BuildTask(query_context, fragment_context.get(), child_fragment.get(), notifier);
        }
    }
    switch (fragment_operators[0]->operator_type()) {
        case PhysicalOperatorType::kExplain: {
            std::vector<std::shared_ptr<std::string>> result;
            auto *explain_op = static_cast<PhysicalExplain *>(fragment_operators[0]);

            if (explain_op->explain_type() == ExplainType::kPipeline or explain_op->explain_type() == ExplainType::kAnalyze) {
                CollectTasks(result, plan_fragment_ptr->Children()[0].get());
                explain_op->SetExplainTaskText(std::make_shared<std::vector<std::shared_ptr<std::string>>>(result));
                explain_op->SetPlanFragment(plan_fragment_ptr->Children()[0].get());
                break;
            }
        }
        default:
            break;
    }

    plan_fragment_ptr->SetContext(std::move(fragment_context));
}

FragmentContext::FragmentContext(PlanFragment *plan_fragment_ptr, QueryContext *query_context, Notifier *notifier)
    : notifier_(notifier), plan_fragment_ptr_(plan_fragment_ptr), query_context_(query_context), fragment_type_(plan_fragment_ptr->GetFragmentType()),
      unfinished_child_n_(plan_fragment_ptr->Children().size()) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("FragmentContext");
#endif
}

FragmentContext::~FragmentContext() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("FragmentContext");
#endif
}

bool FragmentContext::TryFinishFragment() {
    auto fragment_id = plan_fragment_ptr_->FragmentID();
    auto parent_plan_fragments = plan_fragment_ptr_->GetParents();

    if (!TryFinishFragmentInner()) {
        LOG_TRACE(fmt::format("{} tasks in fragment {} are not completed", unfinished_task_n_.load(), fragment_id));
        if (fragment_type_ == FragmentType::kParallelStream) {
            bool sent_data = false;
            for (auto &task : tasks_) {
                if (task->sink_state_->state_type_ == SinkStateType::kQueue) {
                    auto *queue_sink_state = static_cast<QueueSinkState *>(task->sink_state_.get());
                    {
                        std::lock_guard<std::mutex> lock(queue_sink_state->sent_data_mutex_);
                        if (queue_sink_state->sent_data_) {
                            sent_data = true;
                            queue_sink_state->sent_data_ = false;
                        }
                    }
                }
            }
            if (!sent_data) {
                return false;
            }

            for (auto *parent_plan_fragment : parent_plan_fragments) {
                auto *scheduler = query_context_->scheduler();
                LOG_TRACE(fmt::format("Schedule fragment: {} before fragment {} has finished.", parent_plan_fragment->FragmentID(), fragment_id));
                scheduler->ScheduleFragment(parent_plan_fragment);
            }
        }
        return false;
    } else {
        LOG_TRACE(fmt::format("All tasks in fragment: {} are completed", fragment_id));

        for (auto *parent_plan_fragment : parent_plan_fragments) {
            auto *parent_fragment_ctx = parent_plan_fragment->GetContext();
            if (parent_fragment_ctx->TryStartFragment()) {
                // All child fragment are finished.

                auto *scheduler = query_context_->scheduler();
                LOG_TRACE(fmt::format("Schedule fragment: {} because fragment {} has finished.",
                                      parent_plan_fragment->FragmentID(),
                                      plan_fragment_ptr_->FragmentID()));
                scheduler->ScheduleFragment(parent_plan_fragment);
            }
        }
        return true;
    }
}

std::vector<PhysicalOperator *> &FragmentContext::GetOperators() { return plan_fragment_ptr_->GetOperators(); }

PhysicalSink *FragmentContext::GetSinkOperator() const { return plan_fragment_ptr_->GetSinkNode(); }

PhysicalSource *FragmentContext::GetSourceOperator() const { return plan_fragment_ptr_->GetSourceNode(); }

size_t InitKnnScanFragmentContext(PhysicalKnnScan *knn_scan_operator, FragmentContext *fragment_context, QueryContext *query_context) {

    size_t task_n = knn_scan_operator->TaskletCount();
    KnnExpression *knn_expr = knn_scan_operator->knn_expression_.get();

    switch (fragment_context->ContextType()) {
        case FragmentType::kSerialMaterialize: {
            auto *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_context);
            serial_materialize_fragment_ctx->knn_scan_shared_data_ =
                std::make_unique<KnnScanSharedData>(knn_scan_operator->base_table_ref_,
                                                    std::move(knn_scan_operator->block_metas_),
                                                    std::move(knn_scan_operator->table_index_meta_),
                                                    std::move(knn_scan_operator->segment_index_metas_),
                                                    std::move(knn_expr->opt_params_),
                                                    knn_expr->topn_,
                                                    knn_expr->dimension_,
                                                    1,
                                                    knn_scan_operator->real_knn_query_embedding_ptr_,
                                                    knn_scan_operator->real_knn_query_elem_type_,
                                                    knn_expr->distance_type_);
            break;
        }
        case FragmentType::kParallelMaterialize: {
            auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_context);
            parallel_materialize_fragment_ctx->knn_scan_shared_data_ =
                std::make_unique<KnnScanSharedData>(knn_scan_operator->base_table_ref_,
                                                    std::move(knn_scan_operator->block_metas_),
                                                    std::move(knn_scan_operator->table_index_meta_),
                                                    std::move(knn_scan_operator->segment_index_metas_),
                                                    std::move(knn_expr->opt_params_),
                                                    knn_expr->topn_,
                                                    knn_expr->dimension_,
                                                    1,
                                                    knn_scan_operator->real_knn_query_embedding_ptr_,
                                                    knn_scan_operator->real_knn_query_elem_type_,
                                                    knn_expr->distance_type_);
            break;
        }
        default: {
            UnrecoverableError("Invalid fragment type.");
        }
    }

    return task_n;
}

size_t InitCompactFragmentContext(PhysicalCompact *compact_operator, FragmentContext *fragment_context, FragmentContext *parent_context) {
    if (parent_context == nullptr) {
        size_t task_n = compact_operator->TaskletCount();
        if (fragment_context->ContextType() != FragmentType::kParallelMaterialize) {
            UnrecoverableError("Compact operator should be in serial materialized fragment.");
        }
        auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_context);
        parallel_materialize_fragment_ctx->compact_state_data_ = std::make_shared<CompactStateData>(compact_operator->base_table_ref_->table_info_);
        return task_n;
    }

    size_t task_n = compact_operator->TaskletCount();
    if (fragment_context->ContextType() != FragmentType::kParallelMaterialize) {
        UnrecoverableError("Compact operator should be in parallel materialized fragment.");
    }
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_context);
    if (parent_context->ContextType() != FragmentType::kSerialMaterialize) {
        UnrecoverableError("Compact operator parent should be in serial materialized fragment.");
    }
    auto *parent_serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(parent_context);

    auto &compact_state_data = parent_serial_materialize_fragment_ctx->compact_state_data_;
    parallel_materialize_fragment_ctx->compact_state_data_ = compact_state_data;
    return task_n;
}

void FragmentContext::MakeSourceState(i64 parallel_count) {
    PhysicalOperator *first_operator = this->GetOperators().back();
    switch (first_operator->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            UnrecoverableError("Unexpected operator type");
        }
        case PhysicalOperatorType::kAggregate: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if ((i64)tasks_.size() != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }

            // Partition the hash range to each source state
            auto *aggregate_operator = static_cast<PhysicalAggregate *>(first_operator);
            std::vector<HashRange> hash_range = aggregate_operator->GetHashRanges(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = std::make_unique<AggregateSourceState>(hash_range[task_id].start_, hash_range[task_id].end_);
            }
            break;
        }
        case PhysicalOperatorType::kProjection: {
            if (this->GetOperators().size() == 1) {
                // Only one operator and it's project
                tasks_[0]->source_state_ = std::make_unique<EmptySourceState>();
            } else {
                // Check if this is part of an INSERT SELECT operation
                bool is_insert_select = false;
                for (const auto &op : this->GetOperators()) {
                    if (op->operator_type() == PhysicalOperatorType::kInsert) {
                        is_insert_select = true;
                        break;
                    }
                }

                if (is_insert_select) {
                    // Allow Project operator in INSERT SELECT case
                    tasks_[0]->source_state_ = std::make_unique<EmptySourceState>();
                } else {
                    UnrecoverableError("Project shouldn't be the first operator of the fragment");
                }
            }
            break;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kUnnest:
        case PhysicalOperatorType::kUnnestAggregate:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort:
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kDelete: {
            UnrecoverableError(
                fmt::format("{} shouldn't be the first operator of the fragment", PhysicalOperatorToString(first_operator->operator_type())));
            break;
        }
        case PhysicalOperatorType::kMergeAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeKnn:
        case PhysicalOperatorType::kMergeMatchTensor:
        case PhysicalOperatorType::kMergeMatchSparse:
        case PhysicalOperatorType::kFusion: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should be serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }

            tasks_[0]->source_state_ = std::make_unique<QueueSourceState>();
            break;
        }
        case PhysicalOperatorType::kCompact: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = std::make_unique<CompactSourceState>();
            }
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
        case PhysicalOperatorType::kPreparedPlan: {
            UnrecoverableError(fmt::format("Not support {} now", PhysicalOperatorToString(first_operator->operator_type())));
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }

            // Partition the hash range to each source state
            auto *table_scan_operator = (PhysicalTableScan *)first_operator;
            std::vector<std::shared_ptr<std::vector<GlobalBlockID>>> blocks_group = table_scan_operator->PlanBlockEntries(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = std::make_unique<TableScanSourceState>(blocks_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kMatchTensorScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = std::make_unique<MatchTensorScanSourceState>();
            }
            break;
        }
        case PhysicalOperatorType::kMatchSparseScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }
            auto *match_sparse_scan_operator = static_cast<PhysicalMatchSparseScan *>(first_operator);
            std::vector<std::shared_ptr<std::vector<GlobalBlockID>>> blocks_group = match_sparse_scan_operator->PlanBlockEntries(parallel_count);
            std::vector<std::shared_ptr<std::vector<SegmentID>>> segment_group =
                match_sparse_scan_operator->PlanWithIndex(blocks_group, parallel_count, query_context_);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ =
                    std::make_unique<MatchSparseScanSourceState>(std::move(blocks_group[task_id]), segment_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kIndexScan: {
            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }

            // Partition the hash range to each source state
            auto *index_scan_operator = (PhysicalIndexScan *)first_operator;
            std::vector<std::unique_ptr<std::vector<SegmentID>>> segment_ids = index_scan_operator->PlanSegments(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = std::make_unique<IndexScanSourceState>(std::move(segment_ids[task_id]));
            }
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }

            for (size_t task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = std::make_unique<KnnScanSourceState>();
            }
            break;
        }
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateIndexPrepare:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropIndex:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView:
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kShow:
        case PhysicalOperatorType::kMatch:
        case PhysicalOperatorType::kOptimize:
        case PhysicalOperatorType::kFlush:
        case PhysicalOperatorType::kReadCache:
        case PhysicalOperatorType::kCheck: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type())));
            }

            tasks_[0]->source_state_ = std::make_unique<EmptySourceState>();
            break;
        }

        default: {
            UnrecoverableError(fmt::format("Unexpected operator type: {}", PhysicalOperatorToString(first_operator->operator_type())));
        }
    }
}

void FragmentContext::MakeSinkState(i64 parallel_count) {
    PhysicalOperator *first_operator = this->GetOperators().back();
    PhysicalOperator *last_operator = this->GetOperators().front();
    switch (last_operator->operator_type()) {

        case PhysicalOperatorType::kInvalid: {
            UnrecoverableError("Unexpected operator type");
        }
        case PhysicalOperatorType::kAggregate: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(fmt::format("{} should in parallel stream fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                auto sink_state = std::make_unique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);

                tasks_[task_id]->sink_state_ = std::move(sink_state);
            }
            break;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kHash: {
            if (fragment_type_ != FragmentType::kParallelStream) {
                UnrecoverableError(fmt::format("{} should in parallel stream fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                auto sink_state = std::make_unique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
                sink_state->column_types_ = last_operator->GetOutputTypes();
                sink_state->column_names_ = last_operator->GetOutputNames();

                tasks_[task_id]->sink_state_ = std::move(sink_state);
            }
            break;
        }
        case PhysicalOperatorType::kLimit: {
            // if (fragment_type_ != FragmentType::kParallelStream) {
            //     PhysicalOperatorToString(last_operator->operator_type()));
            //     UnrecoverableError(fmt::format("{} should in parallel stream fragment");
            // }

            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                auto sink_state = std::make_unique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);

                tasks_[task_id]->sink_state_ = std::move(sink_state);
            }
            break;
        }
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kMergeAggregate:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kMergeMatchTensor:
        case PhysicalOperatorType::kMergeMatchSparse:
        case PhysicalOperatorType::kMergeKnn: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = std::make_unique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), 0);
            break;
        }
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kShow:
        case PhysicalOperatorType::kCheck: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = std::make_unique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), 0);
            auto *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
            sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
            sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            break;
        }
        case PhysicalOperatorType::kReadCache:
        case PhysicalOperatorType::kMatch: {
            for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = std::make_unique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
            }
            break;
        }
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kSort:
        case PhysicalOperatorType::kMatchTensorScan:
        case PhysicalOperatorType::kMatchSparseScan:
        case PhysicalOperatorType::kKnnScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = std::make_unique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
            }
            break;
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kUnnest:
        case PhysicalOperatorType::kUnnestAggregate:
        case PhysicalOperatorType::kIndexScan: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized/stream fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (static_cast<i64>(tasks_.size()) != parallel_count) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = std::make_unique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
                auto *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[task_id]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            }
            break;
        }
        case PhysicalOperatorType::kProjection: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                if (tasks_.size() != 1) {
                    UnrecoverableError("SerialMaterialize type fragment should only have 1 task.");
                }

                tasks_[0]->sink_state_ = std::make_unique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), 0);
                auto *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            } else {
                if (static_cast<i64>(tasks_.size()) != parallel_count) {
                    UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
                }

                for (u64 task_id = 0; static_cast<i64>(task_id) < parallel_count; ++task_id) {
                    tasks_[task_id]->sink_state_ = std::make_unique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
                    auto *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[task_id]->sink_state_.get());
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
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kPreparedPlan: {
            UnrecoverableError(fmt::format("Not support {} now", PhysicalOperatorToString(last_operator->operator_type())));
        }
        case PhysicalOperatorType::kDelete:
        case PhysicalOperatorType::kUpdate: {
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = std::make_unique<SummarySinkState>();
            }
            break;
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            tasks_[0]->sink_state_ = std::make_unique<ResultSinkState>();
            break;
        }
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = std::make_unique<MessageSinkState>();
            break;
        }
        case PhysicalOperatorType::kCompact: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }
            for (auto &task : tasks_) {
                task->sink_state_ = std::make_unique<MessageSinkState>();
            }
            break;
        }
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropIndex:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView:
        case PhysicalOperatorType::kOptimize:
        case PhysicalOperatorType::kFlush: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                UnrecoverableError(fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type())));
            }

            tasks_[0]->sink_state_ = std::make_unique<ResultSinkState>();
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unexpected operator type: {}", PhysicalOperatorToString(last_operator->operator_type())));
        }
    }
}

// Allocate tasks for the fragment and determine the sink and source
void FragmentContext::CreateTasks(i64 cpu_count, i64 operator_count, FragmentContext *parent_context) {
    i64 parallel_count = cpu_count;
    PhysicalOperator *first_operator = this->GetOperators().back();
    switch (first_operator->operator_type()) {
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kMatchTensorScan:
        case PhysicalOperatorType::kMatchSparseScan:
        case PhysicalOperatorType::kIndexScan: {
            parallel_count = std::min(parallel_count, (i64)(first_operator->TaskletCount()));
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            auto *knn_scan_operator = static_cast<PhysicalKnnScan *>(first_operator);
            size_t task_n = InitKnnScanFragmentContext(knn_scan_operator, this, query_context_);
            parallel_count = std::min(parallel_count, (i64)task_n);
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kReadCache:
        case PhysicalOperatorType::kMatch:
        case PhysicalOperatorType::kMergeKnn:
        case PhysicalOperatorType::kMergeMatchTensor:
        case PhysicalOperatorType::kMergeMatchSparse:
        case PhysicalOperatorType::kProjection: {
            // Serial Materialize
            parallel_count = 1;
            break;
        }
        case PhysicalOperatorType::kCompact: {
            auto *compact_operator = static_cast<PhysicalCompact *>(first_operator);
            size_t task_n = InitCompactFragmentContext(compact_operator, this, parent_context);
            parallel_count = std::min(parallel_count, static_cast<i64>(task_n));
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        default: {
            break;
        }
    }

    switch (fragment_type_) {
        case FragmentType::kInvalid: {
            UnrecoverableError("Invalid fragment type");
        }
        case FragmentType::kSerialMaterialize: {
            parallel_count = 1;
            tasks_.reserve(parallel_count);
            tasks_.emplace_back(std::make_unique<FragmentTask>(this, 0, operator_count));
            IncreaseTask();
            break;
        }
        case FragmentType::kParallelMaterialize:
        case FragmentType::kParallelStream: {
            tasks_.reserve(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_.emplace_back(std::make_unique<FragmentTask>(this, task_id, operator_count));
                IncreaseTask();
            }
            break;
        }
    }

    // Determine which type of source state.
    MakeSourceState(parallel_count);

    // Determine which type of the sink state.
    MakeSinkState(parallel_count);
}

std::shared_ptr<DataTable> SerialMaterializedFragmentCtx::GetResultInternal() {
    // Only one sink state
    if (tasks_.size() != 1) {
        UnrecoverableError("There should be one sink state in serial materialized fragment");
    }

    if (tasks_[0]->sink_state_->Ignore()) {
        std::shared_ptr<DataTable> result_table = DataTable::MakeEmptyResultTable();
        result_table->SetResultMsg(std::make_unique<std::string>("Ignore error"));
        return result_table;
    }
    if (tasks_[0]->sink_state_->Error()) {
        RecoverableError(tasks_[0]->sink_state_->status_);
    }

    switch (tasks_[0]->sink_state_->state_type()) {
        case SinkStateType::kInvalid: {
            UnrecoverableError("Invalid sink state type");
            break;
        }
        case SinkStateType::kMaterialize: {
            auto *materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());

            std::vector<std::shared_ptr<ColumnDef>> column_defs;
            size_t column_count = materialize_sink_state->column_names_->size();
            column_defs.reserve(column_count);
            for (size_t col_idx = 0; col_idx < column_count; ++col_idx) {
                column_defs.emplace_back(std::make_shared<ColumnDef>(col_idx,
                                                                     materialize_sink_state->column_types_->at(col_idx),
                                                                     materialize_sink_state->column_names_->at(col_idx),
                                                                     std::set<ConstraintType>()));
            }

            std::shared_ptr<DataTable> result_table = DataTable::MakeResultTable(column_defs);
            result_table->total_hits_count_flag_ = materialize_sink_state->total_hits_count_flag_;
            result_table->total_hits_count_ = materialize_sink_state->total_hits_count_;
            for (auto &data_block : materialize_sink_state->data_block_array_) {
                result_table->UpdateRowCount(data_block->row_count());
                result_table->data_blocks_.emplace_back(std::move(data_block));
            }
            materialize_sink_state->data_block_array_.clear();
            //            result_table->data_blocks_ = std::move(materialize_sink_state->data_block_array_);
            return result_table;
        }
        case SinkStateType::kResult: {
            auto *result_sink_state = static_cast<ResultSinkState *>(tasks_[0]->sink_state_.get());
            std::shared_ptr<DataTable> result_table = DataTable::MakeResultTable({result_sink_state->result_def_});
            return result_table;
        }
        case SinkStateType::kMessage: {
            auto *message_sink_state = static_cast<MessageSinkState *>(tasks_[0]->sink_state_.get());
            if (message_sink_state->message_.get() == nullptr) {
                UnrecoverableError("No response message");
            }

            std::shared_ptr<DataTable> result_table = DataTable::MakeEmptyResultTable();
            result_table->SetResultMsg(std::move(message_sink_state->message_));
            return result_table;
        }
        case SinkStateType::kSummary: {
            u64 counter = 0, sum = 0;
            for (size_t i = 0; i < tasks_.size(); ++i) {
                auto summaryState = dynamic_cast<SummarySinkState *>(tasks_[i]->sink_state_.get());
                counter += summaryState->count_;
                sum += summaryState->sum_;
            }
            std::shared_ptr<DataTable> result_table = DataTable::MakeSummaryResultTable(counter, sum);
            return result_table;
        }
        case SinkStateType::kQueue: {
            UnrecoverableError("Can't get result from Queue sink type.");
        }
    }
    UnrecoverableError("Unreachable");
    return nullptr;
}

std::shared_ptr<DataTable> ParallelMaterializedFragmentCtx::GetResultInternal() {
    std::shared_ptr<DataTable> result_table = nullptr;
    for (const auto &task : tasks_) {
        if (!task->sink_state_->status_.ok()) {
            RecoverableError(task->sink_state_->status_);
        }
    }
    if (tasks_[0]->sink_state_->state_type() == SinkStateType::kSummary) {
        u64 counter = 0, sum = 0;
        for (size_t i = 0; i < tasks_.size(); ++i) {
            auto summaryState = dynamic_cast<SummarySinkState *>(tasks_[i]->sink_state_.get());
            counter += summaryState->count_;
            sum += summaryState->sum_;
        }
        result_table = DataTable::MakeSummaryResultTable(counter, sum);
        return result_table;
    } else if (tasks_[0]->sink_state_->state_type() == SinkStateType::kMessage) {
        auto *message_sink_state = static_cast<MessageSinkState *>(tasks_[0]->sink_state_.get());
        if (message_sink_state->message_.get() == nullptr) {
            UnrecoverableError("No response message");
        }

        std::shared_ptr<DataTable> result_table = DataTable::MakeEmptyResultTable();
        result_table->SetResultMsg(std::move(message_sink_state->message_));
        return result_table;
    }

    auto *first_materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
    std::vector<std::shared_ptr<ColumnDef>> column_defs;
    size_t column_count = first_materialize_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for (size_t col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(std::make_shared<ColumnDef>(col_idx,
                                                             first_materialize_sink_state->column_types_->at(col_idx),
                                                             first_materialize_sink_state->column_names_->at(col_idx),
                                                             std::set<ConstraintType>()));
    }

    size_t total_hits_count = 0;
    for (const auto &task : tasks_) {
        if (task->sink_state_->state_type() != SinkStateType::kMaterialize) {
            UnrecoverableError("Parallel materialized fragment will only have common sink state");
        }

        auto *materialize_sink_state = static_cast<MaterializeSinkState *>(task->sink_state_.get());
        if (result_table.get() == nullptr) {
            result_table = DataTable::MakeResultTable(column_defs);
        }
        result_table->total_hits_count_flag_ = materialize_sink_state->total_hits_count_flag_;
        total_hits_count += materialize_sink_state->total_hits_count_;

        for (auto &result_data_block : materialize_sink_state->data_block_array_) {
            result_table->Append(std::move(result_data_block));
        }
        materialize_sink_state->data_block_array_.clear();
    }
    result_table->total_hits_count_ = total_hits_count;

    return result_table;
}

std::shared_ptr<DataTable> ParallelStreamFragmentCtx::GetResultInternal() {
    std::shared_ptr<DataTable> result_table = nullptr;
    for (const auto &task : tasks_) {
        if (!task->sink_state_->status_.ok()) {
            RecoverableError(task->sink_state_->status_);
        }
    }
    if (tasks_[0]->sink_state_->state_type() == SinkStateType::kSummary) {
        u64 counter = 0, sum = 0;
        for (size_t i = 0; i < tasks_.size(); ++i) {
            auto *summaryState = dynamic_cast<SummarySinkState *>(tasks_[i]->sink_state_.get());
            counter += summaryState->count_;
            sum += summaryState->sum_;
        }
        result_table = DataTable::MakeSummaryResultTable(counter, sum);
        return result_table;
    }

    auto *first_materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
    std::vector<std::shared_ptr<ColumnDef>> column_defs;
    size_t column_count = first_materialize_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for (size_t col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(std::make_shared<ColumnDef>(col_idx,
                                                             first_materialize_sink_state->column_types_->at(col_idx),
                                                             first_materialize_sink_state->column_names_->at(col_idx),
                                                             std::set<ConstraintType>()));
    }

    for (const auto &task : tasks_) {
        if (task->sink_state_->state_type() != SinkStateType::kMaterialize) {
            UnrecoverableError("Parallel materialized fragment will only have common sink state");
        }

        auto *materialize_sink_state = static_cast<MaterializeSinkState *>(task->sink_state_.get());
        if (result_table.get() == nullptr) {
            result_table = DataTable::MakeResultTable(column_defs);
        }
        result_table->total_hits_count_flag_ = materialize_sink_state->total_hits_count_flag_;
        result_table->total_hits_count_ = materialize_sink_state->total_hits_count_;

        for (auto &result_data_block : materialize_sink_state->data_block_array_) {
            result_table->Append(std::move(result_data_block));
        }
        materialize_sink_state->data_block_array_.clear();
    }

    return result_table;
}

void FragmentContext::DumpFragmentCtx() {
    for (auto &task : tasks_) {
        LOG_TRACE(fmt::format("Task id: {}, status: {}", task->TaskID(), FragmentTaskStatus2String(task->status())));
    }
    for (auto iter = plan_fragment_ptr_->GetOperators().begin(); iter != plan_fragment_ptr_->GetOperators().end(); ++iter) {
        LOG_TRACE(fmt::format("Operator type: {}", PhysicalOperatorToString((*iter)->operator_type())));
    }
}

} // namespace infinity
