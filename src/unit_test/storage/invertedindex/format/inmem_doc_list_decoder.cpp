
#include "unit_test/base_test.h"

import stl;
import memory_pool;
import index_defines;
import posting_byte_slice;
import inmem_doc_list_decoder;
import doc_list_format_option;
import doc_list_encoder;

using namespace infinity;
constexpr u32 default_document_legnth = 10;

class InMemDocListDecoderTest : public BaseTest {
public:
    InMemDocListDecoderTest() {
        doc_list_encoder_ = nullptr;
        doc_list_decoder_ = nullptr;
    }

    void SetUp() override {
        buffer_pool_ = new RecyclePool(10240);
        byte_slice_pool_ = new MemoryPool(10240);

        DocListFormatOption doc_list_format_option(OPTION_FLAG_NONE);
        doc_list_encoder_ = new DocListEncoder(doc_list_format_option, byte_slice_pool_, buffer_pool_);

        for (SizeT i = 0; i < 128; ++i) {
            doc_list_encoder_->AddPosition();
            doc_list_encoder_->EndDocument(i, default_document_legnth, 0);
        }
        for (SizeT i = 128; i < 256; ++i) {
            doc_list_encoder_->AddPosition();
            doc_list_encoder_->EndDocument(i * 2, default_document_legnth, 0);
        }
        doc_list_encoder_->AddPosition();
        doc_list_encoder_->EndDocument(1000, default_document_legnth, 0);
    }
    void TearDown() override {
        doc_list_encoder_ptr_.reset();
        doc_list_format_ptr_.reset();

        delete doc_list_encoder_;
        delete buffer_pool_;
        delete byte_slice_pool_;
    }

protected:
    void TestDecodeWithOptionFlag(const optionflag_t flag, SizeT doc_num, docid_t *docids, tf_t *tfs, docpayload_t *doc_payloads) {
        DocListFormatOption doc_list_format_option(flag);
        DocListFormat doc_list_format(doc_list_format_option);
        PostingByteSlice *posting_buffer =
            new (byte_slice_pool_->Allocate(sizeof(PostingByteSlice))) PostingByteSlice(byte_slice_pool_, buffer_pool_);
        posting_buffer->Init(&doc_list_format);

        docid_t prev_doc_id = 0;
        for (SizeT i = 0; i < doc_num; ++i) {
            uint8_t row = 0;
            posting_buffer->PushBack(row++, docids[i] - prev_doc_id);
            prev_doc_id = docids[i];
            if (doc_list_format_option.HasTfList()) {
                posting_buffer->PushBack(row++, tfs[i]);
            }
            if (doc_list_format_option.HasDocPayload()) {
                posting_buffer->PushBack(row++, doc_payloads[i]);
            }
            posting_buffer->EndPushBack();
        }

        InMemDocListDecoder doc_list_decoder(byte_slice_pool_, doc_list_format_option);
        doc_list_decoder.Init(2, nullptr, posting_buffer);

        docid_t prev_last_doc_id = 0;
        docid_t last_doc_id = 0;
        ttf_t current_ttf = 0;
        docid_t doc_buffer[1024];
        ASSERT_TRUE(doc_list_decoder.DecodeSkipList(0, prev_last_doc_id, last_doc_id, current_ttf));
        ASSERT_TRUE(doc_list_decoder.DecodeCurrentDocIDBuffer(doc_buffer));
        docid_t first_doc_id = prev_last_doc_id + doc_buffer[0];

        ASSERT_EQ(docids[0], first_doc_id);
        ASSERT_EQ(docids[doc_num - 1], last_doc_id);
        ASSERT_EQ((ttf_t)0, current_ttf);
        ASSERT_EQ(docids[0], doc_buffer[0]);
        for (SizeT i = 1; i < doc_num; ++i) {
            ASSERT_EQ(docids[i] - docids[i - 1], doc_buffer[i]);
        }

        if (doc_list_format_option.HasTfList()) {
            tf_t tf_buffer[1024];
            doc_list_decoder.DecodeCurrentTFBuffer(tf_buffer);
            for (SizeT i = 0; i < doc_num; ++i) {
                ASSERT_EQ(tfs[i], tf_buffer[i]);
            }
        }
        if (doc_list_format_option.HasDocPayload()) {
            docpayload_t docPayloadBuffer[1024];
            doc_list_decoder.DecodeCurrentDocPayloadBuffer(docPayloadBuffer);
            for (SizeT i = 0; i < doc_num; ++i) {
                ASSERT_EQ(doc_payloads[i], docPayloadBuffer[i]);
            }
        }
    }

