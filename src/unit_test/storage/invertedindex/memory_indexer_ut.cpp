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

module infinity_core:ut.memory_indexer;

import :ut.base_test;

import :analyzer;
import :analyzer_pool;
import :index_defines;
import :posting_list_format;
import :column_vector;
import :value;
import :memory_indexer;
import :column_index_reader;
import :posting_iterator;
import :column_inverter;
import :column_index_merger;
import third_party;
import :inmem_posting_decoder;
import :inmem_position_list_decoder;
import :inmem_index_segment_reader;
import :segment_posting;
import :infinity_context;
import third_party;
import :db_meta;
import :table_meta;
import :table_index_meta;
import :new_txn;
import :txn_state;
import :new_txn_manager;
import :data_block;
import :table_def;
import :index_full_text;
import :status;

import data_type;
import internal_types;
import logical_type;
import global_resource_usage;
import column_def;
import extra_ddl_info;
import statement_common;

using namespace infinity;

class MemoryIndexerTest : public BaseTestParamStr {
public:
    struct ExpectedPosting {
        std::string term;
        std::vector<RowID> doc_ids;
        std::vector<u32> tfs;
    };

protected:
    optionflag_t flag_{OPTION_FLAG_ALL};
    std::vector<std::string> wiki_paragraphs_;
    std::vector<ExpectedPosting> expected_postings_;
    std::shared_ptr<DBMeta> db_meta_;
    std::shared_ptr<TableMeta> table_meta_;
    std::shared_ptr<TableIndexMeta> index_meta_;

public:
    void SetUp() override {
        BaseTestParamStr::SetUp();
        auto *new_txn_mgr = infinity::InfinityContext::instance().storage()->new_txn_manager();
        auto db_name = std::make_shared<std::string>("db1");
        auto column_def1 = std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kVarchar), "col1", std::set<ConstraintType>());
        auto table_name = std::make_shared<std::string>("tb1");
        auto table_def = TableDef::Make(db_name, table_name, std::make_shared<std::string>(), {column_def1});
        auto index_name = std::make_shared<std::string>("idx1");
        std::vector<InitParameter *> index_param_list;
        auto index_base = IndexFullText::Make(index_name, std::make_shared<std::string>(), "file_name", {column_def1->name()}, index_param_list);
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create db"), TransactionType::kCreateDB);
            Status status = txn->CreateDatabase(*db_name, ConflictType::kError, std::make_shared<std::string>());
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create table"), TransactionType::kCreateTable);
            Status status = txn->CreateTable(*db_name, std::move(table_def), ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }
        {
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("create index"), TransactionType::kCreateIndex);
            Status status = txn->CreateIndex(*db_name, *table_name, index_base, ConflictType::kIgnore);
            EXPECT_TRUE(status.ok());
            status = new_txn_mgr->CommitTxn(txn);
            EXPECT_TRUE(status.ok());
        }

        // https://en.wikipedia.org/wiki/Finite-state_transducer
        wiki_paragraphs_ = {
            R"#(A finite-state transducer (FST) is a finite-state machine with two memory tapes, following the terminology for Turing machines: an input tape and an output tape. This contrasts with an ordinary finite-state automaton, which has a single tape. An FST is a type of finite-state automaton (FSA) that maps between two sets of symbols.[1] An FST is more general than an FSA. An FSA defines a formal language by defining a set of accepted strings, while an FST defines a relation between sets of strings.)#",
            R"#(An FST will read a set of strings on the input tape and generates a set of relations on the output tape. An FST can be thought of as a translator or relater between strings in a set.)#",
            R"#(In morphological parsing, an example would be inputting a string of letters into the FST, the FST would then output a string of morphemes.)#",
            R"#(An automaton can be said to recognize a string if we view the content of its tape as input. In other words, the automaton computes a function that maps strings into the set {0,1}. Alternatively, we can say that an automaton generates strings, which means viewing its tape as an output tape. On this view, the automaton generates a formal language, which is a set of strings. The two views of automata are equivalent: the function that the automaton computes is precisely the indicator function of the set of strings it generates. The class of languages generated by finite automata is known as the class of regular languages.)#",
            R"#(The two tapes of a transducer are typically viewed as an input tape and an output tape. On this view, a transducer is said to transduce (i.e., translate) the contents of its input tape to its output tape, by accepting a string on its input tape and generating another string on its output tape. It may do so nondeterministically and it may produce more than one output for each input string. A transducer may also produce no output for a given input string, in which case it is said to reject the input. In general, a transducer computes a relation between two formal languages.)#",
        };

        expected_postings_ = {{"fst", {0, 1, 2}, {4, 2, 2}}, {"automaton", {0, 3}, {2, 5}}, {"transducer", {0, 4}, {1, 4}}};
    }

    std::shared_ptr<ColumnVector> MakeColumnVector(const std::vector<std::string> &paragraphs, size_t repeats = 1) {
        auto col_vec = ColumnVector::Make(std::make_shared<DataType>(LogicalType::kVarchar));
        col_vec->Initialize();
        for (size_t i = 0; i < repeats; i++) {
            for (size_t j = 0; j < paragraphs.size(); ++j) {
                Value value = Value::MakeVarchar(paragraphs[j]);
                col_vec->AppendValue(value);
            }
        }
        return col_vec;
    }

    std::shared_ptr<DataBlock> MakeInputBlock(const std::vector<std::string> &paragraphs, size_t repeats = 1) {
        auto data_block = DataBlock::Make();
        auto col_vec = MakeColumnVector(paragraphs, repeats);
        data_block->InsertVector(col_vec, 0);
        data_block->Finalize();
        return data_block;
    }

    void Check() {
        {
            std::string table_key;
            std::string index_key;
            auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
            auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("dummy"), TransactionType::kRead);
            Status status = txn->GetTableIndexMeta("db1", "tb1", "idx1", db_meta_, table_meta_, index_meta_, &table_key, &index_key);
            EXPECT_TRUE(status.ok());
            status = txn->Commit();
            EXPECT_TRUE(status.ok());
        }
        ColumnIndexReader reader;
        reader.Open(flag_, *index_meta_);
        // Pair<u64, float> res = reader.GetTotalDfAndAvgColumnLength();
        // ASSERT_GT(res.first, 0);     // row count
        // ASSERT_GT(res.second, 0.0f); // avg column length
        for (size_t i = 0; i < expected_postings_.size(); ++i) {
            const ExpectedPosting &expected = expected_postings_[i];
            const std::string &term = expected.term;

            std::unique_ptr<PostingIterator> post_iter(reader.Lookup(term));
            ASSERT_TRUE(post_iter != nullptr);

            RowID doc_id = INVALID_ROWID;
            for (size_t j = 0; j < expected.doc_ids.size(); ++j) {
                doc_id = post_iter->SeekDoc(expected.doc_ids[j]);
                ASSERT_EQ(doc_id, expected.doc_ids[j]);
                u32 tf = post_iter->GetCurrentTF();
                ASSERT_EQ(tf, expected.tfs[j]);
                pos_t cur_pos = 0;
                pos_t res_pos = INVALID_POSITION;
                do {
                    post_iter->SeekPosition(cur_pos, res_pos);
                    cur_pos = res_pos + 1;
                } while (res_pos != INVALID_POSITION);
            }
            if (doc_id != INVALID_ROWID) {
                doc_id = post_iter->SeekDoc(doc_id + 1);
                ASSERT_EQ(doc_id, INVALID_ROWID);
            }
        }
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         MemoryIndexerTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(MemoryIndexerTest, Chunk) {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto db_name = std::make_shared<std::string>("db1");
    auto table_name = std::make_shared<std::string>("tb1");
    std::vector<std::shared_ptr<DataBlock>> blocks = {MakeInputBlock(wiki_paragraphs_)};

    auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("import"), TransactionType::kImport);
    auto status = txn->Import(*db_name, *table_name, blocks);
    EXPECT_TRUE(status.ok());
    status = new_txn_mgr->CommitTxn(txn);
    EXPECT_TRUE(status.ok());
    Check();
}

