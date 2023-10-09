#pragma once

#include "buffered_byte_slice.h"
#include "buffered_skiplist_writer.h"
#include "common/memory/memory_pool.h"
#include "doc_list_format_option.h"
#include "inmem_doc_list_decoder.h"
#include "position_bitmap_writer.h"
#include "storage/io/file_writer.h"

#include <memory>

namespace infinity {

class DocListEncoder {
public:
    DocListEncoder(const DocListFormatOption &format_option,
                   MemoryPool *byte_slice_pool,
                   RecyclePool *buffer_pool,
                   DocListFormat *doc_list_format = nullptr);

    ~DocListEncoder();

    uint32_t GetCurrentTF() const { return current_tf_; }

    uint32_t GetTotalTF() const { return total_tf_; }

    uint32_t GetDF() const { return df_; }

    uint32_t GetLastDocID() const { return last_doc_id_; }

    uint16_t GetLastDocPayload() const { return last_doc_payload_; }

    void SetCurrentTF(tf_t tf) {
        current_tf_ = tf;
        total_tf_ += tf;
    }

    void AddPosition();

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

    void Dump(const std::shared_ptr<FileWriter> &file);

    void Flush();

    InMemDocListDecoder *GetInMemDocListDecoder(MemoryPool *session_pool) const;

    BufferedByteSlice* 
    GetDocListBuffer() { return &doc_list_buffer_; }

private:
    void AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf);

    void FlushDocListBuffer();

    void CreateDocSkipListWriter();

    void AddSkipListItem(uint32_t item_size);

private:
    BufferedByteSlice doc_list_buffer_;
    bool own_doc_list_format_;
    DocListFormatOption format_option_;
    DocListFormat *doc_list_format_;

    docid_t last_doc_id_;
    docpayload_t last_doc_payload_;
    tf_t current_tf_;
    tf_t total_tf_;
    df_t df_;

    PositionBitmapWriter* tf_bitmap_writer_;
    BufferedSkipListWriter* doc_skiplist_writer_;
    MemoryPool* byte_slice_pool_;
    friend class InMemDocListDecoderTest;
};

} // namespace infinity