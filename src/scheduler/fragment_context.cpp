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

#include <vector>

module fragment_context;

import stl;

import fragment_task;

import infinity_exception;
import operator_state;
import physical_operator;
import physical_operator_type;

import table_scan_function_data;
import match_tensor_scan_function_data;
import match_sparse_scan_function_data;
import knn_scan_data;
import physical_table_scan;
import physical_index_scan;
import physical_knn_scan;
import physical_aggregate;
import physical_explain;
import physical_create_index_prepare;
import physical_create_index_do;
import physical_sort;
import physical_top;
import physical_merge_top;
import physical_match_tensor_scan;
import physical_match_sparse_scan;
import physical_compact;
import physical_compact_index_prepare;
import physical_compact_index_do;
import physical_compact_finish;

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
import create_index_data;
import compact_state_data;
import logger;
import task_scheduler;
import plan_fragment;
import aggregate_expression;
import expression_state;
import column_def;
import explain_statement;
import table_entry;
import segment_entry;

namespace infinity {

template <typename OperatorStateType>
UniquePtr<OperatorState> MakeTaskStateTemplate(PhysicalOperator *physical_op) {

    return MakeUnique<OperatorStateType>();
}

UniquePtr<OperatorState> MakeCreateIndexDoState(PhysicalCreateIndexDo *physical_create_index_do, FragmentTask *task, FragmentContext *fragment_ctx) {
    UniquePtr<CreateIndexDoOperatorState> operator_state = MakeUnique<CreateIndexDoOperatorState>();
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
    operator_state->create_index_shared_data_ = parallel_materialize_fragment_ctx->create_index_shared_data_.get();
    return operator_state;
}

UniquePtr<OperatorState> MakeTableScanState(PhysicalTableScan *physical_table_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();

    if (source_state->state_type_ != SourceStateType::kTableScan) {
        String error_message = "Expect table scan source state";
        UnrecoverableError(error_message);
    }

    auto *table_scan_source_state = static_cast<TableScanSourceState *>(source_state);

    UniquePtr<OperatorState> operator_state = MakeUnique<TableScanOperatorState>();
    TableScanOperatorState *table_scan_op_state_ptr = (TableScanOperatorState *)(operator_state.get());
    table_scan_op_state_ptr->table_scan_function_data_ = MakeUnique<TableScanFunctionData>(physical_table_scan->GetBlockIndex(),
                                                                                           table_scan_source_state->global_ids_,
                                                                                           physical_table_scan->ColumnIDs());
    return operator_state;
}

UniquePtr<OperatorState> MakeMatchTensorScanState(const PhysicalMatchTensorScan *physical_match_tensor_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kMatchTensorScan) {
        String error_message = "Expect MatchTensorScan source state";
        UnrecoverableError(error_message);
    }
    auto operator_state = MakeUnique<MatchTensorScanOperatorState>();
    operator_state->match_tensor_scan_function_data_ = MakeUnique<MatchTensorScanFunctionData>(physical_match_tensor_scan->GetTopN());
    return operator_state;
}

UniquePtr<OperatorState> MakeMergeMatchTensorState(PhysicalOperator *physical_op) {
    auto operator_state = MakeUnique<MergeMatchTensorOperatorState>();
    return operator_state;
}

UniquePtr<OperatorState> MakeMatchSparseScanState(const PhysicalMatchSparseScan *physical_match_sparse_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    auto operator_state = MakeUnique<MatchSparseScanOperatorState>();
    auto *match_sparse_scan_source_state = static_cast<MatchSparseScanSourceState *>(source_state);
    operator_state->match_sparse_scan_function_data_ =
        MatchSparseScanFunctionData(match_sparse_scan_source_state->global_ids_, match_sparse_scan_source_state->segment_ids_);
    return operator_state;
}

UniquePtr<OperatorState> MakeMergeMatchSparseState(PhysicalOperator *physical_op) {
    auto operator_state = MakeUnique<MergeMatchSparseOperatorState>();
    return operator_state;
}

UniquePtr<OperatorState> MakeIndexScanState(PhysicalIndexScan *physical_index_scan, FragmentTask *task) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kIndexScan) {
        String error_message = "Expect index scan source state";
        UnrecoverableError(error_message);
    }
    auto *index_scan_source_state = static_cast<IndexScanSourceState *>(source_state);
    return MakeUnique<IndexScanOperatorState>(std::move(index_scan_source_state->segment_ids_));
}

UniquePtr<OperatorState> MakeKnnScanState(PhysicalKnnScan *physical_knn_scan, FragmentTask *task, FragmentContext *fragment_ctx) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kKnnScan) {
        String error_message = "Expect knn scan source state";
        UnrecoverableError(error_message);
    }

    UniquePtr<OperatorState> operator_state = MakeUnique<KnnScanOperatorState>();
    KnnScanOperatorState *knn_scan_op_state_ptr = (KnnScanOperatorState *)(operator_state.get());
    const bool execute_block_scan_job = task->TaskID() < static_cast<i64>(physical_knn_scan->BlockScanTaskCount());
    switch (fragment_ctx->ContextType()) {
        case FragmentType::kSerialMaterialize: {
            SerialMaterializedFragmentCtx *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_ctx);
            knn_scan_op_state_ptr->knn_scan_function_data_ =
                MakeUnique<KnnScanFunctionData>(serial_materialize_fragment_ctx->knn_scan_shared_data_.get(), task->TaskID(), execute_block_scan_job);
            break;
        }
        case FragmentType::kParallelMaterialize: {
            ParallelMaterializedFragmentCtx *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
            knn_scan_op_state_ptr->knn_scan_function_data_ =
                MakeUnique<KnnScanFunctionData>(parallel_materialize_fragment_ctx->knn_scan_shared_data_.get(),
                                                task->TaskID(),
                                                execute_block_scan_job);
            break;
        }
        default: {
            String error_message = "Invalid fragment type.";
            UnrecoverableError(error_message);
        }
    }

    return operator_state;
}

