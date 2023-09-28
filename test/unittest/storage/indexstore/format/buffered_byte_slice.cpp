#include <gtest/gtest.h>

#include "base_test.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "storage/indexstore/format/buffered_byte_slice.h"
#include "storage/indexstore/format/buffered_byte_slice_reader.h"
#include "storage/indexstore/format/doc_list_format_option.h"
#include <iostream>
#include <memory>

namespace infinity {

class BufferedByteSliceTest : public BaseTest {
public:
    BufferedByteSliceTest() {
        byte_slice_pool_ = new MemoryPool(1024);
        buffer_pool_ = new RecyclePool(1024);
    }
    ~BufferedByteSliceTest() {
        delete byte_slice_pool_;
        delete buffer_pool_;
    }
    void
    SetUp() override {
        DocListFormatOption option(NO_TERM_FREQUENCY);
        doc_list_format_.reset(new DocListFormat());
        doc_list_format_->Init(option);
        buffered_byte_slice_.reset(new BufferedByteSlice(byte_slice_pool_, buffer_pool_));
        buffered_byte_slice_->Init(doc_list_format_.get());
    }

    void
    TearDown() override {
        buffered_byte_slice_.reset();
        doc_list_format_.reset();
    }
protected:
    MemoryPool* byte_slice_pool_;
    RecyclePool* buffer_pool_;
    std::shared_ptr<BufferedByteSlice> buffered_byte_slice_;
    std::shared_ptr<DocListFormat> doc_list_format_;
};


TEST_F(BufferedByteSliceTest, test1)
{
    buffered_byte_slice_->PushBack(0, (uint32_t)1);
    buffered_byte_slice_->PushBack(1, (uint16_t)2);
    buffered_byte_slice_->EndPushBack();

    FlushInfo flush_info = buffered_byte_slice_->GetFlushInfo();
    ASSERT_TRUE(flush_info.IsValidShortBuffer());

    buffered_byte_slice_->PushBack(0, (uint32_t)2);
    buffered_byte_slice_->PushBack(1, (uint16_t)3);
    buffered_byte_slice_->EndPushBack();

    uint32_t doc_id_buffer[MAX_DOC_PER_RECORD];
    uint16_t doc_payload_buffer[MAX_DOC_PER_RECORD];

    BufferedByteSliceReader reader;
    reader.Open(buffered_byte_slice_.get());
/*
    size_t decode_len;
    reader.Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len);
    ASSERT_EQ((uint32_t)1, doc_id_buffer[0]);
    ASSERT_EQ((uint32_t)2, doc_id_buffer[1]);

    reader.Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len);
    ASSERT_EQ((uint16_t)2, doc_payload_buffer[0]);
    ASSERT_EQ((uint16_t)3, doc_payload_buffer[1]);
    */
}

TEST_F(BufferedByteSliceTest, test2)
{
    buffered_byte_slice_->PushBack(0, (uint32_t)1);
    buffered_byte_slice_->PushBack(1, (uint16_t)2);
    buffered_byte_slice_->EndPushBack();

    uint32_t flush_size = sizeof(uint32_t) + sizeof(uint8_t) * 2 + sizeof(uint16_t);
    ASSERT_EQ((size_t)flush_size, buffered_byte_slice_->Flush());

    FlushInfo flush_info = buffered_byte_slice_->GetFlushInfo();
    ASSERT_TRUE(!flush_info.IsValidShortBuffer());
    ASSERT_EQ((uint32_t)1, flush_info.GetFlushCount());
    ASSERT_EQ(flush_size, flush_info.GetFlushLength());

    ASSERT_EQ((size_t)0, buffered_byte_slice_->GetBufferSize());

    ASSERT_EQ((size_t)0, buffered_byte_slice_->Flush());
}

TEST_F(BufferedByteSliceTest, test3)
{
    const int32_t decode_len = 5;
    const int32_t count = 11;
    for (uint32_t i = 0; i < count; ++i) {
        buffered_byte_slice_->PushBack(0, i);
        buffered_byte_slice_->PushBack(1, (uint16_t)(i * 2));
        buffered_byte_slice_->EndPushBack();
        if (buffered_byte_slice_->NeedFlush(decode_len)) {
            buffered_byte_slice_->Flush();
        }
    }

    BufferedByteSlice snapshot_buffered_byte_slice(byte_slice_pool_, buffer_pool_);
    buffered_byte_slice_->SnapShot(&snapshot_buffered_byte_slice);

    ASSERT_EQ(snapshot_buffered_byte_slice.GetTotalCount(), buffered_byte_slice_->GetTotalCount());
    ASSERT_EQ(snapshot_buffered_byte_slice.GetByteSliceList(), buffered_byte_slice_->GetByteSliceList());

    BufferedByteSliceReader reader;
    reader.Open(buffered_byte_slice_.get());

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

} 
