//
// Created by JinHai on 2022/10/27.
//

#if 0
#include "transblock.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

void
TransBlock::Init(const std::vector<LogicalType>& types) {
    StorageAssert(!chunks_.empty(), "Transient block was already initialized before.");

    for(auto& type: types) {
        chunks_.emplace_back(type, DEFAULT_CHUNK_SIZE);
    }
}

void
TransBlock::Reset() {
    chunks_.clear();
    row_count_ = 0;
}


}
#endif