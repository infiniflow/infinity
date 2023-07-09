//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "storage/meta/entry/db_entry.h"
#include "storage/data_block.h"

namespace infinity {

class TxnTableStore {
public:
    explicit
    TxnTableStore(String table_name, TableDef* table_def) : table_name_(std::move(table_name)) {}

    UniquePtr<String>
    Append(const SharedPtr<DataBlock>& input_block);

    UniquePtr<String>
    Delete(const Vector<RowID>& row_ids);

    Vector<SharedPtr<DataBlock>> blocks_;

    SizeT current_block_id_{0};

    String table_name_{};
    TableDef* table_def_{};
};

}
