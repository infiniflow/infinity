//
// Created by JinHai on 2022/11/12.
//

#include "data_block.h"

namespace infinity {

void
DataBlock::Init(const std::vector<DataType> &types) {
    StorageAssert(!initialized, "Data block was initialized before.");
    column_count_ = types.size();
    columns.reserve(column_count_);
    for(size_t i = 0; i < column_count_; ++ i) {
        columns.emplace_back(types[i]);
        columns[i].Initialize(row_capacity_);
    }
}

void
DataBlock::Reset() {

    for(size_t i = 0; i < column_count_; ++ i) {
        columns[i].Reset();
        columns[i].Initialize(row_capacity_);
    }

    row_count_ = 0;
}

Value
DataBlock::GetValue(size_t column_index, size_t row_index) const {
    return columns[column_index].GetValue(row_index);
}

//void
//DataBlock::SetValue(size_t column_index, size_t row_index, const Value &val) {
//    columns[column_index].SetValue(row_index, val);
//}

void
DataBlock::AppendValue(size_t column_index, const Value& value) {
    return columns[column_index].AppendValue(value);
}

}


