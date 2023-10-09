#include "inmem_tri_value_skiplist_reader.h"
#include "main/logger.h"

namespace infinity {

void InMemTriValueSkipListReader::Load(BufferedByteSlice *posting_buffer) {
    InitMember();
    BufferedByteSlice *skiplist_buffer = new BufferedByteSlice(session_pool_, session_pool_);
    posting_buffer->SnapShot(skiplist_buffer);

    skiplist_buffer_ = skiplist_buffer;
    skiplist_reader_.Open(skiplist_buffer_);
}

std::pair<int, bool> InMemTriValueSkipListReader::LoadBuffer() {
    size_t flush_count = skiplist_buffer_->GetTotalCount();
    FlushInfo flushInfo = skiplist_buffer_->GetFlushInfo();

    size_t decode_count = SKIP_LIST_BUFFER_SIZE;
    if (flushInfo.IsValidShortBuffer() == false) {
        decode_count = flush_count;
    }
    if (decode_count == 0) {
        return std::make_pair(0, false);
    }

    size_t doc_num = 0;
    if (!skiplist_reader_.Decode(doc_id_buffer_, decode_count, doc_num)) {
        return std::make_pair(0, false);
    }

    size_t ttf_num = 0;
    if (!skiplist_reader_.Decode(ttf_buffer_, decode_count, ttf_num)) {
        return std::make_pair(0, false);
    }

    size_t len_num = 0;
    if (!skiplist_reader_.Decode(offset_buffer_, decode_count, len_num)) {
        return std::make_pair(0, false);
    }

    if (doc_num != ttf_num || ttf_num != len_num) {
        LOG_ERROR(fmt::format("SKipList decode error, doc_num = {} ttf_num = {} len_num = {}", doc_num, ttf_num, len_num));
        return std::make_pair(-1, false);
    }
    num_in_buffer_ = doc_num;
    current_cursor_ = 0;
    return std::make_pair(0, true);
}

uint32_t InMemTriValueSkipListReader::GetLastValueInBuffer() const {
    uint32_t last_value_in_buffer = current_offset_;
    uint32_t current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_value_in_buffer += offset_buffer_[current_cursor];
        current_cursor++;
    }
    return last_value_in_buffer;
}

uint32_t InMemTriValueSkipListReader::GetLastKeyInBuffer() const {
    uint32_t last_key_in_buffer = current_doc_id_;
    uint32_t current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_key_in_buffer += doc_id_buffer_[current_cursor];
        current_cursor++;
    }
    return last_key_in_buffer;
}

} // namespace infinity
