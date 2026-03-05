module infinity_core:physical_show_config.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowConfig(QueryContext *query_context, ShowOperatorState *operator_state) {
    Config *global_config = query_context->global_config();
    auto [base_option, status] = global_config->GetConfigByName(*object_name_);
    if (!status.ok()) {
        operator_state->status_ = status;
        return;
    }
    if (*object_name_ == "time_zone_bias") {
        operator_state->status_ = Status::InvalidConfig(fmt::format("Option: {} doesn't exist.", *object_name_));
        return;
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);
    auto double_type = std::make_shared<DataType>(LogicalType::kDouble);
    auto bool_type = std::make_shared<DataType>(LogicalType::kBoolean);

    switch (base_option->data_type_) {
        case BaseOptionDataType::kInteger: {
            std::vector<std::shared_ptr<DataType>> output_column_types{
                bigint_type,
            };
            output_block_ptr->Init(output_column_types);

            IntegerOption *integer_option = static_cast<IntegerOption *>(base_option);
            Value value = Value::MakeBigInt(integer_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case BaseOptionDataType::kFloat: {
            std::vector<std::shared_ptr<DataType>> output_column_types{
                double_type,
            };
            output_block_ptr->Init(output_column_types);

            FloatOption *float_option = static_cast<FloatOption *>(base_option);
            Value value = Value::MakeDouble(float_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case BaseOptionDataType::kString: {
            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            StringOption *string_option = static_cast<StringOption *>(base_option);
            std::string value_str;
            if (*object_name_ == "time_zone") {
                auto [time_zone_bias, _] = global_config->GetConfigByName("time_zone_bias");

                IntegerOption *time_zone_bias_int = static_cast<IntegerOption *>(time_zone_bias);
                if (time_zone_bias_int->value_ >= 0) {
                    value_str = fmt::format("{}+{}", string_option->value_, time_zone_bias_int->value_);
                } else {
                    value_str = fmt::format("{}-{}", string_option->value_, time_zone_bias_int->value_);
                }
            } else {
                value_str = string_option->value_;
            }
            Value value = Value::MakeVarchar(value_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case BaseOptionDataType::kBoolean: {
            std::vector<std::shared_ptr<DataType>> output_column_types{
                bool_type,
            };
            output_block_ptr->Init(output_column_types);

            BooleanOption *boolean_option = static_cast<BooleanOption *>(base_option);
            Value value = Value::MakeBool(boolean_option->value_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case BaseOptionDataType::kLogLevel: {
            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            LogLevelOption *loglevel_option = static_cast<LogLevelOption *>(base_option);
            Value value = Value::MakeVarchar(LogLevel2Str(loglevel_option->value_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case BaseOptionDataType::kFlush: {
            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };
            output_block_ptr->Init(output_column_types);

            FlushOption *flush_option = static_cast<FlushOption *>(base_option);
            Value value = Value::MakeVarchar(FlushOptionTypeToString(flush_option->value_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        default: {
            UnrecoverableError("Invalid option data type.");
            break;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}