module;
#include <cassert>

module infinity_core:doc_list_encoder.impl;

import :doc_list_encoder;
import :stl;

import :file_writer;
import :file_reader;
import :posting_byte_slice;
import :skiplist_writer;
import :skiplist_reader;
import :inmem_doc_list_decoder;
import :index_defines;
import :vbyte_compressor;
import :logger;
import :doc_list_format_option;

namespace infinity {

DocListEncoder::DocListEncoder(const DocListFormat *doc_list_format)
    : doc_list_buffer_(), doc_list_format_(doc_list_format), last_doc_id_(0), current_tf_(0), total_tf_(0), df_(0), doc_skiplist_writer_(nullptr) {
    assert(doc_list_format != nullptr);
    doc_list_buffer_.Init(doc_list_format_);
    doc_skiplist_writer_ = MakeShared<SkipListWriter>();
    doc_skiplist_writer_->Init(doc_list_format_->GetDocSkipListFormat());
}

DocListEncoder::~DocListEncoder() {}

void DocListEncoder::AddPosition() {
    current_tf_++;
    total_tf_++;
}

void DocListEncoder::EndDocument(docid_t doc_id, u32 doc_len, docpayload_t doc_payload) {
    AddDocument(doc_id, doc_payload, current_tf_, doc_len);
    {
        std::unique_lock<std::shared_mutex> lock(rw_mutex_);
        df_ += 1;
    }
    current_tf_ = 0;
}

void DocListEncoder::Flush() {
    FlushDocListBuffer();
    SharedPtr<SkipListWriter> doc_skiplist_writer;
    {
        std::shared_lock<std::shared_mutex> lock(rw_mutex_);
        doc_skiplist_writer = doc_skiplist_writer_;
    }
    if (doc_skiplist_writer.get()) {
        doc_skiplist_writer->Flush();
    }
}

void DocListEncoder::AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf, u32 doc_len) {
    doc_list_buffer_.PushBack(0, doc_id - last_doc_id_);
    int n = 1;
    if (doc_list_format_->GetOption().HasTfList()) {
        doc_list_buffer_.PushBack(n++, tf);
    }
    if (doc_list_format_->GetOption().HasDocPayload()) {
        doc_list_buffer_.PushBack(n++, doc_payload);
    }
    doc_list_buffer_.EndPushBack();
    last_doc_id_ = doc_id;
    last_doc_payload_ = doc_payload;
    block_max_tf_ = std::max(block_max_tf_, tf);
    assert((tf > 0 and tf <= doc_len));
    block_max_percentage_ = std::max(block_max_percentage_, static_cast<float>(tf) / doc_len);
    if (doc_list_buffer_.NeedFlush()) {
        FlushDocListBuffer();
    }
}

void DocListEncoder::Dump(const SharedPtr<FileWriter> &file, bool spill) {
    df_t df;
    SharedPtr<SkipListWriter> doc_skiplist_writer;
    {
        std::shared_lock<std::shared_mutex> lock(rw_mutex_);
        df = df_;
        doc_skiplist_writer = doc_skiplist_writer_;
    }
    if (spill) {
        file->WriteVInt(last_doc_id_);
        file->WriteVInt(last_doc_payload_);
        file->WriteVInt(current_tf_);
        file->WriteVInt(total_tf_);
        file->WriteVInt(df);
        file->WriteVInt(block_max_tf_);
        assert((sizeof(i32) == sizeof(float)));
        i32 block_max_percentage = std::bit_cast<i32>(block_max_percentage_);
        file->WriteInt(block_max_percentage);
    } else {
        Flush();
        u32 doc_skiplist_size = 0;
        if (doc_skiplist_writer.get()) {
            doc_skiplist_size = doc_skiplist_writer->EstimateDumpSize();
        }

        u32 doc_list_size = doc_list_buffer_.EstimateDumpSize();

        file->WriteVInt(doc_skiplist_size);
        file->WriteVInt(doc_list_size);
    }

    if (doc_skiplist_writer.get()) {
        doc_skiplist_writer->Dump(file, spill);
    }

    doc_list_buffer_.Dump(file, spill);
}

