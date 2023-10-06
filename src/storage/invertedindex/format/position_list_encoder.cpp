#include "position_list_encoder.h"
#include "inmem_pair_value_skiplist_reader.h"
#include "posting_value.h"
#include "short_list_optimize_util.h"
#include "storage/compression/vbyte_compressor.h"

namespace infinity {

void
PositionListEncoder::AddPosition(pos_t pos) {
    pos_list_buffer_.PushBack(0, pos - last_pos_in_cur_doc_);
    pos_list_buffer_.EndPushBack();

    last_pos_in_cur_doc_ = pos;
    ++total_pos_count_;

    if(pos_list_buffer_.NeedFlush(MAX_POS_PER_RECORD)) {
        FlushPositionBuffer();
    }
}

void
PositionListEncoder::EndDocument() { last_pos_in_cur_doc_ = 0; }

void
PositionListEncoder::Flush() {
    FlushPositionBuffer();
    if(pos_skiplist_writer_) {
        pos_skiplist_writer_->Flush();
    }
}

void
PositionListEncoder::Dump(const std::shared_ptr<FileWriter>& file) {
    Flush();
    uint32_t pos_list_size = pos_list_buffer_.EstimateDumpSize();
    uint32_t pos_skiplist_size = 0;
    if(pos_skiplist_writer_) {
        pos_skiplist_size = pos_skiplist_writer_->EstimateDumpSize();
    }

    file->WriteVInt(pos_skiplist_size);
    file->WriteVInt(pos_list_size);
    if(pos_skiplist_writer_) {
        pos_skiplist_writer_->Dump(file);
    }
    pos_list_buffer_.Dump(file);
}

uint32_t
PositionListEncoder::GetDumpLength() const {
    uint32_t pos_skiplist_size = 0;
    if(pos_skiplist_writer_) {
        pos_skiplist_size = pos_skiplist_writer_->EstimateDumpSize();
    }

    uint32_t pos_list_size = pos_list_buffer_.EstimateDumpSize();
    return VByteCompressor::GetVInt32Length(pos_skiplist_size) + VByteCompressor::GetVInt32Length(pos_list_size) +
           pos_skiplist_size + pos_list_size;
}

void
PositionListEncoder::CreatePosSkipListWriter() {
    assert(pos_skiplist_writer_ == nullptr);
    MemoryPool* bufferPool =
            dynamic_cast<MemoryPool*>(pos_list_buffer_.GetBufferPool());
    assert(bufferPool);

    void* buffer = byte_slice_pool_->Allocate(sizeof(BufferedSkipListWriter));
    BufferedSkipListWriter* pos_skiplist_writer = new(buffer) BufferedSkipListWriter(byte_slice_pool_, bufferPool);
    pos_skiplist_writer->Init(pos_list_format_->GetPositionSkipListFormat());

    pos_skiplist_writer_ = pos_skiplist_writer;
}

void
PositionListEncoder::AddPosSkipListItem(uint32_t total_pos_count, uint32_t compressed_pos_size, bool need_flush) {
    if(pos_list_format_option_.HasTfBitmap()) {
        if(need_flush) {
            pos_skiplist_writer_->AddItem(compressed_pos_size);
        }
    } else {
        pos_skiplist_writer_->AddItem(total_pos_count, compressed_pos_size);
    }
}

void
PositionListEncoder::FlushPositionBuffer() {
    // TODO: uncompress need this
    bool need_flush = pos_list_buffer_.NeedFlush(MAX_POS_PER_RECORD);

    uint32_t flush_size = pos_list_buffer_.Flush();
    if(flush_size > 0) {
        if(pos_skiplist_writer_ == nullptr) {
            CreatePosSkipListWriter();
        }
        AddPosSkipListItem(total_pos_count_, flush_size, need_flush);
    }
}

InMemPositionListDecoder*
PositionListEncoder::GetInMemPositionListDecoder(MemoryPool* session_pool) const {
    // doclist -> ttf -> pos skiplist -> poslist
    ttf_t ttf = total_pos_count_;

    // TODO: delete buffer in MemoryPool
    InMemPairValueSkipListReader* in_mem_skiplist_reader = nullptr;
    if(pos_skiplist_writer_) {
        // not support tf bitmap in realtime segment
        assert(!pos_list_format_option_.HasTfBitmap());
        in_mem_skiplist_reader = new InMemPairValueSkipListReader(session_pool);
        in_mem_skiplist_reader->Load(pos_skiplist_writer_);
    }
    BufferedByteSlice* posting_buffer = new(session_pool->Allocate(sizeof(BufferedByteSlice))) BufferedByteSlice(session_pool, session_pool);
    pos_list_buffer_.SnapShot(posting_buffer);

    InMemPositionListDecoder* decoder = new InMemPositionListDecoder(pos_list_format_option_, session_pool);
    decoder->Init(ttf, in_mem_skiplist_reader, posting_buffer);

    return decoder;
}

}// namespace infinity
