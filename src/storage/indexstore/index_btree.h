#pragma once

#include "index_block.h"
#include "storage/block_pointer.h"
#include "storage/io/file_writer.h"
#include "storage/io/file_reader.h"

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
    IndexTreeBuilder(FileWriter* writer);

    Status Append(const Slice &key, const u32 id, size_t level = 0);

    Status Finish();
private:
    // Finish the current block at the given index level, and then
    // propagate by inserting this block into the next higher-up
    // level index.
    Status FinishBlockAndPropagate(size_t level);

    Status FinishAndWriteBlock(size_t level, BlockPointer *blockptr);

private:
    std::vector<std::unique_ptr<IndexBlockBuilder>> idx_blocks_;

    FileWriter *writer_;
};

class IndexTreeIterator {
public:
    IndexTreeIterator(
        FileReader* reader,
        const BlockPointer & root_block);

    ~IndexTreeIterator() = default;

    Status Seek(const Slice& key);

    const Slice GetCurrentKey() const {
        return seeked_indexes_.back()->iter_.GetCurrentKey();
    }

    bool HasNext();
private:

    IndexBlockIterator *BottomIter();

    IndexBlockReader *BottomReader();

    Status LoadBlock(const BlockPointer &block, int depth);

    IndexBlockIterator *SeekedIter(int depth);

    IndexBlockReader *SeekedReader(int depth);

    Status SeekDownward(const Slice &search_key, const BlockPointer &in_block, int cur_depth);
private:
    struct SeekedIndex {
        SeekedIndex() :
          iter_(&blockreader_){}

        BlockPointer block_ptr_;
        IndexBlockReader blockreader_;
        IndexBlockIterator iter_;
    };

    FileReader *reader_;

    BlockPointer root_block_;

    std::vector<std::unique_ptr<SeekedIndex>> seeked_indexes_;

};
}