    InMemDocListDecoder *CreateDecoder(uint32_t doc_count, bool need_flush, bool need_tf) {
        optionflag_t op_flag = NO_TERM_FREQUENCY;
        if (need_tf) {
            op_flag |= of_term_frequency;
        }

        DocListFormatOption format_option(op_flag);
        doc_list_format_ptr_.reset(new DocListFormat(format_option));

        doc_list_encoder_ptr_.reset(new DocListEncoder(format_option, byte_slice_pool_, buffer_pool_, doc_list_format_ptr_.get()));
        for (uint32_t i = 0; i < doc_count; ++i) {
            doc_list_encoder_ptr_->AddPosition();
            doc_list_encoder_ptr_->AddPosition();
            // tf = 2
            doc_list_encoder_ptr_->EndDocument(i, default_document_legnth, i * 2);
        }

        if (need_flush) {
            doc_list_encoder_ptr_->Flush();
        }
        InMemDocListDecoder *decoder = doc_list_encoder_ptr_->GetInMemDocListDecoder(byte_slice_pool_);
        return decoder;
    }

    void TestDecode(const uint32_t doc_count, bool need_flush = false, bool need_tf = false) {
        InMemDocListDecoder *decoder = CreateDecoder(doc_count, need_flush, need_tf);

        Vector<docid_t> doc_buffer(doc_count);
        Vector<docpayload_t> doc_payload(doc_count);
        Vector<tf_t> tf_buffer(doc_count);

        docid_t prev_last_doc_id = 0;
        docid_t first_doc_id = 0;
        docid_t last_doc_id = 0;
        ttf_t ttf;

        uint32_t i = 0;
        for (; i < doc_count / MAX_DOC_PER_RECORD; ++i) {
            ASSERT_TRUE(decoder->DecodeSkipList(i * MAX_DOC_PER_RECORD, prev_last_doc_id, last_doc_id, ttf));
            ASSERT_TRUE(decoder->DecodeCurrentDocIDBuffer(doc_buffer.data() + i * MAX_DOC_PER_RECORD));
            first_doc_id = prev_last_doc_id + doc_buffer[i * MAX_DOC_PER_RECORD];
            ASSERT_EQ((docid_t)(i * MAX_DOC_PER_RECORD), first_doc_id);
            ASSERT_EQ((docid_t)((i + 1) * MAX_DOC_PER_RECORD - 1), last_doc_id);
            ASSERT_EQ(i * MAX_DOC_PER_RECORD, decoder->GetSeekedDocCount());
            ttf_t expect_ttf = need_tf ? 2 * i * MAX_DOC_PER_RECORD : 0;
            ASSERT_EQ(expect_ttf, ttf);
            if (need_tf) {
                decoder->DecodeCurrentTFBuffer(tf_buffer.data() + i * MAX_DOC_PER_RECORD);
            }
            decoder->DecodeCurrentDocPayloadBuffer(doc_payload.data() + i * MAX_DOC_PER_RECORD);
        }

        if (doc_count % MAX_DOC_PER_RECORD > 0) {
            ASSERT_TRUE(decoder->DecodeSkipList(i * MAX_DOC_PER_RECORD, prev_last_doc_id, last_doc_id, ttf));
            ASSERT_TRUE(decoder->DecodeCurrentDocIDBuffer(doc_buffer.data() + i * MAX_DOC_PER_RECORD));
            first_doc_id = prev_last_doc_id + doc_buffer[i * MAX_DOC_PER_RECORD];
            ASSERT_EQ(i * MAX_DOC_PER_RECORD, decoder->GetSeekedDocCount());
            ASSERT_EQ((docid_t)(i * MAX_DOC_PER_RECORD), first_doc_id);
            ASSERT_EQ((docid_t)(doc_count - 1), last_doc_id);
            ttf_t expect_ttf = need_tf ? 2 * i * MAX_DOC_PER_RECORD : 0;
            ASSERT_EQ(expect_ttf, ttf);
            if (need_tf) {
                decoder->DecodeCurrentTFBuffer(tf_buffer.data() + i * MAX_DOC_PER_RECORD);
            }
            decoder->DecodeCurrentDocPayloadBuffer(doc_payload.data() + i * MAX_DOC_PER_RECORD);
        }

        docid_t pre_doc_id = 0;
        for (uint32_t i = 0; i < doc_count; ++i) {
            ASSERT_EQ((docid_t)(i - pre_doc_id), doc_buffer[i]);
            pre_doc_id = i;
            ASSERT_EQ((docpayload_t)(i * 2), doc_payload[i]);
            if (need_tf) {
                ASSERT_EQ((tf_t)2, tf_buffer[i]);
            }
        }

        ASSERT_TRUE(!decoder->DecodeSkipList(last_doc_id + 1, prev_last_doc_id, last_doc_id, ttf));
        decoder->~InMemDocListDecoder();
        byte_slice_pool_->Deallocate((void *)decoder, sizeof(InMemDocListDecoder));
    }

