//
// Created by JinHai on 2022/7/28.
//

module;

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;

module physical_create_view;

namespace infinity {

void PhysicalCreateView::Init() {}

void PhysicalCreateView::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) { output_state->SetComplete(); }

void PhysicalCreateView::Execute(QueryContext *query_context) {

    Txn *txn = query_context->GetTxn();

    txn->CreateView(create_view_info_->schema_name_, create_view_info_->view_name_, create_view_info_->conflict_type_);

    //    SharedPtr<View> view_ptr = MakeShared<View>(create_view_info_,
    //                                                GetOutputNames(),
    //                                                GetOutputTypes());
    //    InfinityContext::instance().catalog()->CreateView(create_view_info_->schema_name_,
    //                                               view_ptr,
    //                                               create_view_info_->conflict_type_);
    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity
