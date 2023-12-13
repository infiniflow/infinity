module;

import stl;
import memory_pool;
import file_writer;
import buffered_byte_slice;
import buffered_skiplist_writer;
import doc_list_format_option;
import inmem_doc_list_decoder;
import position_bitmap_writer;
import inmem_pair_value_skiplist_reader;
import inmem_tri_value_skiplist_reader;
import index_defines;
import skiplist_reader;
module doc_list_encoder;

namespace infinity {

DocListEncoder::DocListEncoder(const DocListFormatOption &format_option,
                               MemoryPool *byte_slice_pool,
                               RecyclePool *buffer_pool,
                               DocListFormat *doc_list_format)
    : doc_list_buffer_(byte_slice_pool, buffer_pool), own_doc_list_format_(false), format_option_(format_option), doc_list_format_(doc_list_format),
      last_doc_id_(0), current_tf_(0), total_tf_(0), df_(0), tf_bitmap_writer_(nullptr), doc_skiplist_writer_(nullptr),
      byte_slice_pool_(byte_slice_pool) {
    if (!doc_list_format) {
        doc_list_format_ = new DocListFormat;
        doc_list_format_->Init(format_option);
        own_doc_list_format_ = true;
    }
    doc_list_buffer_.Init(doc_list_format_);
    if (format_option_.HasTfBitmap()) {
        tf_bitmap_writer_ = new PositionBitmapWriter(byte_slice_pool_);
    }
}

DocListEncoder::~DocListEncoder() {
    if (own_doc_list_format_) {
        delete doc_list_format_;
        doc_list_format_ = nullptr;
    }
    if (tf_bitmap_writer_) {
        delete tf_bitmap_writer_;
        tf_bitmap_writer_ = nullptr;
    }
    if (doc_skiplist_writer_) {
        doc_skiplist_writer_->~BufferedSkipListWriter();
        doc_skiplist_writer_ = nullptr;
    }
}

void DocListEncoder::AddPosition() {
    current_tf_++;
    total_tf_++;
}

void DocListEncoder::EndDocument(docid_t doc_id, docpayload_t doc_payload) {
    AddDocument(doc_id, doc_payload, current_tf_);
    df_ += 1;
    current_tf_ = 0;
    if (tf_bitmap_writer_) {
        // set to remember the first occ in this doc
        tf_bitmap_writer_->Set(total_tf_ - current_tf_);
        tf_bitmap_writer_->EndDocument(df_, total_tf_);
    }
}

void DocListEncoder::Flush() {
    FlushDocListBuffer();
    if (doc_skiplist_writer_) {
        doc_skiplist_writer_->Flush();
    }
    if (tf_bitmap_writer_) {
        tf_bitmap_writer_->Resize(total_tf_);
    }
}

void DocListEncoder::AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf) {
    doc_list_buffer_.PushBack(0, doc_id - last_doc_id_);
    int n = 1;
    if (format_option_.HasTfList()) {
        doc_list_buffer_.PushBack(n++, tf);
    }
    if (format_option_.HasDocPayload()) {
        doc_list_buffer_.PushBack(n++, doc_payload);
    }
    doc_list_buffer_.EndPushBack();
    last_doc_id_ = doc_id;
    last_doc_payload_ = doc_payload;
    if (doc_list_buffer_.NeedFlush()) {
        FlushDocListBuffer();
    }
}

void DocListEncoder::Dump(const SharedPtr<FileWriter> &file) {
    Flush();
    u32 doc_skiplist_size = 0;
    if (doc_skiplist_writer_) {
        doc_skiplist_size = doc_skiplist_writer_->EstimateDumpSize();
    }

    u32 doc_list_size = doc_list_buffer_.EstimateDumpSize();

    file->WriteVInt(doc_skiplist_size);
    file->WriteVInt(doc_list_size);

    if (doc_skiplist_writer_) {
        doc_skiplist_writer_->Dump(file);
    }

    doc_list_buffer_.Dump(file);
    if (tf_bitmap_writer_) {
        tf_bitmap_writer_->Dump(file, total_tf_);
    }
}

void DocListEncoder::FlushDocListBuffer() {
    u32 flush_size = doc_list_buffer_.Flush();
    if (flush_size > 0) {
        if (doc_skiplist_writer_ == nullptr) {
            CreateDocSkipListWriter();
        }
        AddSkipListItem(flush_size);
    }
}

void DocListEncoder::CreateDocSkipListWriter() {
    void *buffer = byte_slice_pool_->Allocate(sizeof(BufferedSkipListWriter));
    RecyclePool *buffer_pool = dynamic_cast<RecyclePool *>(doc_list_buffer_.GetBufferPool());
    BufferedSkipListWriter *doc_skiplist_writer = new (buffer) BufferedSkipListWriter(byte_slice_pool_, buffer_pool);
    doc_skiplist_writer->Init(doc_list_format_->GetDocSkipListFormat());
    doc_skiplist_writer_ = doc_skiplist_writer;
}

void DocListEncoder::AddSkipListItem(u32 item_size) {
    const DocSkipListFormat *skiplist_format = doc_list_format_->GetDocSkipListFormat();

    if (skiplist_format->HasTfList()) {
        doc_skiplist_writer_->AddItem(last_doc_id_, total_tf_, item_size);
    } else {
        doc_skiplist_writer_->AddItem(last_doc_id_, item_size);
    }
}

InMemDocListDecoder *DocListEncoder::GetInMemDocListDecoder(MemoryPool *session_pool) const {
    df_t df = df_;
    // TODO memory problem
    SkipListReader *skiplist_reader = nullptr;
    if (doc_skiplist_writer_) {
        const DocSkipListFormat *skiplist_format = doc_list_format_->GetDocSkipListFormat();

        if (skiplist_format->HasTfList()) {
            InMemTriValueSkipListReader *in_mem_skiplist_reader = session_pool ? new (session_pool->Allocate(sizeof(InMemTriValueSkipListReader)))
                                                                                     InMemTriValueSkipListReader(session_pool)
                                                                               : new InMemTriValueSkipListReader(session_pool);
            in_mem_skiplist_reader->Load(doc_skiplist_writer_);
            skiplist_reader = in_mem_skiplist_reader;
        } else {
            InMemPairValueSkipListReader *in_mem_skiplist_reader = session_pool ? new (session_pool->Allocate(sizeof(InMemPairValueSkipListReader)))
                                                                                      InMemPairValueSkipListReader(session_pool)
                                                                                : new InMemPairValueSkipListReader(session_pool);
            in_mem_skiplist_reader->Load(doc_skiplist_writer_);
            skiplist_reader = in_mem_skiplist_reader;
        }
    }

    BufferedByteSlice *doc_list_buffer = session_pool ? new (session_pool->Allocate(sizeof(BufferedByteSlice)))
                                                            BufferedByteSlice(session_pool, session_pool)
                                                      : new BufferedByteSlice(session_pool, session_pool);
    doc_list_buffer_.SnapShot(doc_list_buffer);

    InMemDocListDecoder *decoder = session_pool ? new (session_pool->Allocate(sizeof(InMemDocListDecoder))) InMemDocListDecoder(session_pool)
                                                : new InMemDocListDecoder(session_pool);
    decoder->Init(df, skiplist_reader, doc_list_buffer);

    return decoder;
}
} // namespace infinity