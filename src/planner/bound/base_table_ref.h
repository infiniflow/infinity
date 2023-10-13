//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "planner/node/logical_table_scan.h"
#include "table_ref.h"

namespace infinity {

class TableFunction;
class BlockIndex;

class BaseTableRef : public TableRef {
public:
    explicit BaseTableRef(SharedPtr<TableFunction> table_func,
                          TableCollectionEntry *table_entry_ptr,
                          Vector<SizeT> column_ids,
                          SharedPtr<BlockIndex> block_index,
                          const String &alias,
                          u64 table_index,
                          SharedPtr<Vector<String>> column_names,
                          SharedPtr<Vector<SharedPtr<DataType>>> column_types)
        : TableRef(TableRefType::kTable, alias), table_func_(std::move(table_func)), table_entry_ptr_(table_entry_ptr),
          column_ids_(std::move(column_ids)), block_index_(std::move(block_index)), column_names_(std::move(column_names)),
          column_types_(std::move(column_types)), table_index_(table_index) {}

    SharedPtr<TableFunction> table_func_{};

    TableCollectionEntry *table_entry_ptr_{};
    Vector<SizeT> column_ids_{};
    SharedPtr<BlockIndex> block_index_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    u64 table_index_;
};

} // namespace infinity