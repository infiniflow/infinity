module;
#include <cassert>

import stl;
import byte_slice;
import memory_pool;
import file_writer;
import index_defines;
import posting_value;
import buffered_byte_slice;
import short_list_optimize_util;

module buffered_skiplist_writer;

namespace infinity {

BufferedSkipListWriter::BufferedSkipListWriter(MemoryPool *byte_slice_pool, MemoryPool *buffer_pool)
    : BufferedByteSlice(byte_slice_pool, buffer_pool), last_key_(0), last_value1_(0) {}

void BufferedSkipListWriter::AddItem(u32 key, u32 value1, u32 value2) {
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

void BufferedSkipListWriter::AddItem(u32 key, u32 value1) {
    PushBack(0, key - last_key_);
    PushBack(1, value1);
    EndPushBack();

    last_key_ = key;

    if (NeedFlush(SKIP_LIST_BUFFER_SIZE)) {
        Flush();
    }
}

void BufferedSkipListWriter::AddItem(u32 value_delta) {
    last_value1_ += value_delta;
    PushBack(0, last_value1_);
    EndPushBack();

    if (NeedFlush(SKIP_LIST_BUFFER_SIZE)) {
        Flush();
    }
}

SizeT BufferedSkipListWriter::DoFlush() {
    u32 flush_size = 0;
    u8 size = buffer_.Size();

    const PostingValues *posting_values = GetPostingValues();
    for (SizeT i = 0; i < posting_values->GetSize(); ++i) {
        PostingValue *posting_value = posting_values->GetValue(i);
        u8 *buffer = buffer_.GetRow(posting_value->location_);
        flush_size += posting_value->Encode(posting_writer_, buffer, size * posting_value->GetSize());
    }
    return flush_size;
}

void BufferedSkipListWriter::Dump(const SharedPtr<FileWriter> &file) {
    u32 skiplist_size = GetTotalCount();
    if (skiplist_size == 0) {
        return;
    }
    if (GetPostingValues()->GetSize() != 3) {
        posting_writer_.Dump(file);
        return;
    }

    const ByteSliceList *skipList = posting_writer_.GetByteSliceList();
    ByteSliceListIterator iter(skipList);
    u32 start = 0;
    const PostingValues *posting_values = GetPostingValues();
    for (SizeT i = 0; i < posting_values->GetSize(); ++i) {
        PostingValue *posting_value = posting_values->GetValue(i);
        u32 len = posting_value->GetSize() * skiplist_size;
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
            void *data = nullptr;
            SizeT size = 0;
            iter.Next(data, size);
            assert(data);
            assert(size);
            file->Write((char *)data, size);
        }
        start += posting_value->GetSize() * skiplist_size;
    }
}

SizeT BufferedSkipListWriter::EstimateDumpSize() const {
    u32 skiplist_size = GetTotalCount();
    if (skiplist_size == 0) {
        return 0;
    }
    if (GetPostingValues()->GetSize() != 3) {
        return posting_writer_.GetSize();
    }

    const PostingValues *posting_values = GetPostingValues();
    assert(posting_values->GetSize() == 3);

    SizeT opt_size = posting_values->GetValue(1)->GetSize() + posting_values->GetValue(1)->GetSize();
    return posting_writer_.GetSize() - opt_size;
}
} // namespace infinity
