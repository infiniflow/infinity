//
// Created by JinHai on 2022/7/31.
//

#include "chunk.h"
#include "common/utility/infinity_assert.h"

namespace infinity {
// TODO: unit test !!!
void
Chunk::Append(const Chunk& chunk, int64_t start_idx) {
    StorageAssert(data_type_ == chunk.data_type_,
                  "Trying to append different data from: " + chunk.data_type_.ToString() + " to: " + data_type_.ToString());
    int64_t available_size = limit_ - row_count_;
    size_t insert_size = chunk.data_.size() - start_idx;
    if(available_size >= insert_size) {
        data_.insert(data_.begin() + row_count_, chunk.data_.begin() + start_idx, chunk.data_.end());
    } else {
        data_.insert(data_.begin() + row_count_, chunk.data_.begin() + start_idx, chunk.data_.begin() + available_size);
    }
}

void
Chunk::Append(const std::any& value) {
    data_.emplace_back(value);
}

}