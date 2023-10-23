#include "posting_writer.h"

namespace infinity {

PostingWriter::PostingWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool, PostingFormatOption posting_option)
    : byte_slice_pool_(byte_slice_pool), buffer_pool_(buffer_pool), posting_option_(posting_option),
      posting_format_(new PostingFormat(posting_option)) {
    if (posting_option.HasPositionList()) {
        pos_list_encoder_ = new PositionListEncoder(posting_option_.GetPosListFormatOption(),
                                                    byte_slice_pool_,
                                                    buffer_pool_,
                                                    posting_format_->GetPositionListFormat());
    }
    if (posting_option.HasTfBitmap()) {
        doc_list_encoder_ =
            new DocListEncoder(posting_option_.GetDocListFormatOption(), byte_slice_pool_, buffer_pool_, posting_format_->GetDocListFormat());
    }
}

PostingWriter::~PostingWriter() {
    if (pos_list_encoder_) {
        delete pos_list_encoder_;
    }
    if (doc_list_encoder_) {
        delete doc_list_encoder_;
    }
}

void PostingWriter::EndDocument(docid_t doc_id, docpayload_t doc_payload) {
    doc_list_encoder_->EndDocument(doc_id, doc_payload);
    if (pos_list_encoder_) {
        pos_list_encoder_->EndDocument();
    }
}

uint32_t PostingWriter::GetDF() const { return doc_list_encoder_->GetDF(); }

uint32_t PostingWriter::GetTotalTF() const { return doc_list_encoder_->GetTotalTF(); }

tf_t PostingWriter::GetCurrentTF() const { return doc_list_encoder_->GetCurrentTF(); }

void PostingWriter::SetCurrentTF(tf_t tf) { doc_list_encoder_->SetCurrentTF(tf); }

void PostingWriter::Write(const std::shared_ptr<FileWriter> &file_writer) {
    doc_list_encoder_->Dump(file_writer);
    if (pos_list_encoder_) {
        pos_list_encoder_->Dump(file_writer);
    }
}
} // namespace infinity