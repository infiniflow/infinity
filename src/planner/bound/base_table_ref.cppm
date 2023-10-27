//
// Created by JinHai on 2022/9/12.
//

module;

#include <algorithm>

import stl;
import table_ref;
import table_collection_entry;
import parser;
import table_function;
import block_index;
import infinity_assert;
import infinity_exception;

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

    void EraseColumnByIdxs(Vector<int>&& indices) {
        Assert<PlannerException>(std::is_sorted(indices.cbegin(), indices.cend()), "Indices must be in order", __FILE_NAME__, __LINE__);
        Vector<int> r_indices(indices);
        std::sort(r_indices.rbegin(), r_indices.rend());

        for (const auto& index : r_indices) {
            column_ids_.erase(column_ids_.begin() + index);
            column_names_->erase(column_names_->begin() + index);
            column_types_->erase(column_types_->begin() + index);
        }
    };

    SharedPtr<TableFunction> table_func_{};

    TableCollectionEntry *table_entry_ptr_{};
    Vector<SizeT> column_ids_{};
    SharedPtr<BlockIndex> block_index_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    u64 table_index_;
};

} // namespace infinity