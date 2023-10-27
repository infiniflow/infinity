
module;

import stl;
import query_context;
import operator_state;
import parser;
import table_def;
import table;
import infinity_assert;
import infinity_exception;

module physical_drop_index;

namespace infinity {

void PhysicalDropIndex::Init() {}

void PhysicalDropIndex::Execute(QueryContext *query_context) {
    Error<NotImplementException>("Deprecated execute function should not be called.", __FILE_NAME__, __LINE__);
}

void PhysicalDropIndex::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto txn = query_context->GetTxn();
    auto res = txn->DropIndexByName(*schema_name_, *table_name_, *index_name_, conflict_type_);

    auto drop_index_output_state = static_cast<DropIndexOutputState *>(output_state);
    drop_index_output_state->error_message_ = Move(res.err_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    auto result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
    output_state->SetComplete();
}
} // namespace infinity