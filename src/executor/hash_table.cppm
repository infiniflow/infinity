//
// Created by jinhai on 23-2-6.
//

module;

import parser;
import stl;
import column_vector;

export module hash_table;

namespace infinity {

export class HashTable {
public:
    void Init(const Vector<DataType> &types);

    void Append(const Vector<SharedPtr<ColumnVector>> &columns, SizeT block_id, SizeT row_count);

public:
    Vector<DataType> types_{};
    SizeT key_size_{};

    // Key -> (block id -> row array)
    HashMap<String, HashMap<SizeT, Vector<SizeT>>> hash_table_{};
};

} // namespace infinity