void DocListEncoder::Load(const SharedPtr<FileReader> &file) {
    last_doc_id_ = file->ReadVInt();
    last_doc_payload_ = file->ReadVInt();
    current_tf_ = file->ReadVInt();
    total_tf_ = file->ReadVInt();
    df_ = file->ReadVInt();
    block_max_tf_ = file->ReadVInt();
    i32 block_max_percentage = file->ReadInt();
    block_max_percentage_ = std::bit_cast<float>(block_max_percentage);

    doc_skiplist_writer_->Load(file);
    doc_list_buffer_.Load(file);
}

u32 DocListEncoder::GetDumpLength() {
    SharedPtr<SkipListWriter> doc_skiplist_writer;
    {
        std::shared_lock<std::shared_mutex> lock(rw_mutex_);
        doc_skiplist_writer = doc_skiplist_writer_;
    }
    u32 doc_skiplist_size = 0;
    if (doc_skiplist_writer.get()) {
        doc_skiplist_size = doc_skiplist_writer->EstimateDumpSize();
    }
    u32 doc_list_size = doc_list_buffer_.EstimateDumpSize();
    return VByteCompressor::GetVInt32Length(doc_skiplist_size) + VByteCompressor::GetVInt32Length(doc_list_size) + doc_skiplist_size + doc_list_size;
}

void DocListEncoder::FlushDocListBuffer() {
    u32 flush_size = doc_list_buffer_.Flush();
    if (flush_size > 0) {
        AddSkipListItem(flush_size);
    }
    block_max_tf_ = 0;
    block_max_percentage_ = 0.0f;
}

SharedPtr<SkipListWriter> DocListEncoder::GetDocSkipListWriter() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    if (!doc_skiplist_writer_.get()) {
        doc_skiplist_writer_ = MakeShared<SkipListWriter>();
        doc_skiplist_writer_->Init(doc_list_format_->GetDocSkipListFormat());
    }
    return doc_skiplist_writer_;
}

void DocListEncoder::AddSkipListItem(u32 item_size) {
    SharedPtr<SkipListWriter> doc_skiplist_writer = GetDocSkipListWriter();
    const DocSkipListFormat *skiplist_format = doc_list_format_->GetDocSkipListFormat();
    if (skiplist_format->HasBlockMax()) {
        assert((block_max_percentage_ > 0 and block_max_percentage_ <= 1.0f));
        u32 max_percentage_field = static_cast<u32>(std::ceil(block_max_percentage_ * std::numeric_limits<u16>::max()));
        assert((max_percentage_field <= std::numeric_limits<u16>::max()));
        doc_skiplist_writer->AddItem(last_doc_id_, total_tf_, block_max_tf_, static_cast<u16>(max_percentage_field), item_size);
    } else if (skiplist_format->HasTfList()) {
        doc_skiplist_writer->AddItem(last_doc_id_, total_tf_, item_size);
    } else {
        doc_skiplist_writer->AddItem(last_doc_id_, item_size);
    }
}

InMemDocListDecoder *DocListEncoder::GetInMemDocListDecoder() const {
    df_t df;
    SharedPtr<SkipListWriter> doc_skiplist_writer;
    {
        std::shared_lock<std::shared_mutex> lock(rw_mutex_);
        df = df_;
        doc_skiplist_writer = doc_skiplist_writer_;
    }
    SkipListReaderPostingByteSlice *skiplist_reader = nullptr;
    if (doc_skiplist_writer) {
        skiplist_reader = new SkipListReaderPostingByteSlice(doc_list_format_->GetOption());
        skiplist_reader->Load(doc_skiplist_writer.get());
    }

    PostingByteSlice *doc_list_buffer = new PostingByteSlice();
    doc_list_buffer_.SnapShot(doc_list_buffer);

    InMemDocListDecoder *decoder = new InMemDocListDecoder(doc_list_format_->GetOption());
    decoder->Init(df, skiplist_reader, doc_list_buffer);

    return decoder;
}
} // namespace infinity