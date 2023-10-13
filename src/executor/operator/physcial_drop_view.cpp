//
// Created by JinHai on 2022/7/28.
//

#include "executor/operator/physcial_drop_view.h"
#include "storage/txn/txn.h"
#include "main/query_context.h"
#include "parser/statement/extra/create_table_info.h"
#include "storage/table.h"
#include "storage/table_def.h"

namespace infinity {

void PhysicalDropView::Init() {}

void PhysicalDropView::Execute(QueryContext *query_context) {
    Txn *txn = query_context->GetTxn();
    txn->DropViewByName(*schema_name_, *view_name_, conflict_type_);

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
}

void PhysicalDropView::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {}

} // namespace infinity
