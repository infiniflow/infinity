module infinity_core:physical_show_profiles.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :query_context;
import :operator_state;
import :data_block;
import :new_catalog;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowProfiles(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto catalog = query_context->storage()->new_catalog();

    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    auto records = catalog->GetProfileRecords();
    for (size_t i = 0; i < records.size(); ++i) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        // Output record no
        ValueExpression record_no_expr(Value::MakeVarchar(fmt::format("{}", i)));
        record_no_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);

        // Output each query phase
        i64 total_cost{};
        size_t column_count = output_types_->size();
        for (size_t j = 0; j < column_count - 2; ++j) {
            i64 this_time = records[i]->ElapsedAt(j);
            total_cost += this_time;

            std::chrono::nanoseconds duration(this_time);
            ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(duration)));
            phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors_[j + 1]);
        }

        // Output total query duration
        std::chrono::nanoseconds total_duration(total_cost);
        ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(total_duration)));
        phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors_.back());

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}