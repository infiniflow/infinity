//
// Created by jinhai on 23-2-6.
//

#pragma once


#include "common/types/data_type.h"
#include "common/column_vector/column_vector.h"

namespace infinity {

class HashTable {
public:
    void
    Init(const Vector<DataType>& types);

    void
    Append(const Vector<SharedPtr<ColumnVector>>& columns, SizeT block_id, SizeT row_count);

public:
    Vector<DataType> types_{};
    SizeT key_size_{};

    // Key -> (block id -> row array)
    HashMap<String, HashMap<SizeT, Vector<SizeT>>> hash_table_{};
};

}
