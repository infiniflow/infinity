module;

module skiplist_reader;

import stl;
import byte_slice;
import status;
import posting_field;
import flush_info;
import infinity_exception;
import logger;
import third_party;

namespace infinity {

bool SkipListReader::SkipTo(u32 query_doc_id, u32 &doc_id, u32 &prev_doc_id, u32 &offset, u32 &delta) {
    u32 current_doc_id = current_doc_id_;
    u32 current_offset = current_offset_;
    u32 current_ttf = current_ttf_;
    u32 current_cursor = current_cursor_;
    i32 skipped_item_count = skipped_item_count_;
    u32 num_in_buffer = num_in_buffer_;
    for (;; ++current_cursor) {
        const u32 local_prev_doc_id = current_doc_id;
        const u32 local_prev_offset = current_offset;
        const u32 local_prev_ttf = current_ttf;
        if (current_cursor >= num_in_buffer) {
            auto [status, ret] = LoadBuffer();
//            assert(status == 0);
            if (!ret) {
                // current segment is exhausted
                // skip current block
                ++skipped_item_count;
                break;
            }
            current_cursor = current_cursor_;
            num_in_buffer = num_in_buffer_;
        }
        current_doc_id += doc_id_buffer_[current_cursor];
        current_offset += offset_buffer_[current_cursor];
        if (has_tf_list_) {
            current_ttf += ttf_buffer_[current_cursor];
        }
        ++skipped_item_count;
        if (current_doc_id >= query_doc_id) {
            skipped_item_count_ = skipped_item_count;
            doc_id = current_doc_id_ = current_doc_id;
            current_offset_ = current_offset;
            current_ttf_ = current_ttf;
            if (has_block_max_) {
                current_block_max_tf_ = block_max_tf_buffer_[current_cursor];
                current_block_max_tf_percentage_ = block_max_tf_percentage_buffer_[current_cursor];
            }
            prev_doc_id = prev_doc_id_ = local_prev_doc_id;
            offset = prev_offset_ = local_prev_offset;
            delta = offset_buffer_[current_cursor];
            prev_ttf_ = local_prev_ttf;
            // point to next block
            current_cursor_ = current_cursor + 1;
            return true;
        }
    }
    // case for LoadBuffer() return (0, false)
    skipped_item_count_ = skipped_item_count;
    current_doc_id_ = current_doc_id;
    current_offset_ = current_offset;
    current_ttf_ = current_ttf;
    current_cursor_ = current_cursor;
    return false;
}

u32 SkipListReader::GetLastValueInBuffer() const {
    u32 last_value_in_buffer = current_offset_;
    for (u32 current_cursor = current_cursor_; current_cursor < num_in_buffer_; ++current_cursor) {
        last_value_in_buffer += offset_buffer_[current_cursor];
    }
    return last_value_in_buffer;
}

u32 SkipListReader::GetLastKeyInBuffer() const {
    u32 last_key_in_buffer = current_doc_id_;
    for (u32 current_cursor = current_cursor_; current_cursor < num_in_buffer_; ++current_cursor) {
        last_key_in_buffer += doc_id_buffer_[current_cursor];
    }
    return last_key_in_buffer;
}

void SkipListReaderByteSlice::Load(const ByteSliceList *byte_slice_list, u32 start, u32 end) {
    if (start > byte_slice_list->GetTotalSize()) {
        UnrecoverableError("start > byte_slice_list->GetTotalSize().");
    }
    if (end > byte_slice_list->GetTotalSize()) {
        UnrecoverableError("end > byte_slice_list->GetTotalSize().");
    }
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(const_cast<ByteSliceList *>(byte_slice_list));
    byte_slice_reader_.Seek(start);
}

void SkipListReaderByteSlice::Load(ByteSlice *byte_slice, u32 start, u32 end) {
    if (start > byte_slice->size_) {
        UnrecoverableError("start > byte_slice->size_.");
    }
    if (end > byte_slice->size_) {
        UnrecoverableError("end > byte_slice->size_.");
    }

    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(byte_slice);
    byte_slice_reader_.Seek(start);
}

Pair<int, bool> SkipListReaderByteSlice::LoadBuffer() {
    u32 end = byte_slice_reader_.Tell();
    if (end < end_) {
        const Int32Encoder *doc_id_encoder = GetSkipListEncoder();
        u32 doc_num = doc_id_encoder->Decode(static_cast<u32 *>(doc_id_buffer_), SKIP_LIST_BUFFER_SIZE, byte_slice_reader_);
        if (has_tf_list_) {
            const Int32Encoder *tf_encoder = GetSkipListEncoder();
            u32 ttf_num = tf_encoder->Decode(ttf_buffer_.get(), SKIP_LIST_BUFFER_SIZE, byte_slice_reader_);
            if (ttf_num != doc_num) {
                UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} ttf_num = {}", doc_num, ttf_num));
                return MakePair(-1, false);
            }
        }
        if (has_block_max_) {
            const Int32Encoder *block_max_tf_encoder = GetSkipListEncoder();
            u32 block_max_tf_num = block_max_tf_encoder->Decode(block_max_tf_buffer_.get(), SKIP_LIST_BUFFER_SIZE, byte_slice_reader_);
            if (block_max_tf_num != doc_num) {
                UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} block_max_tf_num = {}", doc_num, block_max_tf_num));
                return MakePair(-1, false);
            }
            const Int16Encoder *tf_percentage_encoder = GetTermPercentageEncoder();
            u32 tf_percentage_num = tf_percentage_encoder->Decode(block_max_tf_percentage_buffer_.get(), SKIP_LIST_BUFFER_SIZE, byte_slice_reader_);
            if (tf_percentage_num != doc_num) {
                UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} block_max_tf_percentage_num = {}", doc_num, tf_percentage_num));
                return MakePair(-1, false);
            }
        }
        {
            const Int32Encoder *offset_encoder = GetSkipListEncoder();
            u32 len_num = offset_encoder->Decode(static_cast<u32 *>(offset_buffer_), SKIP_LIST_BUFFER_SIZE, byte_slice_reader_);
            if (len_num != doc_num) {
                UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} offset_num = {}", doc_num, len_num));
                return MakePair(-1, false);
            }
        }
        num_in_buffer_ = doc_num;
        current_cursor_ = 0;
        return MakePair(0, true);
    }
    return MakePair(0, false);
}

