#include "unit_test/base_test.h"
#include <cassert>

import stl;
import memory_pool;
import buffered_byte_slice;
import buffered_byte_slice_reader;
import doc_list_format_option;
import index_defines;

using namespace infinity;

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

    void SetUp() override {
        DocListFormatOption option(NO_TERM_FREQUENCY);
        doc_list_format_.reset(new DocListFormat());
        doc_list_format_->Init(option);
        buffered_byte_slice_.reset(new BufferedByteSlice(byte_slice_pool_, buffer_pool_));
        buffered_byte_slice_->Init(doc_list_format_.get());
    }
    void TearDown() override {
        buffered_byte_slice_.reset();
        doc_list_format_.reset();
    }

protected:
    void CheckDecode(u32 doc_count, u32 flush_count, SharedPtr<BufferedByteSliceReader> &reader) {
        u32 doc_id[doc_count];
        uint16_t payload[doc_count];

        for (u32 i = 0; i < doc_count; ++i) {
            doc_id[i] = i;
            payload[i] = i * 2;
        }

        u32 doc_id_buffer[doc_count * 2];
        uint16_t doc_payload_buffer[doc_count * 2];

        size_t decode_len;
        u32 i = 0;
        for (; i < doc_count / flush_count; ++i) {
            ASSERT_TRUE(reader->Decode(doc_id_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)flush_count);
            ASSERT_TRUE(reader->Decode(doc_payload_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)flush_count);
        }

        if (doc_count % flush_count > 0) {
            ASSERT_TRUE(reader->Decode(doc_id_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)doc_count % flush_count);
            ASSERT_TRUE(reader->Decode(doc_payload_buffer + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (size_t)doc_count % flush_count);
        }
        ASSERT_TRUE(!reader->Decode(doc_id_buffer + i * flush_count, flush_count, decode_len));
        ASSERT_TRUE(!reader->Decode(doc_payload_buffer + i * flush_count, flush_count, decode_len));

        for (u32 i = 0; i < doc_count; ++i) {
            ASSERT_EQ(doc_id_buffer[i], doc_id[i]);
            ASSERT_EQ(doc_payload_buffer[i], payload[i]);
        }
    }

    SharedPtr<BufferedByteSliceReader> CreateReader(u32 doc_id[], uint16_t doc_payload[], u32 doc_count, u32 flush_count) {
        buffered_byte_slice_.reset(new BufferedByteSlice(byte_slice_pool_, buffer_pool_));
        buffered_byte_slice_->Init(doc_list_format_.get());

        assert(buffered_byte_slice_);
        for (size_t i = 0; i < doc_count; ++i) {
            buffered_byte_slice_->PushBack(0, doc_id[i]);
            buffered_byte_slice_->PushBack(1, doc_payload[i]);
            buffered_byte_slice_->EndPushBack();
            if (buffered_byte_slice_->NeedFlush(flush_count)) {
                buffered_byte_slice_->Flush();
            }
        }
        SharedPtr<BufferedByteSliceReader> reader(new BufferedByteSliceReader);
        reader->Open(buffered_byte_slice_.get());
        return reader;
    }

    void TestCheck(const u32 doc_count, u32 flush_count) {
        SharedPtr<BufferedByteSliceReader> reader = CreateReader(doc_count, flush_count);
        CheckDecode(doc_count, flush_count, reader);

        reader->Open(buffered_byte_slice_.get());
        CheckDecode(doc_count, flush_count, reader);
    }

    SharedPtr<BufferedByteSliceReader> CreateReader(u32 doc_count, u32 flush_count) {
        u32 doc_id[doc_count];
        uint16_t payload[doc_count];

        for (u32 i = 0; i < doc_count; ++i) {
            doc_id[i] = i;
            payload[i] = i * 2;
        }

        return CreateReader(doc_id, payload, doc_count, flush_count);
    }

    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    SharedPtr<BufferedByteSlice> buffered_byte_slice_;
    SharedPtr<DocListFormat> doc_list_format_;
};

TEST_F(BufferedByteSliceReaderTest, test1) {
    using namespace infinity;
    DocListFormatOption option(of_none);
    DocListFormat doc_list_format;
    doc_list_format.Init(option);
    {
        // empty posting buffer
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    }
    {
        // only has short buffer
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
    {
        // only has flushed byte slice
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        posting_buffer.Flush();

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
    {
        // has short buffer and flushed byte slice
        BufferedByteSlice posting_buffer(byte_slice_pool_, buffer_pool_);
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        posting_buffer.Flush();

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        BufferedByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        size_t decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
}

TEST_F(BufferedByteSliceReaderTest, test2) {
    using namespace infinity;
    size_t flush_size = 5;
    SharedPtr<BufferedByteSliceReader> reader = CreateReader(33, flush_size);

    docid_t doc_id_buffer[MAX_DOC_PER_RECORD];
    docpayload_t doc_payload_buffer[MAX_DOC_PER_RECORD];

    size_t decode_len;
    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)4, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)8, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)9, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)18, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)14, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)28, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)19, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)38, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)24, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)48, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)29, doc_id_buffer[4]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(flush_size, decode_len);
    ASSERT_EQ((u32)58, doc_payload_buffer[4]);

    ASSERT_TRUE(reader->Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(3, decode_len);
    ASSERT_EQ((u32)32, doc_id_buffer[2]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ(3, decode_len);
    ASSERT_EQ((u32)64, doc_payload_buffer[2]);
}

TEST_F(BufferedByteSliceReaderTest, test3) {
    using namespace infinity;
    TestCheck(10, 5);
    TestCheck(513, 128);
}
