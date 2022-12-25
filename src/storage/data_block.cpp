//
// Created by JinHai on 2022/11/12.
//

#include "data_block.h"

namespace infinity {

void
DataBlock::Init(const std::vector<DataType> &types) {
    StorageAssert(!initialized, "Data block was initialized before.");
    column_count_ = types.size();
    column_vectors.reserve(column_count_);
    for(size_t i = 0; i < column_count_; ++ i) {
        column_vectors.emplace_back(types[i]);
        column_vectors[i].Initialize(row_capacity_);
    }
    initialized = true;
}

void
DataBlock::Reset() {

    for(size_t i = 0; i < column_count_; ++ i) {
        column_vectors[i].Reset();
        column_vectors[i].Initialize(row_capacity_);
    }

    row_count_ = 0;
}

Value
DataBlock::GetValue(size_t column_index, size_t row_index) const {
    return column_vectors[column_index].GetValue(row_index);
}

void
DataBlock::SetValue(size_t column_index, size_t row_index, const Value &val) {
    StorageAssert(column_index < column_count_,
                  "Attempt to access invalid column index: " + std::to_string(column_index));
    column_vectors[column_index].SetValue(row_index, val);
}

void
DataBlock::AppendValue(size_t column_index, const Value& value) {
    StorageAssert(column_index < column_count_,
                  "Attempt to access invalid column index: " + std::to_string(column_index));
    column_vectors[column_index].AppendValue(value);
    ++ row_count_;
}

}