    RecyclePool *buffer_pool_;
    MemoryPool *byte_slice_pool_;
    InMemDocListDecoder *doc_list_decoder_;
    DocListEncoder *doc_list_encoder_;

    SharedPtr<DocListFormat> doc_list_format_ptr_;
    SharedPtr<DocListEncoder> doc_list_encoder_ptr_;
};

TEST_F(InMemDocListDecoderTest, test1) {
    using namespace infinity;

    DocListFormatOption option(OPTION_FLAG_NONE);
    DocListFormat doc_list_format(option);
    PostingByteSlice *posting_buffer = new (byte_slice_pool_->Allocate(sizeof(PostingByteSlice))) PostingByteSlice(byte_slice_pool_, buffer_pool_);
    posting_buffer->Init(&doc_list_format);

    docid_t doc1 = 1;
    docid_t doc2 = 10;
    posting_buffer->PushBack(0, doc1);
    posting_buffer->EndPushBack();
    posting_buffer->PushBack(0, doc2 - doc1);
    posting_buffer->EndPushBack();

    InMemDocListDecoder doc_list_decoder(byte_slice_pool_, option);
    doc_list_decoder.Init(2, nullptr, posting_buffer);

    docid_t prev_last_doc_id = 0;
    docid_t first_doc_id = 0;
    docid_t last_doc_id = 0;
    ttf_t current_ttf = 0;
    docid_t doc_buffer[1024];
    ASSERT_TRUE(doc_list_decoder.DecodeSkipList(0, prev_last_doc_id, last_doc_id, current_ttf));
    ASSERT_TRUE(doc_list_decoder.DecodeCurrentDocIDBuffer(doc_buffer));
    first_doc_id = prev_last_doc_id + doc_buffer[0];

    ASSERT_EQ(doc1, first_doc_id);
    ASSERT_EQ(doc2, last_doc_id);
    ASSERT_EQ((ttf_t)0, current_ttf);
    ASSERT_EQ(doc1, doc_buffer[0]);
    ASSERT_EQ(doc2 - doc1, doc_buffer[1]);

    ASSERT_TRUE(!doc_list_decoder.DecodeSkipList(doc2, prev_last_doc_id, last_doc_id, current_ttf));
}

