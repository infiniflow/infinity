#pragma once

#include "buffered_byte_slice.h"
#include "buffered_skiplist_writer.h"
#include "common/memory/memory_pool.h"
#include "inmem_pos_list_decoder.h"
#include "pos_list_format_option.h"
#include "storage/io/byte_slice_writer.h"
#include "storage/io/file_writer.h"

#include <memory>

namespace infinity {

class PositionListEncoder {
public:
    PositionListEncoder(
            const PositionListFormatOption& pos_list_format_option, MemoryPool* byte_slice_pool,
            MemoryPool* buffer_pool, const PositionListFormat* posListFormat = nullptr)
        : pos_list_buffer_(byte_slice_pool, buffer_pool), last_pos_in_cur_doc_(0), total_pos_count_(0), pos_list_format_option_(pos_list_format_option), is_own_format_(false), pos_skiplist_writer_(nullptr), pos_list_format_(posListFormat), byte_slice_pool_(byte_slice_pool) {
        assert(pos_list_format_option.HasPositionList());
        if(!posListFormat) {
            pos_list_format_ = new PositionListFormat(pos_list_format_option);
            is_own_format_ = true;
        }
        pos_list_buffer_.Init(pos_list_format_);
    }

    ~PositionListEncoder() {
        if(pos_skiplist_writer_) {
            pos_skiplist_writer_->~BufferedSkipListWriter();
            pos_skiplist_writer_ = nullptr;
        }
        if(is_own_format_) {
            delete pos_list_format_;
            pos_list_format_ = nullptr;
        }
    }

    void
    AddPosition(pos_t pos);
    void
    EndDocument();
    void
    Flush();
    void
    Dump(const std::shared_ptr<FileWriter>& file);
    uint32_t
    GetDumpLength() const;

    InMemPositionListDecoder*
    GetInMemPositionListDecoder(MemoryPool* session_pool) const;

    const ByteSliceList*
    GetPositionList() const { return pos_list_buffer_.GetByteSliceList(); }

    void
    SetDocSkipListWriter(BufferedSkipListWriter* writer) { pos_skiplist_writer_ = writer; }

    const PositionListFormat*
    GetPositionListFormat() const { return pos_list_format_; }

private:
    void
    CreatePosSkipListWriter();
    void
    AddPosSkipListItem(uint32_t total_pos_count, uint32_t compressed_pos_size, bool need_flush);
    void
    FlushPositionBuffer();

private:
    BufferedByteSlice pos_list_buffer_;
    pos_t last_pos_in_cur_doc_;                      // 4byte
    uint32_t total_pos_count_;                       // 4byte
    PositionListFormatOption pos_list_format_option_;// 1byte
    bool is_own_format_;                             // 1byte
    BufferedSkipListWriter* pos_skiplist_writer_;
    const PositionListFormat* pos_list_format_;
    MemoryPool* byte_slice_pool_;

    friend class PositionListEncoderTest;
};

}// namespace infinity
