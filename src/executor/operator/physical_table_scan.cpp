//
// Created by JinHai on 2022/7/28.
//

#include "storage/table_with_fix_row.h"
#include "physical_table_scan.h"

namespace infinity {

void
PhysicalTableScan::Init() {
}

void
PhysicalTableScan::Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) {
    auto* table_scan_input_state = static_cast<TableScanInputState*>(input_state);
    auto* table_scan_output_state = static_cast<TableScanOutputState*>(output_state);

    Vector<u64>& segment_entry_index_array = *table_scan_input_state->segment_entry_indexes_;
    for(const u64 segment_idx: segment_entry_index_array) {
        SegmentEntry* segment_entry = table_scan_function_data_ptr_->segment_entries_->at(segment_idx);
        LOG_TRACE("Segment Entry ID: {}", segment_entry->segment_id_);
    }

    table_scan_output_state->data_block_ = MakeShared<DataBlock>();
    table_scan_output_state->data_block_->Init(*column_types_);

    table_scan_func_ptr_->main_function_(query_context,
                                         table_scan_function_data_ptr_.get(),
                                         *table_scan_output_state->data_block_);
}

void
PhysicalTableScan::Execute(QueryContext* query_context) {
    // Generate the result table definition
    Vector<SharedPtr<ColumnDef>> column_defs;
    size_t column_count = column_names_->size();
    for(size_t idx = 0; idx < column_count; ++ idx) {
        // Use the column id to fetch column name and type
        String& col_name_ref = column_names_->at(idx);
        SharedPtr<DataType> col_type_ref = column_types_->at(idx);

        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx,
                                                             col_type_ref,
                                                             col_name_ref,
                                                             HashSet<ConstraintType>());
        column_defs.emplace_back(col_def);
    }

    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>(table_alias_), column_defs);

    output_ = MakeShared<Table>(table_def_ptr, TableType::kResult);

    while(true) {
         SharedPtr<DataBlock> output_block = MakeShared<DataBlock>();
         output_block->Init(*column_types_);
         table_scan_func_ptr_->main_function_(
                 query_context,
                 table_scan_function_data_ptr_.get(),
                 *output_block);
        if(output_block->row_count() > 0) {
            output_->Append(output_block);
        } else {
            break;
        }
    }
}

Vector<SharedPtr<Vector<u64>>>
PhysicalTableScan::PlanSegmentEntries(i64 parallel_count) const {
    SizeT segment_count = table_scan_function_data_ptr_->segment_entries_->size();
    Vector<SharedPtr<Vector<u64>>> result(parallel_count, nullptr);
    for(SizeT task_id = 0; task_id < parallel_count; ++ task_id) {
        result[task_id] = MakeShared<Vector<u64>>();
    }

    for(SizeT idx = 0; idx < segment_count; ++ idx) {
        u64 task_id = idx % parallel_count;
        result[task_id]->emplace_back(idx);
    }
    return result;
}

}