TEST_F(InMemDocListDecoderTest, test2) {
    using namespace infinity;

    DocListFormatOption option(OPTION_FLAG_NONE);
    DocListFormat doc_list_format(option);

    PostingByteSlice *posting_buffer = new (byte_slice_pool_->Allocate(sizeof(PostingByteSlice))) PostingByteSlice(byte_slice_pool_, buffer_pool_);

    posting_buffer->Init(&doc_list_format);

    for (docid_t i = 0; i < 128; ++i) {
        posting_buffer->PushBack(0, i);
        posting_buffer->EndPushBack();
    }
    posting_buffer->Flush();

    InMemDocListDecoder doc_list_decoder(byte_slice_pool_, option);
    doc_list_decoder.Init(2, nullptr, posting_buffer);

    docid_t prev_last_doc_id = 0;
    docid_t first_doc_id = 0;
    docid_t last_doc_id = 0;
    ttf_t current_ttf = 0;
    docid_t doc_buffer[1024];
    ASSERT_TRUE(doc_list_decoder.DecodeSkipList(0, prev_last_doc_id, last_doc_id, current_ttf));
    ASSERT_TRUE(doc_list_decoder.DecodeCurrentDocIDBuffer(doc_buffer));
    first_doc_id = prev_last_doc_id + doc_buffer[0];

    ASSERT_EQ((u32)0, first_doc_id);
    ASSERT_EQ((u32)8128, last_doc_id);
    ASSERT_EQ((ttf_t)0, current_ttf);
    ASSERT_EQ((u32)0, doc_buffer[0]);
    ASSERT_EQ((u32)1, doc_buffer[1]);
    ASSERT_EQ((u32)2, doc_buffer[2]);

    ASSERT_TRUE(!doc_list_decoder.DecodeSkipList(last_doc_id, prev_last_doc_id, last_doc_id, current_ttf));

    posting_buffer->PushBack(0, 128);
    posting_buffer->EndPushBack();

    // without skiplist, we only decode one time
    ASSERT_TRUE(!doc_list_decoder.DecodeSkipList(last_doc_id, prev_last_doc_id, last_doc_id, current_ttf));

    posting_buffer->Flush();

    // without skiplist, we only decode one time
    ASSERT_TRUE(!doc_list_decoder.DecodeSkipList(last_doc_id, prev_last_doc_id, last_doc_id, current_ttf));
}

TEST_F(InMemDocListDecoderTest, test3) {
    using namespace infinity;

    docid_t prev_last_doc_id = 0;
    docid_t first_doc_id = 0;
    docid_t last_doc_id = 0;
    ttf_t current_ttf = 0;
    docid_t doc_buffer[MAX_DOC_PER_RECORD];

    doc_list_decoder_ = doc_list_encoder_->GetInMemDocListDecoder(byte_slice_pool_);

    ASSERT_TRUE(doc_list_decoder_->DecodeSkipList(256, prev_last_doc_id, last_doc_id, current_ttf));
    ASSERT_TRUE(doc_list_decoder_->DecodeCurrentDocIDBuffer(doc_buffer));
    first_doc_id = prev_last_doc_id + doc_buffer[0];

    ASSERT_EQ((u32)256, first_doc_id);
    ASSERT_EQ((u32)510, last_doc_id);
    ASSERT_EQ((ttf_t)0, current_ttf);
    ASSERT_EQ((u32)129, doc_buffer[0]);
    ASSERT_EQ((u32)2, doc_buffer[1]);

    ASSERT_TRUE(doc_list_decoder_->DecodeSkipList(1000, prev_last_doc_id, last_doc_id, current_ttf));
    ASSERT_TRUE(doc_list_decoder_->DecodeCurrentDocIDBuffer(doc_buffer));
    first_doc_id = prev_last_doc_id + doc_buffer[0];

    ASSERT_EQ((u32)1000, first_doc_id);
    ASSERT_EQ((u32)1000, last_doc_id);
    ASSERT_EQ((ttf_t)0, current_ttf);
    ASSERT_EQ((u32)490, doc_buffer[0]);

    ASSERT_TRUE(!doc_list_decoder_->DecodeSkipList(last_doc_id, prev_last_doc_id, last_doc_id, current_ttf));

    doc_list_decoder_->~InMemDocListDecoder();
    byte_slice_pool_->Deallocate((void *)doc_list_decoder_, sizeof(InMemDocListDecoder));
}

