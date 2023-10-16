#include "physical_drop_index.h"
#include "common/types/logical_type.h"
#include "common/utility/infinity_assert.h"
#include "executor/operator_state.h"
#include "main/query_context.h"
#include "parser/definition/column_def.h"
#include "parser/definition/table_def.h"
#include "storage/table.h"
#include "storage/txn/txn.h"

namespace infinity {

void PhysicalDropIndex::Init() {}

void PhysicalDropIndex::Execute(QueryContext *query_context) { NotImplementError("Deprecated execute function should not be called.") }

void PhysicalDropIndex::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto txn = query_context->GetTxn();
    auto res = txn->DropIndexByName(*schema_name_, *table_name_, *index_name_, conflict_type_);

    auto drop_index_output_state = static_cast<DropIndexOutputState *>(output_state);
    drop_index_output_state->error_message_ = std::move(res.err_);

    // Generate the result
    auto column_defs = {MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    auto result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
    output_state->SetComplete();
}
} // namespace infinity