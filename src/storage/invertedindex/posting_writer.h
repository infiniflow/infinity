#pragma once

#include "common/memory/memory_pool.h"
#include "format/doc_list_encoder.h"
#include "format/inmem_posting_decoder.h"
#include "format/pos_list_encoder.h"
#include "format/posting_list_format.h"
#include "storage/io/file_writer.h"

namespace infinity {
class PostingWriter {
public:
    PostingWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool, PostingFormatOption posting_option);

    ~PostingWriter();

    void AddPosition(pos_t pos);

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

    uint32_t GetTotalTF() const;

    uint32_t GetDF() const;

    docpayload_t GetLastDocPayload() const { return 0; };

    void SetCurrentTF(tf_t tf);

    tf_t GetCurrentTF() const;

    void Write(const std::shared_ptr<FileWriter> &file_writer);

    InMemPostingDecoder *CreateInMemPostingDecoder(MemoryPool *session_pool) const;

private:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    PostingFormatOption posting_option_;
    PostingFormat *posting_format_;
    DocListEncoder *doc_list_encoder_ = nullptr;
    PositionListEncoder *position_list_encoder_ = nullptr;
};
} // namespace infinity