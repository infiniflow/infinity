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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.posting_writer;

import :ut.base_test;
import :stl;
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
#endif

import data_type;
import internal_types;

using namespace infinity;

class PostingWriterTest : public BaseTestParamStr {
public:
    PostingWriterTest() {}

    void SetUp() override {
        BaseTestParamStr::SetUp();
        file_ = String(GetFullTmpDir()) + "/posting_writer";
    }

protected:
    String file_;
    optionflag_t flag_{OPTION_FLAG_ALL};
    PostingFormat posting_format_{flag_};
    String config_path_{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         PostingWriterTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(PostingWriterTest, test1) {
    Vector<docid_t> expected = {1, 3, 5, 7, 9};
    VectorWithLock<u32> column_length_array(20, 10);
    {
        SharedPtr<PostingWriter> posting = MakeShared<PostingWriter>(posting_format_, column_length_array);

        for (u32 i = 0; i < expected.size(); ++i) {
            posting->AddPosition(1);
            posting->AddPosition(3);
            posting->AddPosition(5);
            posting->EndDocument(expected[i], 0);
        }

        SharedPtr<FileWriter> file_writer = MakeShared<FileWriter>(file_, 128000);
        TermMeta term_meta(posting->GetDF(), posting->GetTotalTF());
        posting->Dump(file_writer, term_meta, true);
        file_writer->Sync();
    }
    {
        SharedPtr<PostingWriter> posting = MakeShared<PostingWriter>(posting_format_, column_length_array);
        SharedPtr<FileReader> file_reader = MakeShared<FileReader>(file_, 128000);
        posting->Load(file_reader);

        docid_t docid = 10;
        {
            posting->AddPosition(1);
            posting->AddPosition(3);
            posting->AddPosition(5);
            posting->EndDocument(docid, 0);
        }

        SharedPtr<Vector<SegmentPosting>> seg_postings = MakeShared<Vector<SegmentPosting>>();
        SegmentPosting seg_posting;
        RowID base_row_id = 0;
        seg_posting.Init(base_row_id, posting);
        seg_postings->push_back(seg_posting);
        PostingIterator iter(flag_);
        iter.Init(seg_postings, 0);

        RowID doc_id = INVALID_ROWID;
        for (SizeT j = 0; j < expected.size(); ++j) {
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
