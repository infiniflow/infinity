module infinity_core:physical_show_session_variable.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :data_table;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowSessionVariable(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::shared_ptr<DataType> varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    std::shared_ptr<DataType> integer_type = std::make_shared<DataType>(LogicalType::kBigInt);
    std::shared_ptr<DataType> bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    SessionVariable session_var = VarUtil::GetSessionVarByName(*object_name_);
    BaseSession *session_ptr = query_context->current_session();
    switch (session_var) {
        case SessionVariable::kQueryCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->query_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case SessionVariable::kTotalCommitCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->committed_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case SessionVariable::kTotalRollbackCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBigInt(session_ptr->rollbacked_txn_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case SessionVariable::kConnectedTime: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeVarchar(session_ptr->ConnectedTimeToStr());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        default: {
            operator_state->status_ = Status::NoSysVar(*object_name_);
            RecoverableError(operator_state->status_);
            return;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}