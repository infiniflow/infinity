module;
#include <cassert>

module buffered_skiplist_writer;
import stl;
import byte_slice;
import memory_pool;
import file_writer;
import file_reader;
import index_defines;
import posting_field;
import buffered_byte_slice;
import short_list_optimize_util;

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

void BufferedSkipListWriter::Dump(const SharedPtr<FileWriter> &file, bool spill) {
    if (spill) {
        file->WriteVInt(posting_writer_.GetSize());
        if (posting_writer_.GetSize() == 0)
            return;
        file->WriteVInt(last_key_);
        file->WriteVInt(last_value1_);
    }
    posting_writer_.Dump(file);
}

void BufferedSkipListWriter::Load(const SharedPtr<FileReader> &file) {
    u32 size = file->ReadVInt();
    if (size == 0)
        return;
    last_key_ = file->ReadVInt();
    last_value1_ = file->ReadVInt();
    posting_writer_.Load(file, size);
}

} // namespace infinity
