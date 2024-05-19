#include "unit_test/base_test.h"

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import doc_list_format_option;
import index_defines;
import flush_info;

using namespace infinity;

class PostingByteSliceTest : public BaseTest {
public:
    PostingByteSliceTest() {
        byte_slice_pool_ = new MemoryPool(1024);
        buffer_pool_ = new RecyclePool(1024);
    }
    ~PostingByteSliceTest() {
        delete byte_slice_pool_;
        delete buffer_pool_;
    }
    void SetUp() override {
        DocListFormatOption option(NO_TERM_FREQUENCY);
        doc_list_format_.reset(new DocListFormat());
        doc_list_format_->Init(option);
        posting_byte_slice_.reset(new PostingByteSlice(byte_slice_pool_, buffer_pool_));
        posting_byte_slice_->Init(doc_list_format_.get());
    }

    void TearDown() override {
        posting_byte_slice_.reset();
        doc_list_format_.reset();
    }

protected:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    SharedPtr<PostingByteSlice> posting_byte_slice_;
    SharedPtr<DocListFormat> doc_list_format_;
};

TEST_F(PostingByteSliceTest, test1) {
    using namespace infinity;

    posting_byte_slice_->PushBack(0, (uint32_t)1);
    posting_byte_slice_->PushBack(1, (uint16_t)2);
    posting_byte_slice_->EndPushBack();

    FlushInfo flush_info = posting_byte_slice_->GetFlushInfo();
    ASSERT_TRUE(flush_info.IsValidPostingBuffer());

    posting_byte_slice_->PushBack(0, (uint32_t)2);
    posting_byte_slice_->PushBack(1, (uint16_t)3);
    posting_byte_slice_->EndPushBack();

    uint32_t doc_id_buffer[MAX_DOC_PER_RECORD];
    uint16_t doc_payload_buffer[MAX_DOC_PER_RECORD];

    PostingByteSliceReader reader;
    reader.Open(posting_byte_slice_.get());

    size_t decode_len;
    reader.Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len);
    ASSERT_EQ((uint32_t)1, doc_id_buffer[0]);
    ASSERT_EQ((uint32_t)2, doc_id_buffer[1]);

    reader.Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len);
    ASSERT_EQ((uint16_t)2, doc_payload_buffer[0]);
    ASSERT_EQ((uint16_t)3, doc_payload_buffer[1]);
}

TEST_F(PostingByteSliceTest, test2) {
    using namespace infinity;

    posting_byte_slice_->PushBack(0, (uint32_t)1);
    posting_byte_slice_->PushBack(1, (uint16_t)2);
    posting_byte_slice_->EndPushBack();

    posting_byte_slice_->Flush();

    FlushInfo flush_info = posting_byte_slice_->GetFlushInfo();
    ASSERT_TRUE(!flush_info.IsValidPostingBuffer());
    ASSERT_EQ((uint32_t)1, flush_info.GetFlushCount());
    ASSERT_EQ((size_t)0, posting_byte_slice_->GetBufferSize());
    ASSERT_EQ((size_t)0, posting_byte_slice_->Flush());
}

TEST_F(PostingByteSliceTest, test3) {
    using namespace infinity;

    const int32_t decode_len = 5;
    const int32_t count = 11;
    for (uint32_t i = 0; i < count; ++i) {
        posting_byte_slice_->PushBack(0, i);
        posting_byte_slice_->PushBack(1, (uint16_t)(i * 2));
        posting_byte_slice_->EndPushBack();

        if (posting_byte_slice_->NeedFlush(decode_len)) {
            posting_byte_slice_->Flush();
        }
    }

    PostingByteSlice snapshot_posting_byte_slice(byte_slice_pool_, buffer_pool_);
    posting_byte_slice_->SnapShot(&snapshot_posting_byte_slice);

    ASSERT_EQ(snapshot_posting_byte_slice.GetTotalCount(), posting_byte_slice_->GetTotalCount());
    ASSERT_EQ(snapshot_posting_byte_slice.GetByteSliceList(), posting_byte_slice_->GetByteSliceList());

    PostingByteSliceReader reader;
    reader.Open(posting_byte_slice_.get());

    uint32_t buffer[count * 2];
    uint16_t doc_payload_buffer[count * 2];
    size_t actual_decode_len;
    reader.Decode(buffer, decode_len, actual_decode_len);
    reader.Decode(doc_payload_buffer, decode_len, actual_decode_len);
    reader.Decode(buffer + decode_len, decode_len, actual_decode_len);
    reader.Decode(doc_payload_buffer + decode_len, decode_len, actual_decode_len);
    reader.Decode(buffer + decode_len * 2, decode_len, actual_decode_len);
    reader.Decode(doc_payload_buffer + decode_len * 2, decode_len, actual_decode_len);

    for (uint32_t i = 0; i < count; ++i) {
        ASSERT_EQ(i, buffer[i]);
        ASSERT_EQ(i * 2, doc_payload_buffer[i]);
    }
}
