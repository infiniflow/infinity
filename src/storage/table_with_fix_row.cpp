//
// Created by JinHai on 2022/7/31.
//

#include "table_with_fix_row.h"

#include "main/infinity.h"
#include "block.h"

#include <memory>
#include <utility>

namespace infinity {

FixedRowCountTable::FixedRowCountTable(std::shared_ptr<TableDefinition> table_def)
    : Table(std::move(table_def)) {
    this->table_type_ = TableType::kFixedRowCount;
    this->row_count_limit_ = Infinity::instance().config()->option_.default_row_count_;
}

void
FixedRowCountTable::Append(const TransientBlock& tblock) {
    uint64_t column_count = table_def_->column_count();
    Assert(tblock.columns_.size() == column_count, "Wrong column data are appended to the table");
    uint64_t insert_row_count = tblock.row_count_;
    uint64_t start_idx = 0;
    while(insert_row_count > 0) {

        uint64_t row_capacity = row_count_ % row_count_limit_;
        if(row_capacity == 0) {
            // Last row group have no space, new row group is needed.
            blocks_.emplace_back(std::make_shared<Block>(table_def_, table_type_, row_count_limit_));
            row_capacity = row_count_limit_;
        }

        blocks_.back()->Append(tblock.columns_, start_idx);

        if(insert_row_count <= row_capacity) {
            insert_row_count = 0;
            row_count_ += insert_row_count;
            start_idx += insert_row_count;
        } else {
            insert_row_count -= row_capacity;
            row_count_ += row_capacity;
            start_idx += row_capacity;
        }
    }
}

}