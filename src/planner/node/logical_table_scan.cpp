//
// Created by JinHai on 2022/8/10.
//

#include "logical_table_scan.h"
#include "planner/bound/base_table_ref.h"
#include <sstream>
#include <utility>

namespace infinity {

LogicalTableScan::LogicalTableScan(u64 node_id,
                                   SharedPtr<BaseTableRef> base_table_ref)
    : LogicalNode(node_id, LogicalNodeType::kTableScan),
    base_table_ref_(std::move(base_table_ref)) {

    // FIXME: Disable the code: initialize column names and columns ?
//    size_t column_count = table_ptr_->table_def()->column_count();
//    columns_.reserve(column_count);
//    column_names_.reserve(column_count);
//    for(size_t idx = 0; idx < column_count; ++ idx) {
//        columns_.emplace_back(idx);
//        column_names_.emplace_back(table_ptr_->table_def()->columns()[idx].name());
//    }
}

Vector<ColumnBinding>
LogicalTableScan::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT column_count = base_table_ref_->column_names_->size();
    result.reserve(column_count);
    for(SizeT i = 0; i < column_count; ++ i) {
        result.emplace_back(base_table_ref_->table_index_, i);
    }
    return result;
}

SharedPtr<Vector<String>>
LogicalTableScan::GetOutputNames() const {
    return base_table_ref_->column_names_;
}

SharedPtr<Vector<SharedPtr<DataType>>>
LogicalTableScan::GetOutputTypes() const {
    return base_table_ref_->column_types_;
}

TableCollectionEntry*
LogicalTableScan::table_collection_ptr() const {
    return base_table_ref_->table_entry_ptr_;
}

String
LogicalTableScan::TableAlias() const {
    return base_table_ref_->alias_;
}

u64
LogicalTableScan::TableIndex() const {
    return base_table_ref_->table_index_;
}

String
LogicalTableScan::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str
       << "TableScan: " << base_table_ref_->table_entry_ptr_->table_collection_name_
       << ", on: ";
    size_t column_count = base_table_ref_->column_names_->size();
    for(size_t i = 0; i < column_count - 1; ++ i) {
        ss << base_table_ref_->column_names_->at(i) << " ";
    }
    ss << base_table_ref_->column_names_->back();
    space += arrow_str.size();

    return ss.str();
}

}