module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.skiplist_reader;

import :ut.base_test;
import :posting_byte_slice;
import :in_doc_pos_state;
import :byte_slice;
import :index_defines;
import :skiplist_writer;
import :file_writer;
import :posting_field;
import :posting_byte_slice_reader;
import :skiplist_reader;
import :posting_list_format;
import third_party;
import :byte_slice_reader;
import :random;

using namespace infinity;

class SkiplistReaderTest : public BaseTest {
public:
    SkiplistReaderTest() {}
    ~SkiplistReaderTest() {}
    void SetUp() override { skiplist_writer_ = std::make_shared<SkipListWriter>(); }
    void TearDown() override { skiplist_writer_.reset(); }

protected:
    void InitSkiplistWriter(PostingFields &posting_fields,
                            size_t doc_num,
                            std::vector<u32> &doc_ids,
                            std::vector<u32> &tfs,
                            std::vector<u32> &deltas,
                            std::vector<u32> &offsets);

protected:
    std::shared_ptr<SkipListWriter> skiplist_writer_ = nullptr;
    static constexpr size_t BUFFER_SIZE_ = 1024;
};

void SkiplistReaderTest::InitSkiplistWriter(PostingFields &posting_fields,
                                            size_t doc_num,
                                            std::vector<u32> &doc_ids,
                                            std::vector<u32> &tfs,
                                            std::vector<u32> &deltas,
                                            std::vector<u32> &offsets) {
    u8 row_count = 0;
    u32 offset = 0;

    auto posting_field_doc_id = new TypedPostingField<u32>;
    posting_field_doc_id->location_ = row_count++;
    posting_field_doc_id->offset_ = offset;
    posting_field_doc_id->encoder_ = GetSkipListEncoder();
    offset += sizeof(u32);

    auto posting_field_tf = new TypedPostingField<u32>;
    posting_field_tf->location_ = row_count++;
    posting_field_tf->offset_ = offset;
    posting_field_tf->encoder_ = GetSkipListEncoder();
    offset += sizeof(u32);

    auto posting_field_offset = new TypedPostingField<u32>;
    posting_field_offset->location_ = row_count++;
    posting_field_offset->offset_ = offset;
    posting_field_offset->encoder_ = GetSkipListEncoder();
    offset += sizeof(u32);

    posting_fields.AddValue(posting_field_doc_id);
    posting_fields.AddValue(posting_field_tf);
    posting_fields.AddValue(posting_field_offset);

    skiplist_writer_->Init(&posting_fields);

    u32 now_offset = 0;
    for (size_t i = 0; i < doc_num; ++i) {
        doc_ids.emplace_back(i);
        tfs.emplace_back(i * 10);
        deltas.emplace_back(i * 100);
        offsets.emplace_back(now_offset);
        now_offset += deltas.back();
        skiplist_writer_->AddItem(doc_ids.back(), tfs.back(), deltas.back());
    }
    skiplist_writer_->Flush();
}