SkipListReaderPostingByteSlice::~SkipListReaderPostingByteSlice() {
    delete skiplist_buffer_;
    skiplist_buffer_ = nullptr;
}

void SkipListReaderPostingByteSlice::Load(const PostingByteSlice *posting_buffer) {
    PostingByteSlice *skiplist_buffer = new PostingByteSlice();
    posting_buffer->SnapShot(skiplist_buffer);

    skiplist_buffer_ = skiplist_buffer;
    skiplist_reader_.Open(skiplist_buffer_);
}

Pair<int, bool> SkipListReaderPostingByteSlice::LoadBuffer() {
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

    if (has_tf_list_) {
        SizeT ttf_num = 0;
        if (!skiplist_reader_.Decode(ttf_buffer_.get(), decode_count, ttf_num)) {
            return MakePair(0, false);
        }
        if (doc_num != ttf_num) {
            UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} ttf_num = {}", doc_num, ttf_num));
            return MakePair(-1, false);
        }
    }

    if (has_block_max_) {
        SizeT block_max_tf_num = 0;
        if (!skiplist_reader_.Decode(block_max_tf_buffer_.get(), decode_count, block_max_tf_num)) {
            return MakePair(0, false);
        }
        if (doc_num != block_max_tf_num) {
            UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} block_max_tf_num = {}", doc_num, block_max_tf_num));
            return MakePair(-1, false);
        }

        SizeT tf_percentage_num = 0;
        if (!skiplist_reader_.Decode(block_max_tf_percentage_buffer_.get(), decode_count, tf_percentage_num)) {
            return MakePair(0, false);
        }
        if (doc_num != tf_percentage_num) {
            UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} block_max_tf_percentage_num = {}", doc_num, tf_percentage_num));
            return MakePair(-1, false);
        }
    }

    SizeT len_num = 0;
    if (!skiplist_reader_.Decode(offset_buffer_, decode_count, len_num)) {
        return MakePair(0, false);
    }
    if (doc_num != len_num) {
        UnrecoverableError(fmt::format("SKipList decode error, doc_num = {} len_num = {}", doc_num, len_num));
        return MakePair(-1, false);
    }

    num_in_buffer_ = doc_num;
    current_cursor_ = 0;
    return MakePair(0, true);
}

} // namespace infinity
