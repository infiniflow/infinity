module;

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import position_list_skiplist_reader;
import index_defines;
import logger;
import flush_info;
import third_party;

module inmem_position_list_skiplist_reader;

namespace infinity {
InMemPositionListSkipListReader::InMemPositionListSkipListReader(MemoryPool *session_pool) : session_pool_(session_pool), skiplist_buffer_(nullptr) {}

InMemPositionListSkipListReader::~InMemPositionListSkipListReader() {
    if (session_pool_) {
        skiplist_buffer_->~PostingByteSlice();
        session_pool_->Deallocate((void *)skiplist_buffer_, sizeof(PostingByteSlice));
    } else {
        delete skiplist_buffer_;
        skiplist_buffer_ = nullptr;
    }
}

void InMemPositionListSkipListReader::Load(PostingByteSlice *posting_buffer) {
    skipped_item_count_ = -1;
    current_key_ = 0;
    current_value_ = 0;
    prev_key_ = 0;
    prev_value_ = 0;
    current_cursor_ = 0;
    num_in_buffer_ = 0;

    PostingByteSlice *skiplist_buffer = session_pool_ ? new (session_pool_->Allocate(sizeof(PostingByteSlice)))
                                                            PostingByteSlice(session_pool_, session_pool_)
                                                      : new PostingByteSlice(session_pool_, session_pool_);
    posting_buffer->SnapShot(skiplist_buffer);

    skiplist_buffer_ = skiplist_buffer;
    skiplist_reader_.Open(skiplist_buffer_);
}

Pair<int, bool> InMemPositionListSkipListReader::LoadBuffer() {
    SizeT key_num = 0;
    SizeT flush_count = skiplist_buffer_->GetTotalCount();
    FlushInfo flush_info = skiplist_buffer_->GetFlushInfo();

    SizeT decode_count = SKIP_LIST_BUFFER_SIZE;
    if (flush_info.IsValidPostingBuffer() == false) {
        decode_count = flush_count;
    }

    if (!skiplist_reader_.Decode(key_buffer_, decode_count, key_num)) {
        return MakePair(0, false);
    }
    SizeT value_num = 0;
    if (!skiplist_reader_.Decode(value_buffer_, decode_count, value_num)) {
        return MakePair(0, false);
    }
    if (key_num != value_num) {
        LOG_ERROR(fmt::format("SKipList decode error, doc_num = {} offset_num = {}", key_num, value_num));
        return MakePair(-1, false);
    }
    num_in_buffer_ = key_num;
    current_cursor_ = 0;
    return MakePair(0, true);
}
} // namespace infinity