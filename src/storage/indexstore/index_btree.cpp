#include "index_btree.h"
#include "common/utility/infinity_assert.h"


namespace infinity{
IndexTreeBuilder::IndexTreeBuilder(FileWriter* writer)
    :writer_(writer) {
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
    // Flush all but the root of the index.
    for (size_t i = 0; i < idx_blocks_.size() - 1; i++) {
        FinishBlockAndPropagate(i);
    }
    // Flush root
    BlockPointer blockptr;
    Status s = FinishAndWriteBlock(idx_blocks_.size() - 1, &blockptr);
    if (!s.ok()) {
        return s.IOError("Unable to flush root index block");
    }

    return Status::OK();
}

Status IndexTreeBuilder::FinishBlockAndPropagate(size_t level) {
    IndexBlockBuilder* idx_block = idx_blocks_[level].get();
    if (idx_block->Count() == 0) {
        return Status::OK();
    }
    BlockPointer blockptr;
    FinishAndWriteBlock(level, &blockptr);
    idx_block->Reset();
    return Status::OK();
}

Status IndexTreeBuilder::FinishAndWriteBlock(size_t level, BlockPointer *blockptr) {
    IndexBlockBuilder* idx_block = idx_blocks_[level].get();
    Slice data = idx_block->Finish();
    //TODO, FileWriter requires to be encapsulated together with compressor
    u64 start_offset = writer_->TotalWrittenBytes();
    writer_->Write(data.data(), data.size());
    *blockptr = BlockPointer(start_offset, data.size());
    return Status::OK();
}


IndexTreeIterator::IndexTreeIterator(
    FileReader* reader,
    const BlockPointer & root_block) 
    : reader_(reader)
    , root_block_(root_block){

}

bool IndexTreeIterator::HasNext() {
  for (int i = seeked_indexes_.size(); i > 0; i--) {
    if (seeked_indexes_[i - 1]->iter_.HasNext())
      return true;
  }
  return false;
}

IndexBlockIterator *IndexTreeIterator::BottomIter() {
    return &seeked_indexes_.back()->iter_;
}

IndexBlockReader *IndexTreeIterator::BottomReader() {
    return &seeked_indexes_.back()->blockreader_;
}

IndexBlockIterator *IndexTreeIterator::SeekedIter(int depth) {
    return &seeked_indexes_[depth]->iter_;
}

IndexBlockReader *IndexTreeIterator::SeekedReader(int depth) {
  return &seeked_indexes_[depth]->blockreader_;
}

Status IndexTreeIterator::LoadBlock(const BlockPointer &blockptr, int depth) {
    SeekedIndex *seeked;
    if (depth < seeked_indexes_.size()) {
        seeked = seeked_indexes_[depth].get();

        if (seeked->block_ptr_.Offset() == blockptr.Offset()) {
            return Status::OK();
        }

        seeked->blockreader_.Reset();
        seeked->iter_.Reset();
    } else {
        seeked_indexes_.emplace_back(new SeekedIndex());
        seeked = seeked_indexes_.back().get();
    }
    uint8_t buf[blockptr.Size()];
    reader_->Read((char*)buf, blockptr.Size());
    seeked->block_ptr_ = blockptr;
    Slice block((char*)buf, blockptr.Size());
    seeked->blockreader_.Parse(block);
    return Status::OK();
}

Status IndexTreeIterator::SeekDownward(
    const Slice &search_key, 
    const BlockPointer &in_block,
    int cur_depth) {

    LoadBlock(in_block, cur_depth);

    IndexBlockIterator *iter = SeekedIter(cur_depth);

    iter->Seek(search_key);

    if (SeekedReader(cur_depth)->IsLeaf()) {
        seeked_indexes_.resize(cur_depth + 1);
        return Status::OK();
    }
    return Status::OK();
}
}