TEST_F(InMemDocListDecoderTest, test4) {
    using namespace infinity;

    docid_t prev_last_doc_id = 0;
    docid_t first_doc_id = 0;
    docid_t last_doc_id = 0;
    ttf_t current_ttf = 0;
    docid_t doc_buffer[MAX_DOC_PER_RECORD];

    doc_list_encoder_->AddPosition();
    doc_list_encoder_->EndDocument(1001, default_document_legnth, 0);
    doc_list_encoder_->AddPosition();
    doc_list_encoder_->EndDocument(1002, default_document_legnth, 0);

    PostingByteSlice *posting_buffer = doc_list_encoder_->GetDocListBuffer();
    posting_buffer->Flush();

    doc_list_decoder_ = doc_list_encoder_->GetInMemDocListDecoder(byte_slice_pool_);

    ASSERT_TRUE(doc_list_decoder_->DecodeSkipList(1000, prev_last_doc_id, last_doc_id, current_ttf));
    ASSERT_TRUE(doc_list_decoder_->DecodeCurrentDocIDBuffer(doc_buffer));
    first_doc_id = prev_last_doc_id + doc_buffer[0];

    ASSERT_EQ((u32)1000, first_doc_id);
    ASSERT_EQ((u32)1002, last_doc_id);
    ASSERT_EQ((ttf_t)0, current_ttf);
    ASSERT_EQ((u32)490, doc_buffer[0]);
    ASSERT_EQ((u32)1, doc_buffer[1]);
    ASSERT_EQ((u32)1, doc_buffer[2]);

    ASSERT_TRUE(!doc_list_decoder_->DecodeSkipList(last_doc_id, prev_last_doc_id, last_doc_id, current_ttf));

    doc_list_decoder_->~InMemDocListDecoder();
    byte_slice_pool_->Deallocate((void *)doc_list_decoder_, sizeof(InMemDocListDecoder));
}

TEST_F(InMemDocListDecoderTest, test5) {
    using namespace infinity;

    docid_t prev_last_doc_id = 0;
    docid_t first_doc_id = 0;
    docid_t last_doc_id = 0;
    ttf_t current_ttf = 0;
    docid_t doc_buffer[MAX_DOC_PER_RECORD];

    PostingByteSlice *posting_buffer = doc_list_encoder_->GetDocListBuffer();

    for (docid_t i = 0; i < 300; ++i) {
        posting_buffer->PushBack(0, i);
        posting_buffer->EndPushBack();
        if (posting_buffer->NeedFlush()) {
            posting_buffer->Flush();
        }
    }
    doc_list_decoder_ = doc_list_encoder_->GetInMemDocListDecoder(byte_slice_pool_);

    ASSERT_TRUE(doc_list_decoder_->DecodeSkipList(1000, prev_last_doc_id, last_doc_id, current_ttf));
    ASSERT_TRUE(doc_list_decoder_->DecodeCurrentDocIDBuffer(doc_buffer));
    first_doc_id = prev_last_doc_id + doc_buffer[0];

    ASSERT_EQ((u32)1000, first_doc_id);
    ASSERT_EQ((u32)9001, last_doc_id);
    ASSERT_EQ((ttf_t)0, current_ttf);
    ASSERT_EQ((u32)490, doc_buffer[0]);
    ASSERT_EQ((u32)0, doc_buffer[1]);
    ASSERT_EQ((u32)1, doc_buffer[2]);
    ASSERT_EQ((u32)2, doc_buffer[3]);

    ASSERT_TRUE(!doc_list_decoder_->DecodeSkipList(last_doc_id, prev_last_doc_id, last_doc_id, current_ttf));

    doc_list_decoder_->~InMemDocListDecoder();
    byte_slice_pool_->Deallocate((void *)doc_list_decoder_, sizeof(InMemDocListDecoder));
}

