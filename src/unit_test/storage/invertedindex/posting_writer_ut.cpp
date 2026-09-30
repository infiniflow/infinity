// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.posting_writer;

import :ut.base_test;

import :index_defines;
import :posting_list_format;
import :file_writer;
import :file_reader;
import :posting_writer;
import :term_meta;
import :segment_posting;
import :posting_iterator;
import :vector_with_lock;
import :infinity_context;
import :virtual_store;
import :byte_slice;

import data_type;
import internal_types;

using namespace infinity;

class PostingWriterTest : public BaseTestParamStr {
public:
    PostingWriterTest() {}

    void SetUp() override {
        BaseTestParamStr::SetUp();
        file_ = std::string(GetFullTmpDir()) + "/posting_writer";
    }

protected:
    std::string file_;
    optionflag_t flag_{OPTION_FLAG_ALL};
    PostingFormat posting_format_{flag_};
    std::string config_path_{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         PostingWriterTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(PostingWriterTest, test1) {
    std::vector<docid_t> expected = {1, 3, 5, 7, 9};
    VectorWithLock<u32> column_length_array(20, 10);
    {
        std::shared_ptr<PostingWriter> posting = std::make_shared<PostingWriter>(posting_format_, column_length_array);

        for (u32 i = 0; i < expected.size(); ++i) {
            posting->AddPosition(1);
            posting->AddPosition(3);
            posting->AddPosition(5);
            posting->EndDocument(expected[i], 0);
        }

        std::shared_ptr<FileWriter> file_writer = std::make_shared<FileWriter>(file_, 128000);
        TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
        posting->Dump(file_writer, term_meta, true);
        file_writer->Sync();
    }
    {
        std::shared_ptr<PostingWriter> posting = std::make_shared<PostingWriter>(posting_format_, column_length_array);
        std::shared_ptr<FileReader> file_reader = std::make_shared<FileReader>(file_, 128000);
        posting->Load(file_reader);

        docid_t docid = 10;
        {
            posting->AddPosition(1);
            posting->AddPosition(3);
            posting->AddPosition(5);
            posting->EndDocument(docid, 0);
        }

        std::shared_ptr<std::vector<SegmentPosting>> seg_postings = std::make_shared<std::vector<SegmentPosting>>();
        SegmentPosting seg_posting;
        RowID base_row_id = 0;
        seg_posting.Init(base_row_id, posting);
        seg_postings->push_back(seg_posting);
        PostingIterator iter(flag_);
        iter.Init(seg_postings, 0);

        RowID doc_id = INVALID_ROWID;
        for (size_t j = 0; j < expected.size(); ++j) {
            doc_id = iter.SeekDoc(expected[j]);
            ASSERT_EQ(doc_id, expected[j]);
            u32 tf = iter.GetCurrentTF();
            ASSERT_EQ(tf, (u32)3);
        }
        if (doc_id != INVALID_ROWID) {
            doc_id = iter.SeekDoc(doc_id + 1);
            ASSERT_EQ(doc_id, 10);
        }
    }
}

TEST_P(PostingWriterTest, tf_after_skip_to_across_segments) {
    VectorWithLock<u32> column_length_array(2000, 10);
    std::vector<std::vector<u8>> storage;
    auto seg_postings = std::make_shared<std::vector<SegmentPosting>>();
    for (u32 seg = 0; seg < 2; ++seg) {
        std::string path = file_ + "_seg" + std::to_string(seg);
        auto posting = std::make_shared<PostingWriter>(posting_format_, column_length_array);
        for (docid_t d = 1; d <= 300; ++d) {
            posting->AddPosition(1);
            posting->EndDocument(d, 0);
        }
        TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
        {
            auto file_writer = std::make_shared<FileWriter>(path, 128000);
            posting->Dump(file_writer, term_meta);
            file_writer->Sync();
        }
        size_t size = VirtualStore::GetFileSize(path);
        storage.emplace_back(size);
        FileReader file_reader(path, 128000);
        file_reader.Read((char *)storage.back().data(), size);
        auto slice_list = std::make_shared<ByteSliceList>(ByteSlice::NewSlice(storage.back().data(), size));
        SegmentPosting seg_posting;
        seg_posting.Init(slice_list, RowID(seg, 0), term_meta.doc_freq_, term_meta);
        seg_postings->push_back(seg_posting);
    }
    PostingIterator iter(flag_);
    iter.Init(seg_postings, 0);

    ASSERT_EQ(iter.SeekDoc(RowID(0, 0)), RowID(0, 1));
    ASSERT_EQ(iter.GetCurrentTF(), 1u);
    ASSERT_TRUE(iter.SkipTo(RowID(1, 10)));
    ASSERT_EQ(iter.GetCurrentTF(), 0u);
    ASSERT_EQ(iter.SeekDoc(RowID(1, 10)), RowID(1, 10));
    ASSERT_EQ(iter.GetCurrentTF(), 1u);
}
