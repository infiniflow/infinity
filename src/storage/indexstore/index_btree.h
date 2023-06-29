#pragma once

#include "index_block.h"
#include <leveldb/slice.h>
#include <leveldb/status.h>

#include <vector>
#include <string>
#include <memory>

namespace infinity {
using Slice = leveldb::Slice;
using Status = leveldb::Status;

class IndexTreeBuilder {
public:
    IndexTreeBuilder();

    Status Append(const Slice &key, const u32 id, size_t level = 0);

    Status Finish();
private:
    // Finish the current block at the given index level, and then
    // propagate by inserting this block into the next higher-up
    // level index.
    Status FinishBlockAndPropagate(size_t level);
private:
    std::vector<std::unique_ptr<IndexBlockBuilder>> idx_blocks_;
};

}