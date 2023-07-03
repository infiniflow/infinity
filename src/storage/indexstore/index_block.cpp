#include "index_block.h"
#include "codec.h"
#include "common/utility/infinity_assert.h"

namespace infinity{

inline const uint8_t *SliceDecode(const uint8_t *encoded_ptr, Slice& retptr) {
    uint32_t len;
    const uint8_t *data_start = Codec::GetVIntPtr(encoded_ptr, &len);
    if (data_start == nullptr) {
        // bad varint
        return nullptr;
    }

    retptr = Slice((char*)data_start, len);
    return data_start + len;
}

IndexBlockBuilder::IndexBlockBuilder(bool is_leaf)
    :is_leaf_(is_leaf) {}

void IndexBlockBuilder::Add(const Slice & key, const u32& idx) {
    size_t entry_offset = buffer_.size();
    //size of key
    Codec::AddVInt(buffer_, key.size());
    //key data
    buffer_.append(key.data(), key.size());
    //idx
    Codec::AddVInt(buffer_, idx);
    entry_offsets_.push_back(entry_offset);
}

Slice IndexBlockBuilder::Finish() {
    for (uint32_t offset : entry_offsets_) {
        Codec::AddInt(buffer_, offset);
    }
    Codec::AddInt(buffer_, entry_offsets_.size());
    Codec::AddInt(buffer_, is_leaf_);
    return Slice(buffer_);
}

size_t IndexBlockBuilder::Count() const {
    return 0;
}

void IndexBlockBuilder::Reset() {
    buffer_.clear();
    entry_offsets_.clear();
}


IndexBlockReader::IndexBlockReader()
    :num_entries_(0) {
}

void IndexBlockReader::Reset() {
    data_ = Slice();
    num_entries_ = 0;
}

Status IndexBlockReader::Parse(const Slice& data) {
    data_ = data;
    const u8 *meta_ptr = (const u8*)data_.data() + data_.size() - sizeof(uint32_t) * 2;
    num_entries_ = Codec::DecodeInt(meta_ptr);
    is_leaf_ = Codec::DecodeInt(meta_ptr + sizeof(uint32_t));
    key_offsets_ = meta_ptr - sizeof(uint32_t) * num_entries_;
    return Status::OK();
}

size_t IndexBlockReader::Count() const {
    return num_entries_;
}

bool IndexBlockReader::IsLeaf() const {
    return is_leaf_;
}

IndexBlockIterator *IndexBlockReader::NewIterator() const {
    return new IndexBlockIterator(this);
}

void IndexBlockReader::GetKeyPointer(int idx_in_block, const uint8_t **ptr) const {
    size_t offset_in_block = Codec::DecodeInt(&key_offsets_[idx_in_block * sizeof(u32)]);
    *ptr = (uint8_t*)data_.data() + offset_in_block;
}

int IndexBlockReader::CompareKey(int idx_in_block, const Slice &key) const {
    const uint8_t *key_ptr;
    GetKeyPointer(idx_in_block, &key_ptr);
    Slice this_slice;
    if(SliceDecode(key_ptr, this_slice) == nullptr) {
        throw StorageException("Invalid data in block!");
    }

    return this_slice.compare(key);
}

Status IndexBlockReader::ReadEntry(size_t idx_in_block, Slice& key, u32& idx) const {
    if(idx_in_block >= num_entries_) {
        return Status::NotFound("Invalid index");
    }

    const uint8_t *ptr;
    GetKeyPointer(idx_in_block, &ptr);
    ptr = SliceDecode(ptr, key);
    if(ptr == nullptr) {
        return Status::Corruption("Invalid key in index");
    }
    ptr = Codec::GetVIntPtr(ptr, &idx);
    if(ptr == nullptr) {
        return Status::Corruption("Invalid id in index");
    }    
    return Status::OK();
}


IndexBlockIterator::IndexBlockIterator(const IndexBlockReader* reader)
    :reader_(reader)
    ,cur_idx_(-1) {

}

void IndexBlockIterator::Reset() {
    cur_idx_ = -1;
}

Status IndexBlockIterator::Seek(const Slice& key) {
    size_t left = 0;
    size_t right = reader_->Count() - 1;
    while (left < right) {
        int mid = (left + right + 1) / 2;

        int compare = reader_->CompareKey(mid, key);
        if (compare < 0) {
            left = mid;
        } else if (compare > 0) {
            right = mid - 1;
        } else {
            left = mid;
            break;
        }
    }
    int compare = reader_->CompareKey(left, key);
      if (compare > 0) {
        return Status::NotFound("key not present");
    }
    return SeekToIndex(left);
}

Status IndexBlockIterator::SeekToIndex(size_t idx_in_block) {
    cur_idx_ = idx_in_block;
    Status s = reader_->ReadEntry(idx_in_block, cur_key_, cur_id_);
    return s;
}

Status IndexBlockIterator::Next() {
    return SeekToIndex(cur_idx_ + 1);
}

}