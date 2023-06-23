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
    TxnTableStore(String table_name) : table_name_(std::move(table_name)) {}

    void
    Append(const SharedPtr<DataBlock>& input_block);

    Vector<SharedPtr<DataBlock>> blocks_;

    String table_name_{};
};

}