TEST_F(InMemDocListDecoderTest, test6) {
    using namespace infinity;

    docid_t docids[] = {1, 10};
    tf_t tfs[] = {2, 4};
    TestDecodeWithOptionFlag(of_term_frequency, 2, docids, tfs, nullptr);
}

TEST_F(InMemDocListDecoderTest, test7) {
    using namespace infinity;

    docid_t docids[] = {1, 10};
    docpayload_t docPayloads[] = {2, 4};
    TestDecodeWithOptionFlag(of_doc_payload, 2, docids, nullptr, docPayloads);
}

TEST_F(InMemDocListDecoderTest, test8) {
    using namespace infinity;

    docid_t docids[] = {1, 10};
    tf_t tfs[] = {2, 4};
    docpayload_t docPayloads[] = {2, 4};
    TestDecodeWithOptionFlag(of_term_frequency | of_doc_payload, 2, docids, tfs, docPayloads);
}

TEST_F(InMemDocListDecoderTest, test9) {
    using namespace infinity;

    TestDecode(1);
    TestDecode(MAX_UNCOMPRESSED_DOC_LIST_SIZE + 1);
    TestDecode(MAX_DOC_PER_RECORD);
    TestDecode(MAX_DOC_PER_RECORD + 10);
    TestDecode(MAX_DOC_PER_RECORD * 5 + 1);
    TestDecode(MAX_DOC_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1);
}

TEST_F(InMemDocListDecoderTest, test10) {
    using namespace infinity;

    TestDecode(1, true);
    TestDecode(MAX_UNCOMPRESSED_DOC_LIST_SIZE + 1, true);
    TestDecode(MAX_DOC_PER_RECORD, true);
    TestDecode(MAX_DOC_PER_RECORD + 10, true);
    TestDecode(MAX_DOC_PER_RECORD * 5 + 1, true);
    TestDecode(MAX_DOC_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1, true);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1, true);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1, true);
}

TEST_F(InMemDocListDecoderTest, test11) {
    using namespace infinity;

    TestDecode(1, false, true);
    TestDecode(MAX_UNCOMPRESSED_DOC_LIST_SIZE + 1, false, true);
    TestDecode(MAX_DOC_PER_RECORD, false, true);
    TestDecode(MAX_DOC_PER_RECORD + 10, false, true);
    TestDecode(MAX_DOC_PER_RECORD * 5 + 1, false, true);
    TestDecode(MAX_DOC_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1, false, true);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1, false, true);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1, false, true);
}

TEST_F(InMemDocListDecoderTest, test12) {
    using namespace infinity;

    TestDecode(1, true, true);
    TestDecode(MAX_UNCOMPRESSED_DOC_LIST_SIZE + 1, true, true);
    TestDecode(MAX_DOC_PER_RECORD, true, true);
    TestDecode(MAX_DOC_PER_RECORD + 10, true, true);
    TestDecode(MAX_DOC_PER_RECORD * 5 + 1, true, true);
    TestDecode(MAX_DOC_PER_RECORD * MAX_UNCOMPRESSED_SKIP_LIST_SIZE + 1, true, true);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE + 1, true, true);
    TestDecode(MAX_DOC_PER_RECORD * SKIP_LIST_BUFFER_SIZE * 2 + 1, true, true);
}
