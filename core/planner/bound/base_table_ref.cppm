//
// Created by JinHai on 2022/9/12.
//

module;

import stl;
import table_ref;
import table_collection_entry;
import parser;
import table_function;
import block_index;

export module base_table_ref;

namespace infinity {

export class BaseTableRef : public TableRef {
public:
    explicit BaseTableRef(SharedPtr<TableFunction> table_func,
                          TableCollectionEntry *table_entry_ptr,
                          Vector<SizeT> column_ids,
                          SharedPtr<BlockIndex> block_index,
                          const String &alias,
                          u64 table_index,
                          SharedPtr<Vector<String>> column_names,
                          SharedPtr<Vector<SharedPtr<DataType>>> column_types)
        : TableRef(TableRefType::kTable, alias), table_func_(Move(table_func)), table_entry_ptr_(table_entry_ptr),
          column_ids_(Move(column_ids)), block_index_(Move(block_index)), column_names_(Move(column_names)),
          column_types_(Move(column_types)), table_index_(table_index) {}

    SharedPtr<TableFunction> table_func_{};

    TableCollectionEntry *table_entry_ptr_{};
    Vector<SizeT> column_ids_{};
    SharedPtr<BlockIndex> block_index_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    u64 table_index_;
};

} // namespace infinity