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
import posting_byte_slice;
import skiplist_writer;
import position_list_format_option;
import posting_list_format;
import inmem_position_list_decoder;
import skiplist_reader;
import short_list_optimize_util;

namespace infinity {
PositionListEncoder::PositionListEncoder(const PostingFormatOption &format_option,
                                         MemoryPool *byte_slice_pool,
                                         MemoryPool *buffer_pool,
                                         const PositionListFormat *pos_list_format)
    : pos_list_buffer_(byte_slice_pool, buffer_pool), last_pos_in_cur_doc_(0), total_pos_count_(0), format_option_(format_option),
      is_own_format_(false), pos_list_format_(pos_list_format), byte_slice_pool_(byte_slice_pool) {
    if (!pos_list_format) {
        pos_list_format_ = new PositionListFormat(format_option_.GetPosListFormatOption());
        is_own_format_ = true;
    }
    pos_list_buffer_.Init(pos_list_format_);
    CreatePosSkipListWriter();
}

PositionListEncoder::~PositionListEncoder() {
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
    if (pos_skiplist_writer_.get()) {
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
        if (pos_skiplist_writer_.get()) {
            pos_skiplist_size = pos_skiplist_writer_->EstimateDumpSize();
        }

        file->WriteVInt(pos_skiplist_size);
        file->WriteVInt(pos_list_size);
    }
    if (pos_skiplist_writer_.get()) {
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
    if (pos_skiplist_writer_.get()) {
        pos_skiplist_size = pos_skiplist_writer_->EstimateDumpSize();
    }

    u32 pos_list_size = pos_list_buffer_.EstimateDumpSize();
    return VByteCompressor::GetVInt32Length(pos_skiplist_size) + VByteCompressor::GetVInt32Length(pos_list_size) + pos_skiplist_size + pos_list_size;
}

void PositionListEncoder::CreatePosSkipListWriter() {
    MemoryPool *bufferPool = dynamic_cast<MemoryPool *>(pos_list_buffer_.GetBufferPool());
    pos_skiplist_writer_ = MakeUnique<SkipListWriter>(byte_slice_pool_, bufferPool);
    pos_skiplist_writer_->Init(pos_list_format_->GetPositionSkipListFormat());
}

void PositionListEncoder::AddPosSkipListItem(u32 total_pos_count, u32 compressed_pos_size, bool need_flush) {
    pos_skiplist_writer_->AddItem(total_pos_count, compressed_pos_size);
}

void PositionListEncoder::FlushPositionBuffer() {
    // TODO: uncompress need this
    bool need_flush = pos_list_buffer_.NeedFlush(MAX_POS_PER_RECORD);

    u32 flush_size = pos_list_buffer_.Flush();
    if (flush_size > 0) {
        if (!pos_skiplist_writer_.get()) {
            CreatePosSkipListWriter();
        }
        AddPosSkipListItem(total_pos_count_, flush_size, need_flush);
    }
}

InMemPositionListDecoder *PositionListEncoder::GetInMemPositionListDecoder(MemoryPool *session_pool) const {
    // doclist -> ttf -> pos skiplist -> poslist
    ttf_t ttf = total_pos_count_;

    SkipListReaderPostingByteSlice *in_mem_skiplist_reader = nullptr;
    if (pos_skiplist_writer_.get()) {
        // not support tf bitmap in realtime segment
        in_mem_skiplist_reader = session_pool ? new (session_pool->Allocate(sizeof(SkipListReaderPostingByteSlice)))
                                                    SkipListReaderPostingByteSlice(format_option_.GetDocListFormatOption(), session_pool)
                                              : new SkipListReaderPostingByteSlice(format_option_.GetDocListFormatOption(), session_pool);
        in_mem_skiplist_reader->Load(pos_skiplist_writer_.get());
    }
    PostingByteSlice *posting_buffer = new (session_pool->Allocate(sizeof(PostingByteSlice))) PostingByteSlice(session_pool, session_pool);
    pos_list_buffer_.SnapShot(posting_buffer);

    InMemPositionListDecoder *decoder = session_pool ? new (session_pool->Allocate(sizeof(InMemPositionListDecoder)))
                                                           InMemPositionListDecoder(format_option_, session_pool)
                                                     : new InMemPositionListDecoder(format_option_, session_pool);
    decoder->Init(ttf, in_mem_skiplist_reader, sizeof(SkipListReaderPostingByteSlice), posting_buffer);

    return decoder;
}

} // namespace infinity
