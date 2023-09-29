#include <gtest/gtest.h>

#include "base_test.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "storage/indexstore/format/buffered_byte_slice.h"
#include "storage/indexstore/format/buffered_byte_slice_reader.h"
#include "storage/indexstore/format/doc_list_format_option.h"
#include <iostream>

namespace infinity {

class BufferedByteSliceReaderTest : public BaseTest {
public:
    BufferedByteSliceReaderTest() {
        byte_slice_pool_ = new MemoryPool(1024);
        buffer_pool_ = new RecyclePool(1024);
    }
    ~BufferedByteSliceReaderTest() {
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
    void
    CheckDecode(uint32_t doc_count, uint32_t flush_count, std::shared_ptr<BufferedByteSliceReader>& reader) {
        uint32_t doc_id[doc_count];
        uint16_t payload[doc_count];

        for(uint32_t i = 0; i < doc_count; ++i) {
            doc_id[i] = i;
            payload[i] = i * 2;
        }

        uint32_t doc_id_buffer[doc_count];
        uint16_t doc_payload_buffer[doc_count];

        size_t decode_len;
        uint32_t i = 0;
        for(; i < doc_count / flush_count; ++i) {
            ASSERT_TRUE(reader->Decode(doc_id_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)flush_count);
            ASSERT_TRUE(reader->Decode(doc_payload_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)flush_count);
        }

        if(doc_count % flush_count > 0) {
            ASSERT_TRUE(reader->Decode(doc_id_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)doc_count % flush_count);
            ASSERT_TRUE(reader->Decode(doc_payload_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)doc_count % flush_count);
        }
        ASSERT_TRUE(!reader->Decode(doc_id_buffer + i * flush_count, flush_count, decode_len));
        ASSERT_TRUE(!reader->Decode(doc_payload_buffer + i * flush_count, flush_count, decode_len));

        for(uint32_t i = 0; i < doc_count; ++i) {
            ASSERT_EQ(doc_id_buffer[i], doc_id[i]);
            ASSERT_EQ(doc_payload_buffer[i], payload[i]);
        }
    }

    std::shared_ptr<BufferedByteSliceReader>
    CreateReader(uint32_t doc_id[], uint16_t doc_payload[], uint32_t doc_count, uint32_t flush_count) {
        buffered_byte_slice_.reset(new BufferedByteSlice(byte_slice_pool_, buffer_pool_));
        buffered_byte_slice_->Init(doc_list_format_.get());

        assert(buffered_byte_slice_);
        for(size_t i = 0; i < doc_count; ++i) {
            buffered_byte_slice_->PushBack(0, doc_id[i]);
            buffered_byte_slice_->PushBack(1, doc_payload[i]);
            buffered_byte_slice_->EndPushBack();
            if(buffered_byte_slice_->NeedFlush(flush_count)) {
                buffered_byte_slice_->Flush();
            }
        }
        std::shared_ptr<BufferedByteSliceReader> reader(new BufferedByteSliceReader);
        reader->Open(buffered_byte_slice_.get());
        return reader;
    }

    void
    TestCheck(const uint32_t doc_count, uint32_t flush_count) {
        std::shared_ptr<BufferedByteSliceReader> reader = CreateReader(doc_count, flush_count);
        CheckDecode(doc_count, flush_count, reader);

        reader->Open(buffered_byte_slice_.get());
        CheckDecode(doc_count, flush_count, reader);
    }

    std::shared_ptr<BufferedByteSliceReader>
    CreateReader(uint32_t doc_count, uint32_t flush_count) {
        uint32_t doc_id[doc_count];
        uint16_t payload[doc_count];

        for(uint32_t i = 0; i < doc_count; ++i) {
            doc_id[i] = i;
            payload[i] = i * 2;
        }

        return CreateReader(doc_id, payload, doc_count, flush_count);
    }

    MemoryPool* byte_slice_pool_;
    RecyclePool* buffer_pool_;
    std::shared_ptr<BufferedByteSlice> buffered_byte_slice_;
    std::shared_ptr<DocListFormat> doc_list_format_;
};

TEST_F(BufferedByteSliceReaderTest, test1) {
    DocListFormatOption option(of_none);
    DocListFormat doc_list_format;
    doc_list_format.Init(option);
    {
        // empty posting buffer
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        uint32_t doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    }
    {
        // only has short buffer
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (uint32_t)1);
        posting_buffer.EndPushBack();

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        uint32_t doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
    {
        // only has flushed byte slice
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (uint32_t)1);
        posting_buffer.EndPushBack();

        posting_buffer.Flush();

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        uint32_t doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
    {
        // has short buffer and flushed byte slice
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (uint32_t)1);
        posting_buffer.EndPushBack();

        posting_buffer.Flush();

        posting_buffer.PushBack(0, (uint32_t)1);
        posting_buffer.EndPushBack();

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        uint32_t doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
}

TEST_F(BufferedByteSliceReaderTest, test2) {
    size_t flush_size = 5;
    std::shared_ptr<BufferedByteSliceReader> reader = CreateReader(33, flush_size);

    docid_t doc_id_buffer[MAX_DOC_PER_RECORD];
    docpayload_t doc_payload_buffer[MAX_DOC_PER_RECORD];

    size_t decode_len;
    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)4, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)8, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)9, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)18, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)14, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)28, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)19, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)38, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)24, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)48, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)29, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((uint32_t)58, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(3, decode_len);
    ASSERT_EQ((uint32_t)32, doc_id_buffer[2]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(3, decode_len);
    ASSERT_EQ((uint32_t)64, doc_payload_buffer[2]);
}

TEST_F(BufferedByteSliceReaderTest, test3) {
    //TestCheck(10, 5);
    //TestCheck(513, 128);
}

}// namespace infinity