UniquePtr<OperatorState> MakeCompactState(PhysicalCompact *physical_compact, FragmentTask *task, FragmentContext *fragment_ctx) {
    SourceState *source_state = task->source_state_.get();
    if (source_state->state_type_ != SourceStateType::kCompact) {
        String error_message = "Expect compact source state";
        UnrecoverableError(error_message);
    }
    auto *compact_source_state = static_cast<CompactSourceState *>(source_state);

    if (fragment_ctx->ContextType() != FragmentType::kParallelMaterialize) {
        String error_message = "Compact operator should be in parallel materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
    auto compact_operator_state =
        MakeUnique<CompactOperatorState>(std::move(compact_source_state->segment_groups_), parallel_materialize_fragment_ctx->compact_state_data_);
    return compact_operator_state;
}

UniquePtr<OperatorState>
MakeCompactIndexPrepareState(PhysicalCompactIndexPrepare *physical_compact_index_prepare, FragmentTask *task, FragmentContext *fragment_ctx) {
    if (fragment_ctx->ContextType() != FragmentType::kSerialMaterialize) {
        String error_message = "Compact index prepare operator should be in parallel materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_ctx);
    auto compact_index_prepare_operator_state =
        MakeUnique<CompactIndexPrepareOperatorState>(serial_materialize_fragment_ctx->compact_state_data_,
                                                     serial_materialize_fragment_ctx->create_index_shared_data_array_);
    return compact_index_prepare_operator_state;
}

UniquePtr<OperatorState>
MakeCompactIndexDoState(PhysicalCompactIndexDo *physical_compact_index_do, FragmentTask *task, FragmentContext *fragment_ctx) {
    if (fragment_ctx->ContextType() != FragmentType::kParallelMaterialize) {
        String error_message = "Compact index do operator should be in parallel materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
    auto compact_index_do_operator_state =
        MakeUnique<CompactIndexDoOperatorState>(parallel_materialize_fragment_ctx->compact_state_data_,
                                                parallel_materialize_fragment_ctx->create_index_shared_data_array_);
    return compact_index_do_operator_state;
}

UniquePtr<OperatorState> MakeCompactFinishState(PhysicalCompactFinish *physical_compact_finish, FragmentContext *fragment_ctx) {
    if (fragment_ctx->ContextType() != FragmentType::kSerialMaterialize) {
        String error_message = "Compact finish operator should be in serial materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_ctx);
    auto compact_finish_operator_state = MakeUnique<CompactFinishOperatorState>(serial_materialize_fragment_ctx->compact_state_data_);
    return compact_finish_operator_state;
}

UniquePtr<OperatorState> MakeAggregateState(PhysicalAggregate *physical_aggregate, FragmentTask *task) {
    Vector<UniquePtr<char[]>> states;
    for (auto &expr : physical_aggregate->aggregates_) {
        auto agg_expr = std::static_pointer_cast<AggregateExpression>(expr);
        states.push_back(agg_expr->aggregate_function_.InitState());
    }
    return MakeUnique<AggregateOperatorState>(std::move(states));
}

UniquePtr<OperatorState> MakeMergeKnnState(PhysicalMergeKnn *physical_merge_knn, FragmentTask *task) {
    KnnExpression *knn_expr = physical_merge_knn->knn_expression_.get();
    UniquePtr<OperatorState> operator_state = MakeUnique<MergeKnnOperatorState>();
    MergeKnnOperatorState *merge_knn_op_state_ptr = (MergeKnnOperatorState *)(operator_state.get());
    // Set fake parallel number here. It will be set in SetMergeKnnState
    merge_knn_op_state_ptr->merge_knn_function_data_ = MakeShared<MergeKnnFunctionData>(1,
                                                                                        knn_expr->topn_,
                                                                                        knn_expr->embedding_data_type_,
                                                                                        knn_expr->distance_type_,
                                                                                        physical_merge_knn->base_table_ref_);

    return operator_state;
}

UniquePtr<OperatorState> MakeSortState(PhysicalOperator *physical_op) {
    auto operator_state = MakeUnique<SortOperatorState>();
    auto &expr_states = operator_state->expr_states_;
    auto &sort_expressions = (static_cast<PhysicalSort *>(physical_op))->GetSortExpressions();
    expr_states.reserve(sort_expressions.size());
    for (auto &expr : sort_expressions) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    return operator_state;
}

UniquePtr<OperatorState> MakeTopState(PhysicalOperator *physical_op) {
    auto operator_state = MakeUnique<TopOperatorState>();
    auto &expr_states = operator_state->expr_states_;
    auto &sort_expressions = (static_cast<PhysicalTop *>(physical_op))->GetSortExpressions();
    expr_states.reserve(sort_expressions.size());
    for (auto &expr : sort_expressions) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    return operator_state;
}

UniquePtr<OperatorState> MakeMergeTopState(PhysicalOperator *physical_op) {
    auto operator_state = MakeUnique<MergeTopOperatorState>();
    auto &expr_states = operator_state->expr_states_;
    auto &sort_expressions = (static_cast<PhysicalMergeTop *>(physical_op))->GetSortExpressions();
    expr_states.reserve(sort_expressions.size());
    for (auto &expr : sort_expressions) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    return operator_state;
}

UniquePtr<OperatorState>
MakeTaskState(SizeT operator_id, const Vector<PhysicalOperator *> &physical_ops, FragmentTask *task, FragmentContext *fragment_ctx) {
    switch (physical_ops[operator_id]->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            String error_message = "Invalid physical operator type";
            UnrecoverableError(error_message);
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            if (operator_id != physical_ops.size() - 1) {
                String error_message = "Table scan operator must be the first operator of the fragment.";
                UnrecoverableError(error_message);
            }

            if (operator_id == 0) {
                String error_message = "Table scan shouldn't be the last operator of the fragment.";
                UnrecoverableError(error_message);
            }
            auto physical_table_scan = static_cast<PhysicalTableScan *>(physical_ops[operator_id]);
            return MakeTableScanState(physical_table_scan, task);
        }
        case PhysicalOperatorType::kKnnScan: {
            auto physical_knn_scan = static_cast<PhysicalKnnScan *>(physical_ops[operator_id]);
            return MakeKnnScanState(physical_knn_scan, task, fragment_ctx);
        }
        case PhysicalOperatorType::kAggregate: {
            auto physical_aggregate = static_cast<PhysicalAggregate *>(physical_ops[operator_id]);
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
        case PhysicalOperatorType::kIndexScan: {
            if (operator_id != physical_ops.size() - 1) {
                String error_message = "Table scan operator must be the first operator of the fragment.";
                UnrecoverableError(error_message);
            }

            if (operator_id == 0) {
                String error_message = "Table scan shouldn't be the last operator of the fragment.";
                UnrecoverableError(error_message);
            }
            auto physical_index_scan = static_cast<PhysicalIndexScan *>(physical_ops[operator_id]);
            return MakeIndexScanState(physical_index_scan, task);
        }
        case PhysicalOperatorType::kMergeKnn: {
            auto physical_merge_knn = static_cast<PhysicalMergeKnn *>(physical_ops[operator_id]);
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
            auto physical_match_tensor_scan = static_cast<PhysicalMatchTensorScan *>(physical_ops[operator_id]);
            return MakeMatchTensorScanState(physical_match_tensor_scan, task);
        }
        case PhysicalOperatorType::kMergeMatchTensor: {
            return MakeMergeMatchTensorState(physical_ops[operator_id]);
        }
        case PhysicalOperatorType::kMatchSparseScan: {
            auto physical_match_sparse_scan = static_cast<PhysicalMatchSparseScan *>(physical_ops[operator_id]);
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
        case PhysicalOperatorType::kCreateIndexDo: {
            auto *physical_create_index_do = static_cast<PhysicalCreateIndexDo *>(physical_ops[operator_id]);
            return MakeCreateIndexDoState(physical_create_index_do, task, fragment_ctx);
        }
        case PhysicalOperatorType::kCreateIndexFinish: {
            return MakeTaskStateTemplate<CreateIndexFinishOperatorState>(physical_ops[operator_id]);
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
        case PhysicalOperatorType::kCompactIndexPrepare: {
            auto *physical_compact_index_prepare = static_cast<PhysicalCompactIndexPrepare *>(physical_ops[operator_id]);
            return MakeCompactIndexPrepareState(physical_compact_index_prepare, task, fragment_ctx);
        }
        case PhysicalOperatorType::kCompactIndexDo: {
            auto *physical_compact_index_do = static_cast<PhysicalCompactIndexDo *>(physical_ops[operator_id]);
            return MakeCompactIndexDoState(physical_compact_index_do, task, fragment_ctx);
        }
        case PhysicalOperatorType::kCompactFinish: {
            auto *physical_compact_finish = static_cast<PhysicalCompactFinish *>(physical_ops[operator_id]);
            return MakeCompactFinishState(physical_compact_finish, fragment_ctx);
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
        default: {
            String error_message = fmt::format("Not support {} now", PhysicalOperatorToString(physical_ops[operator_id]->operator_type()));
            UnrecoverableError(error_message);
        }
    }
    return nullptr;
}

void CollectTasks(Vector<SharedPtr<String>> &result, PlanFragment *plan_fragment_ptr) {
    if (plan_fragment_ptr->GetContext() == nullptr) {
        return;
    }

    auto tasks = &plan_fragment_ptr->GetContext()->Tasks();
    {
        String fragment_header = "Fragment #";

        fragment_header += std::to_string(plan_fragment_ptr->FragmentID());
        fragment_header += " * ";
        fragment_header += std::to_string(tasks->size());
        fragment_header += " Task";

        result.emplace_back(MakeShared<String>(fragment_header));
    }
    for (const auto &task : *tasks) {
        result.emplace_back(MakeShared<String>(fmt::format("-> Task #{}", task->TaskID())));
    }
    // NOTE: Insert blank elements after each Fragment for alignment
    result.emplace_back(MakeShared<String>());

    if (plan_fragment_ptr->HasChild()) {
        // current fragment have children
        for (const auto &child_fragment : plan_fragment_ptr->Children()) {
            CollectTasks(result, child_fragment.get());
        }
    }
}

void FragmentContext::BuildTask(QueryContext *query_context, FragmentContext *parent_context, PlanFragment *plan_fragment_ptr, Notifier *notifier) {
    Vector<PhysicalOperator *> &fragment_operators = plan_fragment_ptr->GetOperators();
    i64 operator_count = fragment_operators.size();
    if (operator_count < 1) {
        String error_message = "No operators in the fragment.";
        UnrecoverableError(error_message);
    }

    UniquePtr<FragmentContext> fragment_context = nullptr;
    switch (plan_fragment_ptr->GetFragmentType()) {
        case FragmentType::kInvalid: {
            String error_message = "Invalid fragment type";
            UnrecoverableError(error_message);
        }
        case FragmentType::kSerialMaterialize: {
            fragment_context = MakeUnique<SerialMaterializedFragmentCtx>(plan_fragment_ptr, query_context, notifier);
            break;
        }
        case FragmentType::kParallelMaterialize: {
            fragment_context = MakeUnique<ParallelMaterializedFragmentCtx>(plan_fragment_ptr, query_context, notifier);
            break;
        }
        case FragmentType::kParallelStream: {
            fragment_context = MakeUnique<ParallelStreamFragmentCtx>(plan_fragment_ptr, query_context, notifier);
            break;
        }
    }

    // Set parallel size
    i64 parallel_size = static_cast<i64>(query_context->cpu_number_limit());
    // i64 parallel_size = 1;

    fragment_context->CreateTasks(parallel_size, operator_count, parent_context);

    Vector<UniquePtr<FragmentTask>> &tasks = fragment_context->Tasks();
    i64 real_parallel_size = tasks.size();

    for (i64 operator_id = operator_count - 1; operator_id >= 0; --operator_id) {

        // PhysicalOperator *physical_op = fragment_operators[operator_id];
        for (SizeT task_id = 0; task_id < tasks.size(); ++task_id) {
            FragmentTask *task = tasks[task_id].get();

            // build the input and output state of each opeartor
            UniquePtr<OperatorState> operator_state = MakeTaskState(operator_id, fragment_operators, task, fragment_context.get());

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
                            String error_message = "Invalid sink operator state type.";
                            UnrecoverableError(error_message);
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
            Vector<SharedPtr<String>> result;
            PhysicalExplain *explain_op = (PhysicalExplain *)fragment_operators[0];

            if (explain_op->explain_type() == ExplainType::kPipeline) {
                CollectTasks(result, plan_fragment_ptr->Children()[0].get());
                explain_op->SetExplainTaskText(MakeShared<Vector<SharedPtr<String>>>(result));
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
      unfinished_child_n_(plan_fragment_ptr->Children().size()) {}

bool FragmentContext::TryFinishFragment() {
    auto fragment_id = plan_fragment_ptr_->FragmentID();
    auto parent_plan_fragments = plan_fragment_ptr_->GetParents();

    if (!TryFinishFragmentInner()) {
        LOG_TRACE(fmt::format("{} tasks in fragment {} are not completed", unfinished_task_n_.load(), fragment_id));
        if (fragment_type_ == FragmentType::kParallelStream) {
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

Vector<PhysicalOperator *> &FragmentContext::GetOperators() { return plan_fragment_ptr_->GetOperators(); }

PhysicalSink *FragmentContext::GetSinkOperator() const { return plan_fragment_ptr_->GetSinkNode(); }

PhysicalSource *FragmentContext::GetSourceOperator() const { return plan_fragment_ptr_->GetSourceNode(); }

SizeT InitKnnScanFragmentContext(PhysicalKnnScan *knn_scan_operator, FragmentContext *fragment_context, QueryContext *query_context) {

    SizeT task_n = knn_scan_operator->TaskletCount();
    KnnExpression *knn_expr = knn_scan_operator->knn_expression_.get();
    switch (fragment_context->ContextType()) {
        case FragmentType::kSerialMaterialize: {
            SerialMaterializedFragmentCtx *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_context);
            serial_materialize_fragment_ctx->knn_scan_shared_data_ =
                MakeUnique<KnnScanSharedData>(knn_scan_operator->base_table_ref_,
                                              std::move(knn_scan_operator->block_column_entries_),
                                              std::move(knn_scan_operator->index_entries_),
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
            ParallelMaterializedFragmentCtx *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_context);
            parallel_materialize_fragment_ctx->knn_scan_shared_data_ =
                MakeUnique<KnnScanSharedData>(knn_scan_operator->base_table_ref_,
                                              std::move(knn_scan_operator->block_column_entries_),
                                              std::move(knn_scan_operator->index_entries_),
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
            String error_message = "Invalid fragment type.";
            UnrecoverableError(error_message);
        }
    }

    return task_n;
}

SizeT InitCreateIndexDoFragmentContext(const PhysicalCreateIndexDo *create_index_do_operator, FragmentContext *fragment_ctx) {
    auto *table_ref = create_index_do_operator->base_table_ref_.get();
    // FIXME: to create index on unsealed_segment
    SizeT segment_cnt = table_ref->block_index_->SegmentCount();

    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_ctx);
    parallel_materialize_fragment_ctx->create_index_shared_data_ = MakeUnique<CreateIndexSharedData>(table_ref->block_index_.get());
    return segment_cnt;
}

SizeT InitCompactFragmentContext(PhysicalCompact *compact_operator, FragmentContext *fragment_context, FragmentContext *parent_context) {
    SizeT task_n = compact_operator->TaskletCount();
    if (fragment_context->ContextType() != FragmentType::kParallelMaterialize) {
        String error_message = "Compact operator should be in parallel materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_context);
    if (parent_context->ContextType() != FragmentType::kSerialMaterialize) {
        String error_message = "Compact operator parent should be in serial materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *parent_serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(parent_context);

    auto &compact_state_data = parent_serial_materialize_fragment_ctx->compact_state_data_;
    parallel_materialize_fragment_ctx->compact_state_data_ = compact_state_data;
    return task_n;
}

SizeT InitCompactIndexPrepareFragmentContext(PhysicalCompactIndexPrepare *compact_index_prepare_operator,
                                             FragmentContext *fragment_context,
                                             FragmentContext *parent_context) {
    SizeT task_n = compact_index_prepare_operator->TaskletCount();
    if (fragment_context->ContextType() != FragmentType::kSerialMaterialize) {
        String error_message = "Compact index prepare operator should be in parallel materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_context);
    if (parent_context->ContextType() == FragmentType::kSerialMaterialize) {
        auto *parent_serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(parent_context);
        serial_materialize_fragment_ctx->compact_state_data_ = parent_serial_materialize_fragment_ctx->compact_state_data_;
    } else {
        auto *parent_parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(parent_context);
        serial_materialize_fragment_ctx->compact_state_data_ = parent_parallel_materialize_fragment_ctx->compact_state_data_;
        serial_materialize_fragment_ctx->create_index_shared_data_array_ = parent_parallel_materialize_fragment_ctx->create_index_shared_data_array_;
    }

    return task_n;
}

void InitCompactIndexDoFragmentContext(PhysicalCompactIndexDo *compact_index_do_operator,
                                       FragmentContext *fragment_context,
                                       FragmentContext *parent_context) {
    if (fragment_context->ContextType() != FragmentType::kParallelMaterialize) {
        String error_message = "Compact index do operator should be in parallel materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *parallel_materialize_fragment_ctx = static_cast<ParallelMaterializedFragmentCtx *>(fragment_context);
    if (parent_context->ContextType() != FragmentType::kSerialMaterialize) {
        String error_message = "Compact index do operator parent should be in serial materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *parent_serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(parent_context);
    parallel_materialize_fragment_ctx->compact_state_data_ = parent_serial_materialize_fragment_ctx->compact_state_data_;

    auto *table_ref = compact_index_do_operator->base_table_ref_.get();
    SizeT index_size = table_ref->index_index_->index_snapshots_vec_.size();
    parallel_materialize_fragment_ctx->create_index_shared_data_array_ = MakeShared<Vector<UniquePtr<CreateIndexSharedData>>>();
    for (SizeT i = 0; i < index_size; ++i) {
        parallel_materialize_fragment_ctx->create_index_shared_data_array_->emplace_back(MakeUnique<CreateIndexSharedData>());
    }
}

void InitCompactFinishFragmentContext(PhysicalCompactFinish *compact_finish_operator, FragmentContext *fragment_context) {
    if (fragment_context->ContextType() != FragmentType::kSerialMaterialize) {
        String error_message = "Compact finish operator should be in serial materialized fragment.";
        UnrecoverableError(error_message);
    }
    auto *serial_materialize_fragment_ctx = static_cast<SerialMaterializedFragmentCtx *>(fragment_context);
    TableEntry *table_entry = compact_finish_operator->base_table_ref_->table_entry_ptr_;
    serial_materialize_fragment_ctx->compact_state_data_ = MakeShared<CompactStateData>(table_entry);
}

void FragmentContext::MakeSourceState(i64 parallel_count) {
    PhysicalOperator *first_operator = this->GetOperators().back();
    switch (first_operator->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            String error_message = "Unexpected operator type";
            UnrecoverableError(error_message);
        }
        case PhysicalOperatorType::kAggregate: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            // Partition the hash range to each source state
            auto *aggregate_operator = (PhysicalAggregate *)first_operator;
            Vector<HashRange> hash_range = aggregate_operator->GetHashRanges(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<AggregateSourceState>(hash_range[task_id].start_, hash_range[task_id].end_);
            }
            break;
        }
        case PhysicalOperatorType::kProjection: {
            if (this->GetOperators().size() == 1) {
                // Only one operator and it's project
                tasks_[0]->source_state_ = MakeUnique<EmptySourceState>();
            } else {
                String error_message = "Project shouldn't be the first operator of the fragment";
                UnrecoverableError(error_message);
            }
            break;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kFilter:
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
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            tasks_[0]->source_state_ = MakeUnique<QueueSourceState>();
            break;
        }
        case PhysicalOperatorType::kCompact: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            auto *physical_compact = static_cast<PhysicalCompact *>(first_operator);
            Vector<Vector<Vector<SegmentEntry *>>> segment_groups_list = physical_compact->PlanCompact(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<CompactSourceState>(std::move(segment_groups_list[task_id]));
            }
            break;
        }
        case PhysicalOperatorType::kCreateIndexDo:
        case PhysicalOperatorType::kCompactIndexDo: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            for (auto &task : tasks_) {
                task->source_state_ = MakeUnique<EmptySourceState>();
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
            String error_message = fmt::format("Not support {} now", PhysicalOperatorToString(first_operator->operator_type()));
            UnrecoverableError(error_message);
            break;
        }
        case PhysicalOperatorType::kTableScan: {
            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            // Partition the hash range to each source state
            auto *table_scan_operator = (PhysicalTableScan *)first_operator;
            Vector<SharedPtr<Vector<GlobalBlockID>>> blocks_group = table_scan_operator->PlanBlockEntries(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<TableScanSourceState>(blocks_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kMatchTensorScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<MatchTensorScanSourceState>();
            }
            break;
        }
        case PhysicalOperatorType::kMatchSparseScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }
            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }
            auto *match_sparse_scan_operator = static_cast<PhysicalMatchSparseScan *>(first_operator);
            Vector<SharedPtr<Vector<GlobalBlockID>>> blocks_group = match_sparse_scan_operator->PlanBlockEntries(parallel_count);
            Vector<SharedPtr<Vector<SegmentID>>> segment_group = match_sparse_scan_operator->PlanWithIndex(blocks_group, parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<MatchSparseScanSourceState>(std::move(blocks_group[task_id]), segment_group[task_id]);
            }
            break;
        }
        case PhysicalOperatorType::kIndexScan: {
            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            // Partition the hash range to each source state
            auto *index_scan_operator = (PhysicalIndexScan *)first_operator;
            Vector<UniquePtr<Vector<SegmentID>>> segment_ids = index_scan_operator->PlanSegments(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<IndexScanSourceState>(std::move(segment_ids[task_id]));
            }
            break;
        }
        case PhysicalOperatorType::kKnnScan: {
            if (fragment_type_ != FragmentType::kParallelMaterialize && fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel/serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            for (SizeT task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                tasks_[task_id]->source_state_ = MakeUnique<KnnScanSourceState>();
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
        case PhysicalOperatorType::kCreateIndexFinish:
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
        case PhysicalOperatorType::kCompactFinish:
        case PhysicalOperatorType::kCompactIndexPrepare:
        case PhysicalOperatorType::kReadCache: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(first_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(first_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            tasks_[0]->source_state_ = MakeUnique<EmptySourceState>();
            break;
        }

        default: {
            String error_message = fmt::format("Unexpected operator type: {}", PhysicalOperatorToString(first_operator->operator_type()));
            UnrecoverableError(error_message);
        }
    }
}

void FragmentContext::MakeSinkState(i64 parallel_count) {
    PhysicalOperator *first_operator = this->GetOperators().back();
    PhysicalOperator *last_operator = this->GetOperators().front();
    switch (last_operator->operator_type()) {

        case PhysicalOperatorType::kInvalid: {
            String error_message = "Unexpected operator type";
            UnrecoverableError(error_message);
        }
        case PhysicalOperatorType::kAggregate: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                String error_message = fmt::format("{} should in parallel stream fragment", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                auto sink_state = MakeUnique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);

                tasks_[task_id]->sink_state_ = std::move(sink_state);
            }
            break;
        }
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kHash: {
            if (fragment_type_ != FragmentType::kParallelStream) {
                String error_message = fmt::format("{} should in parallel stream fragment", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                auto sink_state = MakeUnique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
                sink_state->column_types_ = last_operator->GetOutputTypes();
                sink_state->column_names_ = last_operator->GetOutputNames();

                tasks_[task_id]->sink_state_ = std::move(sink_state);
            }
            break;
        }
        case PhysicalOperatorType::kLimit: {
            if (fragment_type_ != FragmentType::kParallelStream) {
                String error_message = fmt::format("{} should in parallel stream fragment", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                auto sink_state = MakeUnique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);

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
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            tasks_[0]->sink_state_ = MakeUnique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), 0);
            break;
        }

        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kShow: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            tasks_[0]->sink_state_ = MakeUnique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), 0);
            MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
            sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
            sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            break;
        }
        case PhysicalOperatorType::kMatch: {
            for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
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

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<QueueSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
            }
            break;
        }
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kIndexScan: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized/stream fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if ((i64)tasks_.size() != parallel_count) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
                MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[task_id]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            }
            break;
        }
        case PhysicalOperatorType::kProjection: {
            if (fragment_type_ == FragmentType::kSerialMaterialize) {
                if (tasks_.size() != 1) {
                    String error_message = "SerialMaterialize type fragment should only have 1 task.";
                    UnrecoverableError(error_message);
                }

                tasks_[0]->sink_state_ = MakeUnique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), 0);
                MaterializeSinkState *sink_state_ptr = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
                sink_state_ptr->column_types_ = last_operator->GetOutputTypes();
                sink_state_ptr->column_names_ = last_operator->GetOutputNames();
            } else {
                if ((i64)tasks_.size() != parallel_count) {
                    String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                    UnrecoverableError(error_message);
                }

                for (u64 task_id = 0; (i64)task_id < parallel_count; ++task_id) {
                    tasks_[task_id]->sink_state_ = MakeUnique<MaterializeSinkState>(plan_fragment_ptr_->FragmentID(), task_id);
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
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kPreparedPlan: {
            String error_message = fmt::format("Not support {} now", PhysicalOperatorToString(last_operator->operator_type()));
            UnrecoverableError(error_message);
        }
        case PhysicalOperatorType::kDelete:
        case PhysicalOperatorType::kUpdate: {
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_[task_id]->sink_state_ = MakeUnique<SummarySinkState>();
            }
            break;
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            auto *create_index_prepare_operator = static_cast<const PhysicalCreateIndexPrepare *>(last_operator);
            if (!create_index_prepare_operator->prepare_) {
                tasks_[0]->sink_state_ = MakeUnique<ResultSinkState>();
                break;
            }
        }
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport:
        case PhysicalOperatorType::kCompactIndexPrepare: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            tasks_[0]->sink_state_ = MakeUnique<MessageSinkState>();
            break;
        }
        case PhysicalOperatorType::kCreateIndexDo:
        case PhysicalOperatorType::kCompact:
        case PhysicalOperatorType::kCompactIndexDo: {
            if (fragment_type_ != FragmentType::kParallelMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in parallel materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }
            for (auto &task : tasks_) {
                task->sink_state_ = MakeUnique<MessageSinkState>();
            }
            break;
        }
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kCommand:
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateIndexFinish:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateDatabase:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropIndex:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropDatabase:
        case PhysicalOperatorType::kDropView:
        case PhysicalOperatorType::kOptimize:
        case PhysicalOperatorType::kFlush:
        case PhysicalOperatorType::kCompactFinish: {
            if (fragment_type_ != FragmentType::kSerialMaterialize) {
                UnrecoverableError(
                    fmt::format("{} should in serial materialized fragment", PhysicalOperatorToString(last_operator->operator_type())));
            }

            if (tasks_.size() != 1) {
                String error_message = fmt::format("{} task count isn't correct.", PhysicalOperatorToString(last_operator->operator_type()));
                UnrecoverableError(error_message);
            }

            tasks_[0]->sink_state_ = MakeUnique<ResultSinkState>();
            break;
        }
        default: {
            String error_message = fmt::format("Unexpected operator type: {}", PhysicalOperatorToString(last_operator->operator_type()));
            UnrecoverableError(error_message);
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
            SizeT task_n = InitKnnScanFragmentContext(knn_scan_operator, this, query_context_);
            parallel_count = std::min(parallel_count, (i64)task_n);
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kMatch:
        case PhysicalOperatorType::kMergeKnn:
        case PhysicalOperatorType::kMergeMatchTensor:
        case PhysicalOperatorType::kMergeMatchSparse:
        case PhysicalOperatorType::kProjection: {
            // Serial Materialize
            parallel_count = 1;
            break;
        }
        case PhysicalOperatorType::kCreateIndexDo: {
            const auto *create_index_do_operator = static_cast<const PhysicalCreateIndexDo *>(first_operator);
            InitCreateIndexDoFragmentContext(create_index_do_operator, this);
            parallel_count = std::max(parallel_count, (i64)1l);
            break;
        }
        case PhysicalOperatorType::kCompact: {
            auto *compact_operator = static_cast<PhysicalCompact *>(first_operator);
            SizeT task_n = InitCompactFragmentContext(compact_operator, this, parent_context);
            parallel_count = std::min(parallel_count, (i64)task_n);
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kCompactIndexPrepare: {
            auto *compact_index_prepare_operator = static_cast<PhysicalCompactIndexPrepare *>(first_operator);
            SizeT task_n = InitCompactIndexPrepareFragmentContext(compact_index_prepare_operator, this, parent_context);
            parallel_count = std::min(parallel_count, (i64)task_n);
            if (parallel_count == 0) {
                parallel_count = 1;
            }
            break;
        }
        case PhysicalOperatorType::kCompactIndexDo: {
            auto *compact_index_do_operator = static_cast<PhysicalCompactIndexDo *>(first_operator);
            InitCompactIndexDoFragmentContext(compact_index_do_operator, this, parent_context);
            parallel_count = std::max(parallel_count, (i64)1l);
            break;
        }
        case PhysicalOperatorType::kCompactFinish: {
            auto *compact_finish_operator = static_cast<PhysicalCompactFinish *>(first_operator);
            InitCompactFinishFragmentContext(compact_finish_operator, this);
            parallel_count = 1;
        }
        default: {
            break;
        }
    }

    switch (fragment_type_) {
        case FragmentType::kInvalid: {
            String error_message = "Invalid fragment type";
            UnrecoverableError(error_message);
        }
        case FragmentType::kSerialMaterialize: {
            parallel_count = 1;
            tasks_.reserve(parallel_count);
            tasks_.emplace_back(MakeUnique<FragmentTask>(this, 0, operator_count));
            IncreaseTask();
            break;
        }
        case FragmentType::kParallelMaterialize:
        case FragmentType::kParallelStream: {
            tasks_.reserve(parallel_count);
            for (i64 task_id = 0; task_id < parallel_count; ++task_id) {
                tasks_.emplace_back(MakeUnique<FragmentTask>(this, task_id, operator_count));
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

SharedPtr<DataTable> SerialMaterializedFragmentCtx::GetResultInternal() {
    // Only one sink state
    if (tasks_.size() != 1) {
        String error_message = "There should be one sink state in serial materialized fragment";
        UnrecoverableError(error_message);
    }

    if (tasks_[0]->sink_state_->Ignore()) {
        SharedPtr<DataTable> result_table = DataTable::MakeEmptyResultTable();
        result_table->SetResultMsg(MakeUnique<String>("Ignore error"));
        return result_table;
    }
    if (tasks_[0]->sink_state_->Error()) {
        RecoverableError(tasks_[0]->sink_state_->status_);
    }

    switch (tasks_[0]->sink_state_->state_type()) {
        case SinkStateType::kInvalid: {
            String error_message = "Invalid sink state type";
            UnrecoverableError(error_message);
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
                                                               std::set<ConstraintType>()));
            }

            SharedPtr<DataTable> result_table = DataTable::MakeResultTable(column_defs);
            for (auto &data_block : materialize_sink_state->data_block_array_) {
                result_table->UpdateRowCount(data_block->row_count());
                result_table->data_blocks_.emplace_back(std::move(data_block));
            }
            //            result_table->data_blocks_ = std::move(materialize_sink_state->data_block_array_);
            return result_table;
        }
        case SinkStateType::kResult: {
            auto *result_sink_state = static_cast<ResultSinkState *>(tasks_[0]->sink_state_.get());
            SharedPtr<DataTable> result_table = DataTable::MakeResultTable({result_sink_state->result_def_});
            return result_table;
        }
        case SinkStateType::kMessage: {
            auto *message_sink_state = static_cast<MessageSinkState *>(tasks_[0]->sink_state_.get());
            if (message_sink_state->message_.get() == nullptr) {
                String error_message = "No response message";
                UnrecoverableError(error_message);
            }

            SharedPtr<DataTable> result_table = DataTable::MakeEmptyResultTable();
            result_table->SetResultMsg(std::move(message_sink_state->message_));
            return result_table;
        }
        case SinkStateType::kSummary: {
            u64 counter = 0, sum = 0;
            for (SizeT i = 0; i < tasks_.size(); ++i) {
                auto summaryState = dynamic_cast<SummarySinkState *>(tasks_[i]->sink_state_.get());
                counter += summaryState->count_;
                sum += summaryState->sum_;
            }
            SharedPtr<DataTable> result_table = DataTable::MakeSummaryResultTable(counter, sum);
            return result_table;
        }
        case SinkStateType::kQueue: {
            String error_message = "Can't get result from Queue sink type.";
            UnrecoverableError(error_message);
        }
    }
    String error_message = "Unreachable";
    UnrecoverableError(error_message);
    return nullptr;
}

SharedPtr<DataTable> ParallelMaterializedFragmentCtx::GetResultInternal() {
    SharedPtr<DataTable> result_table = nullptr;
    for (const auto &task : tasks_) {
        if (!task->sink_state_->status_.ok()) {
            RecoverableError(task->sink_state_->status_);
        }
    }
    if (tasks_[0]->sink_state_->state_type() == SinkStateType::kSummary) {
        u64 counter = 0, sum = 0;
        for (SizeT i = 0; i < tasks_.size(); ++i) {
            auto summaryState = dynamic_cast<SummarySinkState *>(tasks_[i]->sink_state_.get());
            counter += summaryState->count_;
            sum += summaryState->sum_;
        }
        result_table = DataTable::MakeSummaryResultTable(counter, sum);
        return result_table;
    }

    auto *first_materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
    Vector<SharedPtr<ColumnDef>> column_defs;
    SizeT column_count = first_materialize_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                       first_materialize_sink_state->column_types_->at(col_idx),
                                                       first_materialize_sink_state->column_names_->at(col_idx),
                                                       std::set<ConstraintType>()));
    }

    for (const auto &task : tasks_) {
        if (task->sink_state_->state_type() != SinkStateType::kMaterialize) {
            String error_message = "Parallel materialized fragment will only have common sink state";
            UnrecoverableError(error_message);
        }

        auto *materialize_sink_state = static_cast<MaterializeSinkState *>(task->sink_state_.get());
        if (result_table.get() == nullptr) {
            result_table = DataTable::MakeResultTable(column_defs);
        }

        for (auto &result_data_block : materialize_sink_state->data_block_array_) {
            result_table->Append(std::move(result_data_block));
        }
    }

    return result_table;
}

SharedPtr<DataTable> ParallelStreamFragmentCtx::GetResultInternal() {
    SharedPtr<DataTable> result_table = nullptr;
    for (const auto &task : tasks_) {
        if (!task->sink_state_->status_.ok()) {
            RecoverableError(task->sink_state_->status_);
        }
    }
    if (tasks_[0]->sink_state_->state_type() == SinkStateType::kSummary) {
        u64 counter = 0, sum = 0;
        for (SizeT i = 0; i < tasks_.size(); ++i) {
            auto summaryState = dynamic_cast<SummarySinkState *>(tasks_[i]->sink_state_.get());
            counter += summaryState->count_;
            sum += summaryState->sum_;
        }
        result_table = DataTable::MakeSummaryResultTable(counter, sum);
        return result_table;
    }

    auto *first_materialize_sink_state = static_cast<MaterializeSinkState *>(tasks_[0]->sink_state_.get());
    Vector<SharedPtr<ColumnDef>> column_defs;
    SizeT column_count = first_materialize_sink_state->column_names_->size();
    column_defs.reserve(column_count);
    for (SizeT col_idx = 0; col_idx < column_count; ++col_idx) {
        column_defs.emplace_back(MakeShared<ColumnDef>(col_idx,
                                                       first_materialize_sink_state->column_types_->at(col_idx),
                                                       first_materialize_sink_state->column_names_->at(col_idx),
                                                       std::set<ConstraintType>()));
    }

    for (const auto &task : tasks_) {
        if (task->sink_state_->state_type() != SinkStateType::kMaterialize) {
            String error_message = "Parallel materialized fragment will only have common sink state";
            UnrecoverableError(error_message);
        }

        auto *materialize_sink_state = static_cast<MaterializeSinkState *>(task->sink_state_.get());

        if (result_table.get() == nullptr) {
            result_table = DataTable::MakeResultTable(column_defs);
        }

        for (auto &result_data_block : materialize_sink_state->data_block_array_) {
            result_table->Append(std::move(result_data_block));
        }
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
