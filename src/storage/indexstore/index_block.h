#pragma once

#include "common/types/internal_types.h"
#include <leveldb/slice.h>
#include <leveldb/status.h>

#include <vector>
#include <string>

namespace infinity {

using Slice = leveldb::Slice;
using Status = leveldb::Status;

class IndexBlockBuilder {
public:
    explicit IndexBlockBuilder(bool is_leaf);

    void Add(const Slice & key, const u32& idx);

    Slice Finish();

    size_t Count() const;

    void Reset();
private:
	bool finished_;

	bool is_leaf_;

	std::string buffer_;

	std::vector<u32> entry_offsets_;
};

class IndexBlockIterator;
class IndexBlockReader {
public:
    IndexBlockReader();

    void Reset();

    Status Parse(const Slice& data);

    size_t Count() const;

    bool IsLeaf() const;

    IndexBlockIterator* NewIterator() const;

    void GetKeyPointer(int idx_in_block, const uint8_t **ptr) const;

    int CompareKey(int idx_in_block, const Slice &key) const;

    Status ReadEntry(size_t idx_in_block, Slice& key, u32& idx) const;
private:

    Slice data_;
    size_t num_entries_;
    bool is_leaf_;
    const uint8_t *key_offsets_;
};

class IndexBlockIterator {
public:
    explicit IndexBlockIterator(const IndexBlockReader *reader);

    void Reset();

    Status Seek(const Slice& key);

    Status SeekToIndex(size_t idx_in_block);

    const Slice GetCurrentKey() const {
        return cur_key_;
    }

    bool HasNext() const {
        return cur_idx_ + 1 < reader_->Count();
    }

    Status Next();    
private:
    const IndexBlockReader *reader_;
    size_t cur_idx_;
    Slice cur_key_;
    u32 cur_id_;

};
}