TEST_F(SkiplistReaderTest, SkipListReaderByteSliceTest) {
    PostingFields posting_fields;
    auto doc_num = SKIP_LIST_BUFFER_SIZE + 2;
    std::vector<u32> doc_ids;
    std::vector<u32> tfs;
    std::vector<u32> deltas;
    std::vector<u32> offsets;
    InitSkiplistWriter(posting_fields, doc_num, doc_ids, tfs, deltas, offsets);

    auto byte_slice_list = skiplist_writer_->GetByteSliceList();

    PostingFormatOption format_option(of_term_frequency);
    auto doc_list_format_option = format_option.GetDocListFormatOption();

    u32 res_doc_id = -1;
    u32 res_prev_doc_id = -1;
    u32 res_offset = -1;
    u32 res_delta = -1;

    {
        auto skiplist_reader = std::make_shared<SkipListReaderByteSlice>(doc_list_format_option);
        skiplist_reader->Load(byte_slice_list, static_cast<u32>(0), static_cast<u32>(byte_slice_list->GetTotalSize()));
        for (auto query_doc_id = 0; query_doc_id < doc_num; ++query_doc_id) {
            skiplist_reader->SkipTo(query_doc_id, res_doc_id, res_prev_doc_id, res_offset, res_delta);
            EXPECT_EQ(res_doc_id, doc_ids[query_doc_id]);
            EXPECT_EQ(res_offset, offsets[query_doc_id]);
            EXPECT_EQ(res_delta, deltas[query_doc_id]);
        }
    }

    {
        auto skiplist_reader = std::make_shared<SkipListReaderByteSlice>(doc_list_format_option);
        skiplist_reader->Load(byte_slice_list, static_cast<u32>(0), static_cast<u32>(byte_slice_list->GetTotalSize()));
        for (auto query_doc_id = 0; query_doc_id < doc_num; query_doc_id += 3) {
            skiplist_reader->SkipTo(query_doc_id, res_doc_id, res_prev_doc_id, res_offset, res_delta);
            EXPECT_EQ(res_doc_id, doc_ids[query_doc_id]);
            EXPECT_EQ(res_offset, offsets[query_doc_id]);
            EXPECT_EQ(res_delta, deltas[query_doc_id]);
        }
    }

    {
        auto skiplist_reader = std::make_shared<SkipListReaderByteSlice>(doc_list_format_option);
        skiplist_reader->Load(byte_slice_list, static_cast<u32>(0), static_cast<u32>(byte_slice_list->GetTotalSize()));
        for (auto query_doc_id = 0; query_doc_id < doc_num; query_doc_id += random() % doc_num + 1) {
            skiplist_reader->SkipTo(query_doc_id, res_doc_id, res_prev_doc_id, res_offset, res_delta);
            EXPECT_EQ(res_doc_id, doc_ids[query_doc_id]);
            EXPECT_EQ(res_offset, offsets[query_doc_id]);
            EXPECT_EQ(res_delta, deltas[query_doc_id]);
        }
    }
}

TEST_F(SkiplistReaderTest, SkipListReaderPostingByteSliceTest) {
    PostingFields posting_fields;
    size_t doc_num = SKIP_LIST_BUFFER_SIZE + 2;
    std::vector<u32> doc_ids;
    std::vector<u32> tfs;
    std::vector<u32> deltas;
    std::vector<u32> offsets;

    InitSkiplistWriter(posting_fields, doc_num, doc_ids, tfs, deltas, offsets);

    PostingFormatOption format_option(of_term_frequency);
    auto doc_list_format_option = format_option.GetDocListFormatOption();
    u32 res_doc_id = -1;
    u32 res_prev_doc_id = -1;
    u32 res_offset = -1;
    u32 res_delta = -1;
    {
        auto skiplist_reader = std::make_shared<SkipListReaderPostingByteSlice>(doc_list_format_option);
        skiplist_reader->Load(skiplist_writer_.get());
        for (size_t query_doc_id = 0; query_doc_id < doc_num; ++query_doc_id) {
            skiplist_reader->SkipTo(query_doc_id, res_doc_id, res_prev_doc_id, res_offset, res_delta);
            EXPECT_EQ(res_doc_id, doc_ids[query_doc_id]);
            EXPECT_EQ(res_offset, offsets[query_doc_id]);
            EXPECT_EQ(res_delta, deltas[query_doc_id]);
        }
    }

    {
        auto skiplist_reader = std::make_shared<SkipListReaderPostingByteSlice>(doc_list_format_option);
        skiplist_reader->Load(skiplist_writer_.get());
        for (size_t query_doc_id = 0; query_doc_id < doc_num; query_doc_id += 3) {
            skiplist_reader->SkipTo(query_doc_id, res_doc_id, res_prev_doc_id, res_offset, res_delta);
            EXPECT_EQ(res_doc_id, doc_ids[query_doc_id]);
            EXPECT_EQ(res_offset, offsets[query_doc_id]);
            EXPECT_EQ(res_delta, deltas[query_doc_id]);
        }
    }

    {
        auto skiplist_reader = std::make_shared<SkipListReaderPostingByteSlice>(doc_list_format_option);
        skiplist_reader->Load(skiplist_writer_.get());
        for (size_t query_doc_id = 0; query_doc_id < doc_num; query_doc_id += random() % doc_num + 1) {
            skiplist_reader->SkipTo(query_doc_id, res_doc_id, res_prev_doc_id, res_offset, res_delta);
            EXPECT_EQ(res_doc_id, doc_ids[query_doc_id]);
            EXPECT_EQ(res_offset, offsets[query_doc_id]);
            EXPECT_EQ(res_delta, deltas[query_doc_id]);
        }
    }
}
