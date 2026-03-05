module infinity_core:physical_show_session_variables.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowSessionVariables(QueryContext *query_context, ShowOperatorState *operator_state) {
    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    BaseSession *session_ptr = query_context->current_session();
    for (auto &session_var_pair : VarUtil::session_name_map_) {
        const std::string &var_name = session_var_pair.first;
        SessionVariable session_var = session_var_pair.second;

        switch (session_var) {
            case SessionVariable::kQueryCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Accomplished query count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case SessionVariable::kTotalCommitCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Committed count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case SessionVariable::kTotalRollbackCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(session_ptr->query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Rollbacked transaction count in this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case SessionVariable::kConnectedTime: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(session_ptr->ConnectedTimeToStr());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Connected timestamp of this session");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            default: {
                operator_state->status_ = Status::NoSysVar(var_name);
                RecoverableError(operator_state->status_);
                return;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}