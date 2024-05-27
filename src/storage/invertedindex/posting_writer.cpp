module;

import stl;

import file_writer;
import file_reader;
import doc_list_encoder;
import inmem_posting_decoder;
import inmem_position_list_decoder;
import inmem_doc_list_decoder;
import position_list_encoder;
import posting_list_format;
import index_defines;
import term_meta;
import vector_with_lock;

module posting_writer;

namespace infinity {

PostingWriter::PostingWriter(PostingFormatOption posting_option, VectorWithLock<u32> &column_lengths)
    : posting_option_(posting_option), posting_format_(new PostingFormat(posting_option)), column_lengths_(column_lengths) {
    if (posting_option.HasPositionList()) {
        position_list_encoder_ = new PositionListEncoder(posting_option_, posting_format_->GetPositionListFormat());
    }
    doc_list_encoder_ = new DocListEncoder(posting_option_.GetDocListFormatOption(), posting_format_->GetDocListFormat());
}

PostingWriter::~PostingWriter() {
    if (posting_format_) {
        delete posting_format_;
    }
    if (position_list_encoder_) {
        delete position_list_encoder_;
    }
    if (doc_list_encoder_) {
        delete doc_list_encoder_;
    }
}

void PostingWriter::EndDocument(docid_t doc_id, docpayload_t doc_payload) {
    u32 doc_len = GetDocColumnLength(doc_id);
    doc_list_encoder_->EndDocument(doc_id, doc_len, doc_payload);
    if (position_list_encoder_) {
        position_list_encoder_->EndDocument();
    }
}

u32 PostingWriter::GetDF() const { return doc_list_encoder_->GetDF(); }

u32 PostingWriter::GetTotalTF() const { return doc_list_encoder_->GetTotalTF(); }

tf_t PostingWriter::GetCurrentTF() const { return doc_list_encoder_->GetCurrentTF(); }

void PostingWriter::SetCurrentTF(tf_t tf) { doc_list_encoder_->SetCurrentTF(tf); }

void PostingWriter::Dump(const SharedPtr<FileWriter> &file_writer, TermMeta &term_meta, bool spill) {
    term_meta.doc_freq_ = GetDF();
    term_meta.total_tf_ = GetTotalTF();
    term_meta.payload_ = 0;
    term_meta.doc_start_ = file_writer->TotalWrittenBytes();
    doc_list_encoder_->Dump(file_writer, spill);
    if (position_list_encoder_) {
        term_meta.pos_start_ = file_writer->TotalWrittenBytes();
        position_list_encoder_->Dump(file_writer, spill);
        term_meta.pos_end_ = file_writer->TotalWrittenBytes();
    }
}

void PostingWriter::Load(const SharedPtr<FileReader> &file_reader) {
    doc_list_encoder_->Load(file_reader);
    if (position_list_encoder_) {
        position_list_encoder_->Load(file_reader);
    }
}

u32 PostingWriter::GetDumpLength() {
    u32 length = doc_list_encoder_->GetDumpLength();
    if (position_list_encoder_) {
        length += position_list_encoder_->GetDumpLength();
    }
    return length;
}

void PostingWriter::EndSegment() {
    doc_list_encoder_->Flush();
    if (position_list_encoder_) {
        position_list_encoder_->Flush();
    }
}

void PostingWriter::AddPosition(pos_t pos) {
    doc_list_encoder_->AddPosition();
    if (position_list_encoder_) {
        position_list_encoder_->AddPosition(pos);
    }
}

InMemPostingDecoder *PostingWriter::CreateInMemPostingDecoder() const {
    InMemPostingDecoder *posting_decoder = new InMemPostingDecoder();

    InMemDocListDecoder *doc_list_decoder = doc_list_encoder_->GetInMemDocListDecoder();
    posting_decoder->SetDocListDecoder(doc_list_decoder);

    if (position_list_encoder_ != nullptr) {
        InMemPositionListDecoder *position_list_decoder = position_list_encoder_->GetInMemPositionListDecoder();
        posting_decoder->SetPositionListDecoder(position_list_decoder);
    }

    return posting_decoder;
}
} // namespace infinity