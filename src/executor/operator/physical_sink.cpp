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

#include <string>

module physical_sink;

import stl;
import query_context;

import operator_state;
import physical_operator_type;
import fragment_context;
import third_party;
import fragment_data;
import data_block;
import status;
import infinity_exception;
import logger;
import logical_type;
import column_def;

namespace infinity {

void PhysicalSink::Init() {}

bool PhysicalSink::Execute(QueryContext *, OperatorState *) { return true; }

bool PhysicalSink::Execute(QueryContext *, FragmentContext *fragment_context, SinkState *sink_state) {
    switch (sink_state->state_type_) {
        case SinkStateType::kInvalid: {
            String error_message = "Invalid sinker type";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case SinkStateType::kMaterialize: {
            // Output general output
            auto *materialize_sink_state = static_cast<MaterializeSinkState *>(sink_state);
            FillSinkStateFromLastOperatorState(materialize_sink_state, materialize_sink_state->prev_op_state_);
            break;
        }
        case SinkStateType::kResult: {
            // Output result
            auto *result_sink_state = static_cast<ResultSinkState *>(sink_state);
            FillSinkStateFromLastOperatorState(result_sink_state, result_sink_state->prev_op_state_);
            break;
        }
        case SinkStateType::kMessage: {
            // Output message
            auto *message_sink_state = static_cast<MessageSinkState *>(sink_state);
            FillSinkStateFromLastOperatorState(message_sink_state, message_sink_state->prev_op_state_);
            break;
        }
        case SinkStateType::kSummary: {
            // Output summary
            auto *summary_sink_state = static_cast<SummarySinkState *>(sink_state);
            FillSinkStateFromLastOperatorState(summary_sink_state, summary_sink_state->prev_op_state_);
            break;
        }
        case SinkStateType::kQueue: {
            QueueSinkState *queue_sink_state = static_cast<QueueSinkState *>(sink_state);
            FillSinkStateFromLastOperatorState(fragment_context, queue_sink_state, queue_sink_state->prev_op_state_);
            break;
        }
    }
    return true;
}

void PhysicalSink::FillSinkStateFromLastOperatorState(MaterializeSinkState *materialize_sink_state, OperatorState *task_op_state) {
    switch (task_op_state->operator_type_) {
        case PhysicalOperatorType::kInvalid: {
            String error_message = "Invalid operator";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        case PhysicalOperatorType::kShow: {
            ShowOperatorState *show_output_state = static_cast<ShowOperatorState *>(task_op_state);
            for (auto &data_block : show_output_state->output_) {
                materialize_sink_state->data_block_array_.emplace_back(std::move(data_block));
            }
            show_output_state->output_.clear();
            break;
        }
        case PhysicalOperatorType::kExplain: {
            ExplainOperatorState *explain_output_state = static_cast<ExplainOperatorState *>(task_op_state);
            if (explain_output_state->data_block_array_.empty()) {
                String error_message = "Empty explain output";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }

            for (auto &data_block : explain_output_state->data_block_array_) {
                materialize_sink_state->data_block_array_.emplace_back(std::move(data_block));
            }
            explain_output_state->data_block_array_.clear();
            break;
        }
        case PhysicalOperatorType::kProjection: {
            ProjectionOperatorState *projection_output_state = static_cast<ProjectionOperatorState *>(task_op_state);
            if (projection_output_state->data_block_array_.empty()) {
                materialize_sink_state->empty_result_ = true;
            } else {
                for (auto &data_block : projection_output_state->data_block_array_) {
                    materialize_sink_state->data_block_array_.emplace_back(std::move(data_block));
                }
                projection_output_state->data_block_array_.clear();
            }
            break;
        }
        case PhysicalOperatorType::kTop: {
            auto top_output_state = static_cast<TopOperatorState *>(task_op_state);
            if (top_output_state->data_block_array_.empty()) {
                if (materialize_sink_state->Error()) {
                    materialize_sink_state->empty_result_ = true;
                } else {
                    String error_message = "Empty sort output";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            } else {
                for (auto &data_block : top_output_state->data_block_array_) {
                    materialize_sink_state->data_block_array_.emplace_back(std::move(data_block));
                }
                top_output_state->data_block_array_.clear();
            }
            break;
        }
        case PhysicalOperatorType::kSort: {
            SortOperatorState *sort_output_state = static_cast<SortOperatorState *>(task_op_state);
            if (sort_output_state->data_block_array_.empty()) {
                if (materialize_sink_state->Error()) {
                    materialize_sink_state->empty_result_ = true;
                } else {
                    String error_message = "Empty sort output";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            } else {
                for (auto &data_block : sort_output_state->data_block_array_) {
                    materialize_sink_state->data_block_array_.emplace_back(std::move(data_block));
                }
                sort_output_state->data_block_array_.clear();
            }
            break;
        }
        case PhysicalOperatorType::kAggregate: {
            AggregateOperatorState *agg_output_state = static_cast<AggregateOperatorState *>(task_op_state);
            if (agg_output_state->data_block_array_.empty()) {
                if (materialize_sink_state->Error()) {
                    materialize_sink_state->empty_result_ = true;
                } else {
                    String error_message = "Empty agg output";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            } else {
                for (auto &data_block : agg_output_state->data_block_array_) {
                    materialize_sink_state->data_block_array_.emplace_back(std::move(data_block));
                }
                agg_output_state->data_block_array_.clear();
            }
            break;
        }
        default: {
            Status status = Status::NotSupport(fmt::format("{} isn't supported here.", PhysicalOperatorToString(task_op_state->operator_type_)));
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
    }
}

void PhysicalSink::FillSinkStateFromLastOperatorState(SummarySinkState *summary_sink_state, OperatorState *task_operator_state) {
    switch (task_operator_state->operator_type_) {
        case PhysicalOperatorType::kInvalid: {
            String error_message = "Invalid operator";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case PhysicalOperatorType::kDelete: {
            DeleteOperatorState *delete_operator_state = static_cast<DeleteOperatorState *>(task_operator_state);
            summary_sink_state->count_ = delete_operator_state->count_;
            summary_sink_state->sum_ = delete_operator_state->sum_;
            break;
        }
        case PhysicalOperatorType::kUpdate: {
            UpdateOperatorState *update_operator_state = static_cast<UpdateOperatorState *>(task_operator_state);
            summary_sink_state->count_ = update_operator_state->count_;
            summary_sink_state->sum_ = update_operator_state->sum_;
            break;
        }
        default: {
            UnrecoverableError(fmt::format("{} isn't supported here.", PhysicalOperatorToString(task_operator_state->operator_type_)));
        }
    }
}

void PhysicalSink::FillSinkStateFromLastOperatorState(ResultSinkState *result_sink_state, OperatorState *task_operator_state) {
    switch (task_operator_state->operator_type_) {

        case PhysicalOperatorType::kInvalid: {
            String error_message = "Invalid operator";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
        case PhysicalOperatorType::kCreateTable: {
            auto *output_state = static_cast<CreateTableOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kCreateCollection: {
            auto *output_state = static_cast<CreateCollectionOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kCreateDatabase: {
            auto *output_state = static_cast<CreateDatabaseOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kCreateView: {
            auto *output_state = static_cast<CreateViewOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kDropTable: {
            auto *output_state = static_cast<DropTableOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kDropIndex: {
            auto *output_state = static_cast<DropIndexOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>()),
                };
            }
            break;
        }
        case PhysicalOperatorType::kDropCollection: {
            auto *output_state = static_cast<DropCollectionOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kDropDatabase: {
            auto *output_state = static_cast<DropDatabaseOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kDropView: {
            auto *output_state = static_cast<DropViewOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kCommand: {
            auto *output_state = static_cast<CommandOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kFlush: {
            auto *output_state = static_cast<FlushOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kOptimize: {
            auto *output_state = static_cast<OptimizeOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
            } else {
                result_sink_state->result_def_ = {
                    MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};
            }
            break;
        }
        case PhysicalOperatorType::kCreateIndexFinish: {
            auto *output_state = static_cast<CreateIndexFinishOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
                break;
            }
            result_sink_state->result_def_ = {
                MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>()),
            };
            break;
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            auto *output_state = static_cast<CreateIndexPrepareOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
                break;
            }
            result_sink_state->result_def_ = {
                MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>()),
            };
            break;
        }
        case PhysicalOperatorType::kCompactFinish: {
            auto *output_state = static_cast<CompactFinishOperatorState *>(task_operator_state);
            if (!output_state->Ok()) {
                result_sink_state->status_ = std::move(output_state->status_);
                break;
            }
            result_sink_state->result_def_ = {
                MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>()),
            };
            break;
        }
        default: {
            RecoverableError(
                Status::NotSupport(fmt::format("{} isn't supported here.", PhysicalOperatorToString(task_operator_state->operator_type_))));
        }
    }
}

void PhysicalSink::FillSinkStateFromLastOperatorState(MessageSinkState *message_sink_state, OperatorState *task_operator_state) {
    switch (task_operator_state->operator_type_) {
        case PhysicalOperatorType::kImport: {
            auto *import_output_state = static_cast<ImportOperatorState *>(task_operator_state);
            message_sink_state->message_ = std::move(import_output_state->result_msg_);
            break;
        }
        case PhysicalOperatorType::kExport: {
            auto *export_output_state = static_cast<ExportOperatorState *>(task_operator_state);
            message_sink_state->message_ = std::move(export_output_state->result_msg_);
            break;
        }
        case PhysicalOperatorType::kInsert: {
            auto *insert_output_state = static_cast<InsertOperatorState *>(task_operator_state);
            message_sink_state->message_ = std::move(insert_output_state->result_msg_);
            break;
        }
        case PhysicalOperatorType::kCreateIndexPrepare: {
            auto *create_index_prepare_output_state = static_cast<CreateIndexPrepareOperatorState *>(task_operator_state);
            message_sink_state->message_ = std::move(create_index_prepare_output_state->result_msg_);
            break;
        }
        case PhysicalOperatorType::kCreateIndexDo: {
            auto *create_index_do_output_state = static_cast<CreateIndexDoOperatorState *>(task_operator_state);
            message_sink_state->message_ = std::move(create_index_do_output_state->result_msg_);
            break;
        }
        case PhysicalOperatorType::kCompact: {
            // auto *compact_output_state = static_cast<CompactOperatorState *>(task_operator_state);
            message_sink_state->message_ = MakeUnique<String>("Tmp for test");
            break;
        }
        case PhysicalOperatorType::kCompactIndexPrepare: {
            // auto *compact_index_output_state = static_cast<CompactIndexOperatorState *>(task_operator_state);
            message_sink_state->message_ = MakeUnique<String>("Tmp for test");
            break;
        }
        case PhysicalOperatorType::kCompactIndexDo: {
            // auto *compact_index_output_state = static_cast<CompactIndexOperatorState *>(task_operator_state);
            message_sink_state->message_ = MakeUnique<String>("Tmp for test");
            break;
        }
        default: {
            RecoverableError(
                Status::NotSupport(fmt::format("{} isn't supported here.", PhysicalOperatorToString(task_operator_state->operator_type_))));
            break;
        }
    }
}

void PhysicalSink::FillSinkStateFromLastOperatorState(FragmentContext *fragment_context,
                                                      QueueSinkState *queue_sink_state,
                                                      OperatorState *task_operator_state) {
    // if (task_operator_state->operator_type_ == PhysicalOperatorType::kAggregate) {
    //     LOG_WARN(fmt::format("Sink Agg task id {}, fragment id {}", queue_sink_state->task_id_, queue_sink_state->fragment_id_));
    // }
    if (queue_sink_state->Error()) {
        LOG_TRACE(fmt::format("Error: {} is sent to notify next fragment", *queue_sink_state->status_.msg_));
        auto fragment_error = MakeShared<FragmentError>(queue_sink_state->fragment_id_, queue_sink_state->status_.clone());
        for (const auto &next_fragment_queue : queue_sink_state->fragment_data_queues_) {
            next_fragment_queue->Enqueue(fragment_error);
        }
        return;
    }

    if (!task_operator_state->Complete() && fragment_context->IsMaterialize()) {
        LOG_TRACE("Task not completed");
        return;
    }
    SizeT output_data_block_count = task_operator_state->data_block_array_.size();
    for (SizeT idx = 0; idx < output_data_block_count; ++idx) {
        auto fragment_data = MakeShared<FragmentData>(queue_sink_state->fragment_id_,
                                                      std::move(task_operator_state->data_block_array_[idx]),
                                                      queue_sink_state->task_id_,
                                                      idx,
                                                      output_data_block_count,
                                                      task_operator_state->Complete());
        if (task_operator_state->Complete() && !fragment_context->IsMaterialize()) {
            fragment_data->data_idx_ = None;
        }
        for (const auto &next_fragment_queue : queue_sink_state->fragment_data_queues_) {
            // when the Enqueue returns false,
            // it means that the downstream has collected enough data,
            // preventing the Queue from Enqueue in data again to avoid redundant calculations.
            if (!next_fragment_queue->Enqueue(fragment_data)) {
                task_operator_state->SetComplete();
            }
        }
    }
    task_operator_state->data_block_array_.clear();
}

} // namespace infinity