TEST_P(MemoryIndexerTest, SLOW_Memory) {
    auto *new_txn_mgr = InfinityContext::instance().storage()->new_txn_manager();
    auto db_name = std::make_shared<std::string>("db1");
    auto table_name = std::make_shared<std::string>("tb1");
    auto index_name = std::make_shared<std::string>("idx1");
    std::vector<std::string> paragraphs1(wiki_paragraphs_.begin(), wiki_paragraphs_.begin() + 4); // 4 rows
    std::vector<std::string> paragraphs2(wiki_paragraphs_.begin() + 4, wiki_paragraphs_.end());   // 1 rows
    std::vector<std::shared_ptr<DataBlock>> blocks = {MakeInputBlock(paragraphs1), MakeInputBlock(paragraphs2)};

    for (size_t i = 0; i < blocks.size(); ++i) {
        auto *txn = new_txn_mgr->BeginTxn(std::make_unique<std::string>("append"), TransactionType::kAppend);
        Status status = txn->Append(*db_name, *table_name, blocks[i]);
        EXPECT_TRUE(status.ok());
        status = new_txn_mgr->CommitTxn(txn);
        EXPECT_TRUE(status.ok());
    }
    std::this_thread::sleep_for(5s);
    Check();
}

// TEST_P(MemoryIndexerTest, SpillLoadTest) {
//     auto indexer1 = std::make_unique<MemoryIndexer>(GetFullDataDir(), "chunk1", RowID(0U, 0U), flag_, "standard");
//     std::shared_ptr<ColumnVector> column_vector = MakeColumnVector(wiki_paragraphs_);
//     indexer1->Insert(column_vector, 0, 2);
//     indexer1->Insert(column_vector, 2, 2);
//     indexer1->Insert(column_vector, 4, 1);
//     indexer1->Dump(false, true);
//     std::unique_ptr<MemoryIndexer> loaded_indexer = std::make_unique<MemoryIndexer>(GetFullDataDir(), "chunk1", RowID(0U, 0U), flag_, "standard");
//
//     SegmentID segment_id = 0;
//     auto segment_reader = std::make_shared<InMemIndexSegmentReader>(segment_id, loaded_indexer.get());
//     for (size_t i = 0; i < expected_postings_.size(); ++i) {
//         const ExpectedPosting &expected = expected_postings_[i];
//         const std::string &term = expected.term;
//         SegmentPosting seg_posting;
//         std::shared_ptr<std::vector<SegmentPosting>> seg_postings = std::make_shared<std::vector<SegmentPosting>>();
//
//         auto ret = segment_reader->GetSegmentPosting(term, seg_posting);
//         if (ret) {
//             seg_postings->push_back(seg_posting);
//         }
//
//         auto posting_iter = std::make_unique<PostingIterator>(flag_);
//         u32 state_pool_size = 0;
//         posting_iter->Init(seg_postings, state_pool_size);
//         RowID doc_id = INVALID_ROWID;
//         for (size_t j = 0; j < expected.doc_ids.size(); ++j) {
//             doc_id = posting_iter->SeekDoc(expected.doc_ids[j]);
//             ASSERT_EQ(doc_id, expected.doc_ids[j]);
//             u32 tf = posting_iter->GetCurrentTF();
//             ASSERT_EQ(tf, expected.tfs[j]);
//         }
//     }
// }

