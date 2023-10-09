#include "buffered_skiplist_writer.h"

namespace infinity {

BufferedSkipListWriter::BufferedSkipListWriter(MemoryPool *byte_slice_pool, MemoryPool *buffer_pool)
    : BufferedByteSlice(byte_slice_pool, buffer_pool), last_key_(0), last_value1_(0) {}

void BufferedSkipListWriter::AddItem(uint32_t key, uint32_t value1, uint32_t value2) {
    PushBack(0, key - last_key_);
    PushBack(1, value1 - last_value1_);
    last_key_ = key;
    last_value1_ = value1;
    PushBack(2, value2);
    EndPushBack();

    if (NeedFlush(SKIP_LIST_BUFFER_SIZE)) {
        Flush();
    }
}

void BufferedSkipListWriter::AddItem(uint32_t key, uint32_t value1) {
    PushBack(0, key - last_key_);
    PushBack(1, value1);
    EndPushBack();

    last_key_ = key;

    if (NeedFlush(SKIP_LIST_BUFFER_SIZE)) {
        Flush();
    }
}

void BufferedSkipListWriter::AddItem(uint32_t value_delta) {
    last_value1_ += value_delta;
    PushBack(0, last_value1_);
    EndPushBack();

    if (NeedFlush(SKIP_LIST_BUFFER_SIZE)) {
        Flush();
    }
}

size_t BufferedSkipListWriter::DoFlush() {
    uint32_t flush_size = 0;
    uint8_t size = buffer_.Size();

    const PostingValues *posting_values = GetPostingValues();
    for (size_t i = 0; i < posting_values->GetSize(); ++i) {
        PostingValue *posting_value = posting_values->GetValue(i);
        uint8_t *buffer = buffer_.GetRow(posting_value->location_);
        flush_size += posting_value->Encode(posting_writer_, buffer, size * posting_value->GetSize());
    }
    return flush_size;
}

void BufferedSkipListWriter::Dump(const std::shared_ptr<FileWriter> &file) {
    uint32_t skiplist_size = GetTotalCount();
    if (skiplist_size == 0) {
        return;
    }
    if (GetPostingValues()->GetSize() != 3) {
        posting_writer_.Dump(file);
        return;
    }

    const ByteSliceList *skipList = posting_writer_.GetByteSliceList();
    ByteSliceListIterator iter(skipList);
    uint32_t start = 0;
    const PostingValues *posting_values = GetPostingValues();
    for (size_t i = 0; i < posting_values->GetSize(); ++i) {
        PostingValue *posting_value = posting_values->GetValue(i);
        uint32_t len = posting_value->GetSize() * skiplist_size;
        if (i > 0) {
            // not encode last value after first row
            len -= posting_value->GetSize();
        }
        if (len == 0) {
            break;
        }
        bool ret = iter.SeekSlice(start);
        assert(ret);
        (void)ret;
        while (iter.HasNext(start + len)) {
            void *data = NULL;
            size_t size = 0;
            iter.Next(data, size);
            assert(data);
            assert(size);
            file->Write((char *)data, size);
        }
        start += posting_value->GetSize() * skiplist_size;
    }
}

size_t BufferedSkipListWriter::EstimateDumpSize() const {
    uint32_t skiplist_size = GetTotalCount();
    if (skiplist_size == 0) {
        return 0;
    }
    if (GetPostingValues()->GetSize() != 3) {
        return posting_writer_.GetSize();
    }

    const PostingValues *posting_values = GetPostingValues();
    assert(posting_values->GetSize() == 3);

    size_t opt_size = posting_values->GetValue(1)->GetSize() + posting_values->GetValue(1)->GetSize();
    return posting_writer_.GetSize() - opt_size;
}
} // namespace infinity
