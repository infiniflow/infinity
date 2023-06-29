#include "index_btree.h"


namespace infinity{
IndexTreeBuilder::IndexTreeBuilder() {

}

Status IndexTreeBuilder::Append(const Slice &key, const u32 id, size_t level) {
    if (level >= idx_blocks_.size()) {
        // Need to create a new level
        idx_blocks_.emplace_back(new IndexBlockBuilder(false));
    }
    IndexBlockBuilder* idx_block = idx_blocks_[level].get();
    idx_block->Add(key, id);

    return Status::OK();
}

Status IndexTreeBuilder::Finish() {

    return Status::OK();
}

Status IndexTreeBuilder::FinishBlockAndPropagate(size_t level) {
    IndexBlockBuilder* idx_block = idx_blocks_[level].get();
    if (idx_block->Count() == 0) {
        return Status::OK();
    }


    idx_block->Reset();
    return Status::OK();
}
}