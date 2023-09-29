//
// Created by jinhai on 23-4-28.
//

#include "physical_sink.h"

namespace infinity {

void
PhysicalSink::Init() {

}

void
PhysicalSink::Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) {

}

void
PhysicalSink::Execute(QueryContext* query_context) {

}

void
PhysicalSink::Execute(QueryContext* query_context, SinkState* sink_state) {
    switch(sink_state->state_type_) {
        case SinkStateType::kInvalid:
            break;
        case SinkStateType::kGeneral: {
            // Output general output
            auto* general_sink_state = static_cast<GeneralSinkState*>(sink_state);
            FillSinkStateFromLastOutputState(general_sink_state, general_sink_state->prev_output_state_);
            break;
        }
        case SinkStateType::kResult: {
            // Output result
            auto* result_sink_state = static_cast<ResultSinkState*>(sink_state);
            FillSinkStateFromLastOutputState(result_sink_state, result_sink_state->prev_output_state_);
            break;
        }
        case SinkStateType::kMessage: {
            // Output message
            auto* message_sink_state = static_cast<MessageSinkState*>(sink_state);
            FillSinkStateFromLastOutputState(message_sink_state, message_sink_state->prev_output_state_);
            break;
        }
    }
}

void
PhysicalSink::FillSinkStateFromLastOutputState(GeneralSinkState* general_sink_state, OutputState* task_output_state) {
    switch(task_output_state->operator_type_) {
        case PhysicalOperatorType::kInvalid: {
            ExecutorError("Invalid operator")
        }
        case PhysicalOperatorType::kShow: {
            ShowOutputState* show_output_state = static_cast<ShowOutputState*>(task_output_state);
            general_sink_state->data_block_array_ = std::move(show_output_state->output_);
            break;
        }
        case PhysicalOperatorType::kExplain: {
            ExplainOutputState* explain_output_state
                    = static_cast<ExplainOutputState*>(task_output_state);
            if(explain_output_state->data_block_ == nullptr) {
                ExecutorError("Empty explain output")
            }
            general_sink_state->data_block_array_.emplace_back(explain_output_state->data_block_);
            break;
        }
        case PhysicalOperatorType::kProjection: {
            ProjectionOutputState* projection_output_state = static_cast<ProjectionOutputState*>(task_output_state);
            if(projection_output_state->data_block_ == nullptr) {
                ExecutorError("Empty projection output")
            }
            general_sink_state->data_block_array_.emplace_back(projection_output_state->data_block_);
            break;
        }
        default: {
            NotImplementError(fmt::format("{} isn't supported here.",
                                          PhysicalOperatorToString(task_output_state->operator_type_)));
        }
    }
}

void
PhysicalSink::FillSinkStateFromLastOutputState(ResultSinkState* result_sink_state, OutputState* task_output_state) {
    switch(task_output_state->operator_type_) {

        case PhysicalOperatorType::kInvalid: {
            ExecutorError("Invalid operator")
        }
        case PhysicalOperatorType::kAggregate:
        case PhysicalOperatorType::kParallelAggregate:
        case PhysicalOperatorType::kMergeParallelAggregate:
        case PhysicalOperatorType::kUnionAll:
        case PhysicalOperatorType::kIntersect:
        case PhysicalOperatorType::kExcept:
        case PhysicalOperatorType::kTableScan:
        case PhysicalOperatorType::kKnnScan:
        case PhysicalOperatorType::kFilter:
        case PhysicalOperatorType::kIndexScan:
        case PhysicalOperatorType::kDummyScan:
        case PhysicalOperatorType::kHash:
        case PhysicalOperatorType::kMergeHash:
        case PhysicalOperatorType::kJoinHash:
        case PhysicalOperatorType::kJoinNestedLoop:
        case PhysicalOperatorType::kJoinMerge:
        case PhysicalOperatorType::kJoinIndex:
        case PhysicalOperatorType::kCrossProduct:
        case PhysicalOperatorType::kLimit:
        case PhysicalOperatorType::kMergeLimit:
        case PhysicalOperatorType::kTop:
        case PhysicalOperatorType::kMergeTop:
        case PhysicalOperatorType::kProjection:
        case PhysicalOperatorType::kSort:
        case PhysicalOperatorType::kMergeSort:
        case PhysicalOperatorType::kDelete:
        case PhysicalOperatorType::kUpdate:
        case PhysicalOperatorType::kInsert:
        case PhysicalOperatorType::kImport:
        case PhysicalOperatorType::kExport:
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kFlush:
        case PhysicalOperatorType::kSink:
        case PhysicalOperatorType::kSource:
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kPreparedPlan:
        case PhysicalOperatorType::kShow: {
            NotImplementError(fmt::format("{} isn't supported here.",
                                          PhysicalOperatorToString(task_output_state->operator_type_)));
        }
        case PhysicalOperatorType::kCreateTable: {
            auto* output_state = static_cast<CreateTableOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kCreateCollection: {
            auto* output_state = static_cast<CreateCollectionOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kCreateDatabase: {
            auto* output_state = static_cast<CreateDatabaseOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kCreateView: {
            auto* output_state = static_cast<CreateViewOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kDropTable: {
            auto* output_state = static_cast<DropTableOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kDropCollection: {
            auto* output_state = static_cast<DropCollectionOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kDropDatabase: {
            auto* output_state = static_cast<DropDatabaseOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
        case PhysicalOperatorType::kDropView: {
            auto* output_state = static_cast<DropViewOutputState*>(task_output_state);
            if(output_state->error_message_ != nullptr) {
                result_sink_state->error_message_ = std::move(output_state->error_message_);
            } else {
                result_sink_state->result_def_ = {
                        MakeShared<ColumnDef>(0,
                                              MakeShared<DataType>(LogicalType::kInteger),
                                              "OK",
                                              HashSet<ConstraintType>())
                };
            }
            break;
        }
    }
}

void
PhysicalSink::FillSinkStateFromLastOutputState(MessageSinkState* message_sink_state, OutputState* task_output_state) {
    switch(task_output_state->operator_type_) {
        case PhysicalOperatorType::kImport: {
            auto* import_output_state = static_cast<ImportOutputState*>(task_output_state);
            message_sink_state->message_ = std::move(import_output_state->result_msg_);
            break;
        }
        default: {
            NotImplementError(fmt::format("{} isn't supported here.",
                                          PhysicalOperatorToString(task_output_state->operator_type_)));
            break;
        }
    }

}

}
