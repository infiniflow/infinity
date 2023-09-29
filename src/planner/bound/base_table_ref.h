//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "table_ref.h"
#include "planner/node/logical_table_scan.h"

#include <utility>

namespace infinity {

class TableFunction;

class BaseTableRef : public TableRef {
public:
    explicit
    BaseTableRef(SharedPtr<TableFunction> table_func,
                 TableCollectionEntry* table_entry_ptr,
                 Vector<SizeT> column_ids,
                 SharedPtr<Vector<SegmentEntry*>> segment_entries,
                 const String& alias,
                 u64 table_index,
                 SharedPtr<Vector<String>> column_names,
                 SharedPtr<Vector<SharedPtr<DataType>>> column_types)
            : TableRef(TableRefType::kTable, alias),
              table_func_(std::move(table_func)),
              table_entry_ptr_(table_entry_ptr),
              column_ids_(std::move(column_ids)),
              segment_entries_(std::move(segment_entries)),
              column_names_(std::move(column_names)),
              column_types_(std::move(column_types)),
              table_index_(table_index) {}

    SharedPtr<TableFunction> table_func_{};

    TableCollectionEntry* table_entry_ptr_{};
    Vector<SizeT> column_ids_{};
    SharedPtr<Vector<SegmentEntry*>> segment_entries_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    u64 table_index_;
};

}