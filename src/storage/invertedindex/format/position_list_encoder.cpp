module;
#include <cassert>

module position_list_encoder;
import stl;
import byte_slice_writer;
import memory_pool;
import file_writer;
import file_reader;
import vbyte_compressor;
import index_defines;
import buffered_byte_slice;
import buffered_skiplist_writer;
import position_list_format_option;
import inmem_position_list_decoder;
import inmem_pair_value_skiplist_reader;
import short_list_optimize_util;
import position_bitmap_reader;

namespace infinity {
PositionListEncoder::PositionListEncoder(const PositionListFormatOption &position_list_format_option,
                                         MemoryPool *byte_slice_pool,
                                         MemoryPool *buffer_pool,
                                         const PositionListFormat *pos_list_format)
    : pos_list_buffer_(byte_slice_pool, buffer_pool), last_pos_in_cur_doc_(0), total_pos_count_(0),
      pos_list_format_option_(position_list_format_option), is_own_format_(false), pos_skiplist_writer_(nullptr), pos_list_format_(pos_list_format),
      byte_slice_pool_(byte_slice_pool) {
    if (!pos_list_format) {
        pos_list_format_ = new PositionListFormat(position_list_format_option);
        is_own_format_ = true;
    }
    pos_list_buffer_.Init(pos_list_format_);
    CreatePosSkipListWriter();
}

PositionListEncoder::~PositionListEncoder() {
    if (pos_skiplist_writer_) {
        pos_skiplist_writer_->~BufferedSkipListWriter();
        pos_skiplist_writer_ = nullptr;
    }
    if (is_own_format_) {
        delete pos_list_format_;
        pos_list_format_ = nullptr;
    }
}

void PositionListEncoder::AddPosition(pos_t pos) {
    pos_list_buffer_.PushBack(0, pos - last_pos_in_cur_doc_);
    pos_list_buffer_.EndPushBack();

    last_pos_in_cur_doc_ = pos;
    ++total_pos_count_;

    if (pos_list_buffer_.NeedFlush(MAX_POS_PER_RECORD)) {
        FlushPositionBuffer();
    }
}

void PositionListEncoder::EndDocument() { last_pos_in_cur_doc_ = 0; }

void PositionListEncoder::Flush() {
    FlushPositionBuffer();
    if (pos_skiplist_writer_) {
        pos_skiplist_writer_->Flush();
    }
}

void PositionListEncoder::Dump(const SharedPtr<FileWriter> &file, bool spill) {
    if (spill) {
        file->WriteVInt(last_pos_in_cur_doc_);
        file->WriteVInt(total_pos_count_);
    } else {
        Flush();
        u32 pos_list_size = pos_list_buffer_.EstimateDumpSize();
        u32 pos_skiplist_size = 0;
        if (pos_skiplist_writer_) {
            pos_skiplist_size = pos_skiplist_writer_->EstimateDumpSize();
        }

        file->WriteVInt(pos_skiplist_size);
        file->WriteVInt(pos_list_size);
    }
    if (pos_skiplist_writer_) {
        pos_skiplist_writer_->Dump(file, spill);
    }
    pos_list_buffer_.Dump(file, spill);
}

void PositionListEncoder::Load(const SharedPtr<FileReader> &file) {
    last_pos_in_cur_doc_ = file->ReadVInt();
    total_pos_count_ = file->ReadVInt();
    pos_skiplist_writer_->Load(file);
    pos_list_buffer_.Load(file);
}

u32 PositionListEncoder::GetDumpLength() const {
    u32 pos_skiplist_size = 0;
    if (pos_skiplist_writer_) {
        pos_skiplist_size = pos_skiplist_writer_->EstimateDumpSize();
    }

    u32 pos_list_size = pos_list_buffer_.EstimateDumpSize();
    return VByteCompressor::GetVInt32Length(pos_skiplist_size) + VByteCompressor::GetVInt32Length(pos_list_size) + pos_skiplist_size + pos_list_size;
}

void PositionListEncoder::CreatePosSkipListWriter() {
    assert(pos_skiplist_writer_ == nullptr);
    MemoryPool *bufferPool = dynamic_cast<MemoryPool *>(pos_list_buffer_.GetBufferPool());
    assert(bufferPool);

    void *buffer = byte_slice_pool_->Allocate(sizeof(BufferedSkipListWriter));
    BufferedSkipListWriter *pos_skiplist_writer = new (buffer) BufferedSkipListWriter(byte_slice_pool_, bufferPool);
    pos_skiplist_writer->Init(pos_list_format_->GetPositionSkipListFormat());

    pos_skiplist_writer_ = pos_skiplist_writer;
}

void PositionListEncoder::AddPosSkipListItem(u32 total_pos_count, u32 compressed_pos_size, bool need_flush) {
    if (pos_list_format_option_.HasTfBitmap()) {
        if (need_flush) {
            pos_skiplist_writer_->AddItem(compressed_pos_size);
        }
    } else {
        pos_skiplist_writer_->AddItem(total_pos_count, compressed_pos_size);
    }
}

void PositionListEncoder::FlushPositionBuffer() {
    // TODO: uncompress need this
    bool need_flush = pos_list_buffer_.NeedFlush(MAX_POS_PER_RECORD);

    u32 flush_size = pos_list_buffer_.Flush();
    if (flush_size > 0) {
        if (pos_skiplist_writer_ == nullptr) {
            CreatePosSkipListWriter();
        }
        AddPosSkipListItem(total_pos_count_, flush_size, need_flush);
    }
}

InMemPositionListDecoder *PositionListEncoder::GetInMemPositionListDecoder(MemoryPool *session_pool) const {
    // doclist -> ttf -> pos skiplist -> poslist
    ttf_t ttf = total_pos_count_;

    InMemPairValueSkipListReader *in_mem_skiplist_reader = nullptr;
    if (pos_skiplist_writer_) {
        // not support tf bitmap in realtime segment
        in_mem_skiplist_reader = session_pool ? new (session_pool->Allocate(sizeof(InMemPairValueSkipListReader)))
                                                    InMemPairValueSkipListReader(session_pool)
                                              : new InMemPairValueSkipListReader(session_pool);
        in_mem_skiplist_reader->Load(pos_skiplist_writer_);
    }
    BufferedByteSlice *posting_buffer = new (session_pool->Allocate(sizeof(BufferedByteSlice))) BufferedByteSlice(session_pool, session_pool);
    pos_list_buffer_.SnapShot(posting_buffer);

    InMemPositionListDecoder *decoder = session_pool ? new (session_pool->Allocate(sizeof(InMemPositionListDecoder)))
                                                           InMemPositionListDecoder(pos_list_format_option_, session_pool)
                                                     : new InMemPositionListDecoder(pos_list_format_option_, session_pool);
    decoder->Init(ttf, in_mem_skiplist_reader, posting_buffer);

    return decoder;
}

} // namespace infinity