TEST_P(MemoryIndexerTest, SLOW_SeekPosition) {
    // "A B C" repeats 7 times
    std::string paragraph(R"#(A B C A B C A B C A B C A B C A B C A B C)#");
    auto column = ColumnVector::Make(std::make_shared<DataType>(LogicalType::kVarchar));
    column->Initialize();
    Value v = Value::MakeVarchar(paragraph);
    for (size_t i = 0; i < 8192; i++) {
        column->AppendValue(v);
    }

    MemoryIndexer indexer1(GetFullDataDir(), "chunk1", RowID(0U, 0U), flag_, "standard");
    indexer1.Insert(column, 0, 8192);
    while (indexer1.GetInflightTasks() > 0) {
        std::this_thread::sleep_for(1s);
        indexer1.CommitSync();
    }

    SegmentID segment_id = 0;
    auto segment_reader = std::make_shared<InMemIndexSegmentReader>(segment_id, &indexer1);
    const std::string term("a");
    SegmentPosting seg_posting;
    auto seg_postings = std::make_shared<std::vector<SegmentPosting>>();
    auto ret = segment_reader->GetSegmentPosting(term, seg_posting);
    if (ret) {
        seg_postings->push_back(seg_posting);
    }

    auto posting_iter = std::make_unique<PostingIterator>(flag_);
    u32 state_pool_size = 0;
    posting_iter->Init(seg_postings, state_pool_size);
    RowID doc_id = INVALID_ROWID;
    std::vector<size_t> doc_ids = {0, 1, 2, 5, 127, 128, 512, 1024, 2048, 4096, 8191};
    for (size_t i = 0; i < doc_ids.size(); ++i) {
        doc_id = RowID::FromUint64(doc_ids[i]);
        doc_id = posting_iter->SeekDoc(doc_id);
        ASSERT_EQ(doc_id, doc_ids[i]);
        u32 tf = posting_iter->GetCurrentTF();
        ASSERT_EQ(tf, 7);
        pos_t target_pos = 0;
        pos_t act_pos = 0;
        for (size_t j = 0; j < 7; ++j) {
            posting_iter->SeekPosition(target_pos, act_pos);
            ASSERT_EQ(act_pos, 3 * j);
            target_pos = act_pos + 1;
        }
        posting_iter->SeekPosition(act_pos + 1, act_pos);
        ASSERT_EQ(act_pos, INVALID_POSITION);
    }
}
