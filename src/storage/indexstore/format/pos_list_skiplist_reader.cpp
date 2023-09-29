#include "pos_list_skiplist_reader.h"
#include "posting_value.h"
#include "common/utility/infinity_assert.h"
#include "main/logger.h"

namespace infinity{

PosListSkipListReader::PosListSkipListReader()
    : start_(0)
    , end_(0)
    , skipped_item_count_(-1)
    , current_key_(0)
    , current_value_(0)
    , prev_key_(0)
    , prev_value_(0)
    , current_cursor_(0)
    , num_in_buffer_(0)
    , key_buffer_base_(key_buffer_)
    , value_buffer_base_(value_buffer_) {}

PosListSkipListReader::PosListSkipListReader(const PosListSkipListReader& other) noexcept
    : start_(other.start_)
    , end_(other.end_)
    , byte_slice_reader_(other.byte_slice_reader_)
    , skipped_item_count_(other.skipped_item_count_)
    , current_key_(other.current_key_)
    , current_value_(other.current_value_)
    , prev_key_(other.prev_key_)
    , prev_value_(other.prev_value_)
    , current_cursor_(0)
    , num_in_buffer_(0)
    , key_buffer_base_(key_buffer_)
    , value_buffer_base_(value_buffer_) {}


PosListSkipListReader::~PosListSkipListReader() {
}

void PosListSkipListReader::Load(const ByteSliceList* byte_slice_list, uint32_t start, uint32_t end, const uint32_t& item_count) {
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(const_cast<ByteSliceList*>(byte_slice_list));
    byte_slice_reader_.Seek(start);
    Load_(start, end, item_count);
}

void PosListSkipListReader::Load(ByteSlice* byte_slice, uint32_t start, uint32_t end, const uint32_t& item_count) {
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(byte_slice);
    byte_slice_reader_.Seek(start);
    Load_(start, end, item_count);
}

void PosListSkipListReader::Load_(uint32_t start, uint32_t end , const uint32_t& item_count) {
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
        assert(end_ == byte_slice_reader_.Tell());
    }
}

bool PosListSkipListReader::SkipTo(uint32_t query_key, uint32_t& key, uint32_t& prev_key, uint32_t& value, uint32_t& delta) {
    assert(current_key_ <= query_key);

    uint32_t local_prev_key, local_prev_value;
    uint32_t current_key = current_key_;
    uint32_t current_value = current_value_;
    uint32_t current_cursor = current_cursor_;
    int32_t skipped_item_count = skipped_item_count_;
    uint32_t num_in_buffer = num_in_buffer_;
    while (true) {
        // TODO: skipped_item_count should not add after skipto failed
        skipped_item_count++;

        local_prev_key = current_key;
        local_prev_value = current_value;

        if (current_cursor >= num_in_buffer) {
            auto [status,ret] = LoadBuffer();
            if(-1 == status) return false;
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

std::pair<int, bool> PosListSkipListReader::LoadBuffer() {
    uint32_t end = byte_slice_reader_.Tell();
    if (end < end_) {
        key_buffer_base_ = key_buffer_;
        const Int32Encoder* key_encoder = GetSkipListEncoder();
        auto key_num = key_encoder->Decode(
            key_buffer_base_, sizeof(key_buffer_) / sizeof(key_buffer_[0]), byte_slice_reader_);

        value_buffer_base_ = value_buffer_;
        const Int32Encoder* offset_encoder = GetSkipListEncoder();
        auto value_num = offset_encoder->Decode(
            value_buffer_base_, sizeof(value_buffer_) / sizeof(value_buffer_[0]), byte_slice_reader_);

        if (key_num != value_num) {
            LOG_ERROR(fmt::format("SKipList decode error, key_num = {} offset_num = {}", key_num, value_num));
            return std::make_pair(-1, false);
        }
        num_in_buffer_ = key_num;
        current_cursor_ = 0;
        return std::make_pair(0, true);
    }
    return std::make_pair(0, false);
}

}