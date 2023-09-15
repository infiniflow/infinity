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
        case SinkStateType::kCommon: {
            // Output result
            CommonSinkState* common_sink_state = static_cast<CommonSinkState*>(sink_state);
            switch(sink_state->prev_output_state_->operator_type_) {
                case PhysicalOperatorType::kInvalid: {
                    ExecutorError("Invalid operator")
                }
                case PhysicalOperatorType::kShow: {
                    ShowOutputState* show_output_state = static_cast<ShowOutputState*>(common_sink_state->prev_output_state_);
                    common_sink_state->column_defs_ = show_output_state->table_def_->columns();
                    common_sink_state->data_block_array_ = std::move(show_output_state->output_);
                    break;
                }
                default: {
                    NotImplementError(fmt::format("{} isn't supported here.",
                                                  PhysicalOperatorToString(sink_state->prev_output_state_->operator_type_)));
                }
            }
            break;
        }
        case SinkStateType::kResult: {
            // Output result
            ResultSinkState* result_sink_state = static_cast<ResultSinkState*>(sink_state);
            switch(sink_state->prev_output_state_->operator_type_) {

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
                                                  PhysicalOperatorToString(sink_state->prev_output_state_->operator_type_)));
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
                    result_sink_state->result_def_ = {
                            MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())
                    };
                }

            }
        }
        case SinkStateType::kMessage:
            break;
    }
}

}
