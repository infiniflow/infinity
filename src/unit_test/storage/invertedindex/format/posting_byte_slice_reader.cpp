#include <cassert>

#include "gtest/gtest.h"
import base_test;
import stl;

import posting_byte_slice;
import posting_byte_slice_reader;
import doc_list_format_option;
import index_defines;

using namespace infinity;

class PostingByteSliceReaderTest : public BaseTest {
public:
    PostingByteSliceReaderTest() {}
    ~PostingByteSliceReaderTest() {}

    void SetUp() override {
        DocListFormatOption option(NO_TERM_FREQUENCY);
        doc_list_format_.reset(new DocListFormat(option));
        posting_byte_slice_.reset(new PostingByteSlice());
        posting_byte_slice_->Init(doc_list_format_.get());
    }
    void TearDown() override {
        posting_byte_slice_.reset();
        doc_list_format_.reset();
    }

protected:
    void CheckDecode(u32 doc_count, u32 flush_count, SharedPtr<PostingByteSliceReader> &reader) {
        Vector<u32> doc_id(doc_count);
        Vector<uint16_t> payload(doc_count);

        for (u32 i = 0; i < doc_count; ++i) {
            doc_id[i] = i;
            payload[i] = i * 2;
        }

        Vector<u32> doc_id_buffer(doc_count * 2);
        Vector<uint16_t> doc_payload_buffer(doc_count * 2);

        SizeT decode_len;
        u32 i = 0;
        for (; i < doc_count / flush_count; ++i) {
            ASSERT_TRUE(reader->Decode(doc_id_buffer.data() + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (SizeT)flush_count);
            ASSERT_TRUE(reader->Decode(doc_payload_buffer.data() + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (SizeT)flush_count);
        }

        if (doc_count % flush_count > 0) {
            ASSERT_TRUE(reader->Decode(doc_id_buffer.data() + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (SizeT)doc_count % flush_count);
            ASSERT_TRUE(reader->Decode(doc_payload_buffer.data() + i * flush_count, flush_count, decode_len));
            ASSERT_EQ(decode_len, (SizeT)doc_count % flush_count);
        }
        ASSERT_TRUE(!reader->Decode(doc_id_buffer.data() + i * flush_count, flush_count, decode_len));
        ASSERT_TRUE(!reader->Decode(doc_payload_buffer.data() + i * flush_count, flush_count, decode_len));

        for (u32 i = 0; i < doc_count; ++i) {
            ASSERT_EQ(doc_id_buffer[i], doc_id[i]);
            ASSERT_EQ(doc_payload_buffer[i], payload[i]);
        }
    }

    SharedPtr<PostingByteSliceReader> CreateReader(u32 doc_id[], uint16_t doc_payload[], u32 doc_count, u32 flush_count) {
        posting_byte_slice_.reset(new PostingByteSlice());
        posting_byte_slice_->Init(doc_list_format_.get());

        assert(posting_byte_slice_);
        for (size_t i = 0; i < doc_count; ++i) {
            posting_byte_slice_->PushBack(0, doc_id[i]);
            posting_byte_slice_->PushBack(1, doc_payload[i]);
            posting_byte_slice_->EndPushBack();
            if (posting_byte_slice_->NeedFlush(flush_count)) {
                posting_byte_slice_->Flush();
            }
        }
        SharedPtr<PostingByteSliceReader> reader(new PostingByteSliceReader);
        reader->Open(posting_byte_slice_.get());
        return reader;
    }

    void TestCheck(const u32 doc_count, u32 flush_count) {
        SharedPtr<PostingByteSliceReader> reader = CreateReader(doc_count, flush_count);
        CheckDecode(doc_count, flush_count, reader);

        reader->Open(posting_byte_slice_.get());
        CheckDecode(doc_count, flush_count, reader);
    }

    SharedPtr<PostingByteSliceReader> CreateReader(u32 doc_count, u32 flush_count) {
        Vector<u32> doc_id(doc_count);
        Vector<uint16_t> payload(doc_count);

        for (u32 i = 0; i < doc_count; ++i) {
            doc_id[i] = i;
            payload[i] = i * 2;
        }

        return CreateReader(doc_id.data(), payload.data(), doc_count, flush_count);
    }

    SharedPtr<PostingByteSlice> posting_byte_slice_;
    SharedPtr<DocListFormat> doc_list_format_;
};

TEST_F(PostingByteSliceReaderTest, test1) {
    using namespace infinity;
    DocListFormatOption option(of_none);
    DocListFormat doc_list_format(option);
    {
        // empty posting buffer
        PostingByteSlice posting_buffer;
        posting_buffer.Init(&doc_list_format);

        PostingByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        SizeT decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, MAX_DOC_PER_RECORD, decode_len));
    }
    {
        // only has short buffer
        PostingByteSlice posting_buffer;
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        PostingByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        SizeT decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
    {
        // only has flushed byte slice
        PostingByteSlice posting_buffer;
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        posting_buffer.Flush();

        PostingByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        SizeT decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
    {
        // has short buffer and flushed byte slice
        PostingByteSlice posting_buffer;
        posting_buffer.Init(&doc_list_format);

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        posting_buffer.Flush();

        posting_buffer.PushBack(0, (u32)1);
        posting_buffer.EndPushBack();

        PostingByteSliceReader reader;
        reader.Open(&posting_buffer);

        u32 doc_id_buffer[MAX_DOC_PER_RECORD];
        SizeT decode_len;
        ASSERT_TRUE(!reader.Decode(doc_id_buffer, 0, decode_len));
    }
}

TEST_F(PostingByteSliceReaderTest, test2) {
    using namespace infinity;
    u32 flush_size = 5;
    SharedPtr<PostingByteSliceReader> reader = CreateReader(33, flush_size);

    docid_t doc_id_buffer[MAX_DOC_PER_RECORD];
    docpayload_t doc_payload_buffer[MAX_DOC_PER_RECORD];

    SizeT decode_len;
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
    ASSERT_EQ((u32)3, decode_len);
    ASSERT_EQ((u32)32, doc_id_buffer[2]);
    ASSERT_TRUE(reader->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_len));
    ASSERT_EQ((u32)3, decode_len);
    ASSERT_EQ((u32)64, doc_payload_buffer[2]);
}

TEST_F(PostingByteSliceReaderTest, test3) {
    using namespace infinity;
    TestCheck(10, 5);
    TestCheck(513, 128);
}
