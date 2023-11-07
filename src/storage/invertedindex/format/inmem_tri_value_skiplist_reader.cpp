module;

import stl;
import std;
import memory_pool;
import buffered_byte_slice;
import buffered_byte_slice_reader;
import tri_value_skiplist_reader;
import index_defines;
import flush_info;

module inmem_tri_value_skiplist_reader;

namespace infinity {

void InMemTriValueSkipListReader::Load(BufferedByteSlice *posting_buffer) {
    InitMember();
    BufferedByteSlice *skiplist_buffer = new BufferedByteSlice(session_pool_, session_pool_);
    posting_buffer->SnapShot(skiplist_buffer);

    skiplist_buffer_ = skiplist_buffer;
    skiplist_reader_.Open(skiplist_buffer_);
}

Pair<int, bool> InMemTriValueSkipListReader::LoadBuffer() {
    SizeT flush_count = skiplist_buffer_->GetTotalCount();
    FlushInfo flushInfo = skiplist_buffer_->GetFlushInfo();

    SizeT decode_count = SKIP_LIST_BUFFER_SIZE;
    if (flushInfo.IsValidShortBuffer() == false) {
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

u32 InMemTriValueSkipListReader::GetLastValueInBuffer() const {
    u32 last_value_in_buffer = current_offset_;
    u32 current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_value_in_buffer += offset_buffer_[current_cursor];
        current_cursor++;
    }
    return last_value_in_buffer;
}

u32 InMemTriValueSkipListReader::GetLastKeyInBuffer() const {
    u32 last_key_in_buffer = current_doc_id_;
    u32 current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_key_in_buffer += doc_id_buffer_[current_cursor];
        current_cursor++;
    }
    return last_key_in_buffer;
}

} // namespace infinity
