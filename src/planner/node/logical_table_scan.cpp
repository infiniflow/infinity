//
// Created by JinHai on 2022/8/10.
//

#include "logical_table_scan.h"

#include <sstream>
#include <utility>

namespace infinity {

LogicalTableScan::LogicalTableScan(SharedPtr<Table> table_ptr,
                                   SharedPtr<TableScanFunction> table_scan_func,
                                   String table_alias,
                                   Vector<String> column_names,
                                   Vector<DataType> column_types)
    : LogicalNode(LogicalNodeType::kTableScan),
    table_ptr_(std::move(table_ptr)),
    table_scan_func_ptr_(std::move(table_scan_func)),
    table_alias_(std::move(table_alias)),
    column_names_(std::move(column_names)),
    column_types_(std::move(column_types)) {

    // FIXME: Disable the code: initialize column names and columns ?
//    size_t column_count = table_ptr_->table_def()->column_count();
//    columns_.reserve(column_count);
//    column_names_.reserve(column_count);
//    for(size_t idx = 0; idx < column_count; ++ idx) {
//        columns_.emplace_back(idx);
//        column_names_.emplace_back(table_ptr_->table_def()->columns()[idx].name());
//    }
}

String
LogicalTableScan::ToString(i64& space) {
    std::stringstream ss;
    String arrow_str;
    if(space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "TableScan: " << table_ptr_->TableName() << ", on: ";
    size_t column_count = table_ptr_->ColumnCount();
    for(size_t i = 0; i < column_count - 1; ++ i) {
        ss << column_names_[i] << " ";
    }
    ss << column_names_.back();
    space += arrow_str.size();

    return ss.str();
}

}