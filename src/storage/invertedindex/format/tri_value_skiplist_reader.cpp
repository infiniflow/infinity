#include "tri_value_skiplist_reader.h"
#include "common/utility/infinity_assert.h"
#include "main/logger.h"
#include "posting_value.h"

namespace infinity {

TriValueSkipListReader::TriValueSkipListReader() {
    InitMember();
}

TriValueSkipListReader::TriValueSkipListReader(const TriValueSkipListReader& other) noexcept
    : current_doc_id_(other.current_doc_id_),
      current_offset_(other.current_offset_),
      current_ttf_(other.current_ttf_),
      prev_doc_id_(other.prev_doc_id_),
      prev_offset_(other.prev_offset_),
      prev_ttf_(other.prev_ttf_),
      current_cursor_(0),
      num_in_buffer_(0) {}


TriValueSkipListReader::~TriValueSkipListReader() {}

void
TriValueSkipListReader::InitMember() {
    skipped_item_count_ = -1;
    current_doc_id_ = 0;
    current_offset_ = 0;
    current_ttf_ = 0;
    prev_doc_id_ = 0;
    prev_offset_ = 0;
    prev_ttf_ = 0;
    current_cursor_ = 0;
    num_in_buffer_ = 0;
}

void
TriValueSkipListReader::Load(const ByteSliceList* byte_slice_list,
                             uint32_t start,
                             uint32_t end,
                             const uint32_t& item_count) {
    SkipListReader::Load(byte_slice_list, start, end);
    Load_(start, end, item_count);
}

void
TriValueSkipListReader::Load(ByteSlice* byte_slice, uint32_t start, uint32_t end, const uint32_t& item_count) {
    SkipListReader::Load(byte_slice, start, end);
    Load_(start, end, item_count);
}

void
TriValueSkipListReader::Load_(uint32_t start, uint32_t end, const uint32_t& item_count) {
    InitMember();
    if(item_count <= MAX_UNCOMPRESSED_SKIP_LIST_SIZE) {
        byte_slice_reader_.Read(doc_id_buffer_, item_count * sizeof(doc_id_buffer_[0]));
        byte_slice_reader_.Read(offset_buffer_, (item_count - 1) * sizeof(offset_buffer_[0]));
        byte_slice_reader_.Read(ttf_buffer_, (item_count - 1) * sizeof(ttf_buffer_[0]));
        num_in_buffer_ = item_count;
        assert(end_ == byte_slice_reader_.Tell());
    }
}

bool
TriValueSkipListReader::SkipTo(uint32_t query_doc_id,
                               uint32_t& doc_id,
                               uint32_t& prev_doc_id,
                               uint32_t& offset,
                               uint32_t& delta) {
    assert(current_doc_id_ <= query_doc_id);

    uint32_t current_doc_id = current_doc_id_;
    uint32_t current_offset = current_offset_;
    uint32_t current_ttf = current_ttf_;
    uint32_t current_cursor = current_cursor_;
    int32_t skipped_item_count = skipped_item_count_;
    uint32_t num_in_buffer = num_in_buffer_;

    uint32_t local_prev_doc_id = prev_doc_id_;
    uint32_t local_prev_offset = prev_offset_;
    uint32_t local_prev_ttf = prev_ttf_;

    while(true) {
        // TODO: skipped_item_count should not add after skipto failed
        skipped_item_count++;

        local_prev_doc_id = current_doc_id;
        local_prev_offset = current_offset;
        local_prev_ttf = current_ttf;

        if(current_cursor >= num_in_buffer) {
            auto [status, ret] = LoadBuffer();
            if(-1 == status)
                return false;
            if(!ret) {
                break;
            }
            current_cursor = current_cursor_;
            num_in_buffer = num_in_buffer_;
        }
        current_doc_id += doc_id_buffer_[current_cursor];
        current_offset += offset_buffer_[current_cursor];
        current_ttf += ttf_buffer_[current_cursor];

        current_cursor++;

        if(current_doc_id >= query_doc_id) {
            doc_id = current_doc_id;
            prev_doc_id = prev_doc_id_ = local_prev_doc_id;
            offset = prev_offset_ = local_prev_offset;
            delta = current_offset - local_prev_offset;
            prev_ttf_ = local_prev_ttf;

            current_doc_id_ = current_doc_id;
            current_offset_ = current_offset;
            current_ttf_ = current_ttf;
            current_cursor_ = current_cursor;
            skipped_item_count_ = skipped_item_count;

            return true;
        }
    }

    current_doc_id_ = current_doc_id;
    current_offset_ = current_offset;
    current_ttf_ = current_ttf;
    current_cursor_ = current_cursor;
    skipped_item_count_ = skipped_item_count;
    return false;
}

std::pair<int, bool>
TriValueSkipListReader::LoadBuffer() {
    uint32_t end = byte_slice_reader_.Tell();
    if(end < end_) {
        const Int32Encoder* doc_id_encoder = GetSkipListEncoder();
        auto doc_num = doc_id_encoder->Decode(
                (uint32_t*)doc_id_buffer_, sizeof(doc_id_buffer_) / sizeof(doc_id_buffer_[0]), byte_slice_reader_);

        const Int32Encoder* tf_encoder = GetSkipListEncoder();
        auto ttf_num = tf_encoder->Decode(
                (uint32_t*)ttf_buffer_, sizeof(ttf_buffer_) / sizeof(ttf_buffer_[0]), byte_slice_reader_);


        const Int32Encoder* offset_encoder = GetSkipListEncoder();
        auto len_num = offset_encoder->Decode(
                offset_buffer_, sizeof(offset_buffer_) / sizeof(offset_buffer_[0]), byte_slice_reader_);

        if(doc_num != ttf_num || ttf_num != len_num) {
            LOG_ERROR(fmt::format("SKipList decode error, doc_num = {} offset_num = {} ttf_num = {}",
                                  doc_num,
                                  len_num,
                                  ttf_num));
            return std::make_pair(-1, false);
        }
        num_in_buffer_ = doc_num;
        current_cursor_ = 0;
        return std::make_pair(0, true);
    }
    return std::make_pair(0, false);
}

}// namespace infinity