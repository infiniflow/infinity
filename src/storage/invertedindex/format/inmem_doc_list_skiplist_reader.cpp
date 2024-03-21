module;

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import doc_list_skiplist_reader;
import index_defines;
import flush_info;

module inmem_doc_list_skiplist_reader;

namespace infinity {

void InMemDocListSkipListReader::Load(PostingByteSlice *posting_buffer) {
    InitMember();
    PostingByteSlice *skiplist_buffer = session_pool_ ? new (session_pool_->Allocate(sizeof(PostingByteSlice)))
                                                            PostingByteSlice(session_pool_, session_pool_)
                                                      : new PostingByteSlice(session_pool_, session_pool_);
    posting_buffer->SnapShot(skiplist_buffer);

    skiplist_buffer_ = skiplist_buffer;
    skiplist_reader_.Open(skiplist_buffer_);
}

Pair<int, bool> InMemDocListSkipListReader::LoadBuffer() {
    SizeT flush_count = skiplist_buffer_->GetTotalCount();
    FlushInfo flush_info = skiplist_buffer_->GetFlushInfo();

    SizeT decode_count = SKIP_LIST_BUFFER_SIZE;
    if (flush_info.IsValidPostingBuffer() == false) {
        decode_count = flush_count;
    }
    if (decode_count == 0) {
        return MakePair(0, false);
    }

    SizeT doc_num = 0;
    if (!skiplist_reader_.Decode(doc_id_buffer_, decode_count, doc_num)) {
        return MakePair(0, false);
    }

    SizeT ttf_num = 0;
    if (!skiplist_reader_.Decode(ttf_buffer_, decode_count, ttf_num)) {
        return MakePair(0, false);
    }

    SizeT len_num = 0;
    if (!skiplist_reader_.Decode(offset_buffer_, decode_count, len_num)) {
        return MakePair(0, false);
    }

    if (doc_num != ttf_num || ttf_num != len_num) {
        // LOG_ERROR(fmt::format("SKipList decode error, doc_num = {} ttf_num = {} len_num = {}", doc_num, ttf_num, len_num));
        return MakePair(-1, false);
    }
    num_in_buffer_ = doc_num;
    current_cursor_ = 0;
    return MakePair(0, true);
}

u32 InMemDocListSkipListReader::GetLastValueInBuffer() const {
    u32 last_value_in_buffer = current_offset_;
    u32 current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_value_in_buffer += offset_buffer_[current_cursor];
        current_cursor++;
    }
    return last_value_in_buffer;
}

u32 InMemDocListSkipListReader::GetLastKeyInBuffer() const {
    u32 last_key_in_buffer = current_doc_id_;
    u32 current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_key_in_buffer += doc_id_buffer_[current_cursor];
        current_cursor++;
    }
    return last_key_in_buffer;
}

} // namespace infinity
