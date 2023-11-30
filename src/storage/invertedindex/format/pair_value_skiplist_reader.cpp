module;

import stl;
import byte_slice;
import byte_slice_reader;

import infinity_exception;
import skiplist_reader;
import index_defines;
import posting_value;

module pair_value_skiplist_reader;

namespace infinity {

PairValueSkipListReader::PairValueSkipListReader()
    : current_key_(0), current_value_(0), prev_key_(0), prev_value_(0), current_cursor_(0), num_in_buffer_(0), key_buffer_base_(key_buffer_),
      value_buffer_base_(value_buffer_) {}

PairValueSkipListReader::PairValueSkipListReader(const PairValueSkipListReader &other) noexcept
    : current_key_(other.current_key_), current_value_(other.current_value_), prev_key_(other.prev_key_), prev_value_(other.prev_value_),
      current_cursor_(0), num_in_buffer_(0), key_buffer_base_(key_buffer_), value_buffer_base_(value_buffer_) {}

PairValueSkipListReader::~PairValueSkipListReader() {}

void PairValueSkipListReader::Load(const ByteSliceList *byte_slice_list, u32 start, u32 end, const u32 &item_count) {
    SkipListReader::Load(byte_slice_list, start, end);
    Load_(start, end, item_count);
}

void PairValueSkipListReader::Load(ByteSlice *byte_slice, u32 start, u32 end, const u32 &item_count) {
    SkipListReader::Load(byte_slice, start, end);
    Load_(start, end, item_count);
}

void PairValueSkipListReader::Load_(u32, u32, const u32 &item_count) {
    skipped_item_count_ = -1;
    current_key_ = 0;
    current_value_ = 0;
    prev_key_ = 0;
    prev_value_ = 0;
    current_cursor_ = 0;
    num_in_buffer_ = 0;
    key_buffer_base_ = key_buffer_;
    value_buffer_base_ = value_buffer_;

    if (item_count <= MAX_UNCOMPRESSED_SKIP_LIST_SIZE) {
        byte_slice_reader_.Read(key_buffer_, item_count * sizeof(key_buffer_[0]));
        byte_slice_reader_.Read(value_buffer_, item_count * sizeof(value_buffer_[0]));
        num_in_buffer_ = item_count;
        Assert<StorageException>(end_ == byte_slice_reader_.Tell(), "end_ == byte_slice_reader_.Tell().");
    }
}

bool PairValueSkipListReader::SkipTo(u32 query_key, u32 &key, u32 &prev_key, u32 &value, u32 &delta) {
    Assert<StorageException>(current_key_ <= query_key, "current_key_ <= query_key.");

    u32 local_prev_key, local_prev_value;
    u32 current_key = current_key_;
    u32 current_value = current_value_;
    u32 current_cursor = current_cursor_;
    i32 skipped_item_count = skipped_item_count_;
    u32 num_in_buffer = num_in_buffer_;
    while (true) {
        // TODO: skipped_item_count should not add after skipto failed
        skipped_item_count++;

        local_prev_key = current_key;
        local_prev_value = current_value;

        if (current_cursor >= num_in_buffer) {
            auto [status, ret] = LoadBuffer();
            if (-1 == status)
                return false;
            if (!ret) {
                break;
            }
            current_cursor = current_cursor_;
            num_in_buffer = num_in_buffer_;
        }
        current_key = key_buffer_base_[current_cursor] + current_key;
        current_value += value_buffer_base_[current_cursor];
        current_cursor++;

        if (current_key >= query_key) {
            key = current_key;
            prev_key = prev_key_ = local_prev_key;
            value = prev_value_ = local_prev_value;
            delta = current_value - local_prev_value;
            current_key_ = current_key;
            current_value_ = current_value;
            current_cursor_ = current_cursor;
            skipped_item_count_ = skipped_item_count;

            return true;
        }
    }

    current_key_ = current_key;
    current_value_ = current_value;
    current_cursor_ = current_cursor;

    skipped_item_count_ = skipped_item_count;
    return false;
}

Pair<int, bool> PairValueSkipListReader::LoadBuffer() {
    u32 end = byte_slice_reader_.Tell();
    if (end < end_) {
        key_buffer_base_ = key_buffer_;
        const Int32Encoder *key_encoder = GetSkipListEncoder();
        auto key_num = key_encoder->Decode(key_buffer_base_, sizeof(key_buffer_) / sizeof(key_buffer_[0]), byte_slice_reader_);

        value_buffer_base_ = value_buffer_;
        const Int32Encoder *offset_encoder = GetSkipListEncoder();
        auto value_num = offset_encoder->Decode(value_buffer_base_, sizeof(value_buffer_) / sizeof(value_buffer_[0]), byte_slice_reader_);

        if (key_num != value_num) {
            // LOG_ERROR(fmt::format("SKipList decode error, key_num = {} offset_num = {}", key_num, value_num));
            return MakePair(-1, false);
        }
        num_in_buffer_ = key_num;
        current_cursor_ = 0;
        return MakePair(0, true);
    }
    return MakePair(0, false);
}

u32 PairValueSkipListReader::GetLastValueInBuffer() const {
    u32 last_value_in_buffer = current_value_;
    u32 current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_value_in_buffer = value_buffer_base_[current_cursor] + last_value_in_buffer;
        current_cursor++;
    }
    return last_value_in_buffer;
}

u32 PairValueSkipListReader::GetLastKeyInBuffer() const {
    u32 last_key_in_buffer = current_key_;
    u32 current_cursor = current_cursor_;
    while (current_cursor < num_in_buffer_) {
        last_key_in_buffer = key_buffer_base_[current_cursor] + last_key_in_buffer;
        current_cursor++;
    }
    return last_key_in_buffer;
}

} // namespace infinity