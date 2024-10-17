//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "gtest/gtest.h"
import base_test;
import stl;
import internal_types;
import index_defines;
import default_values;
import doc_iterator;
import and_iterator;
import or_iterator;
import and_not_iterator;
import column_index_reader;
import query_builder;
import query_node;
import search_driver;
import table_entry;
import infinity_context;
import global_resource_usage;
import third_party;
import logger;
import parse_fulltext_options;

namespace infinity {

class MockVectorDocIterator : public DocIterator {
public:
    MockVectorDocIterator(Vector<RowID> doc_ids, const String &term, const String &column) : doc_ids_(std::move(doc_ids)), idx_(0) {
        term_ = term;
        column_ = column;
    }
    ~MockVectorDocIterator() override = default;

    DocIteratorType GetType() const override { return DocIteratorType::kTermDocIterator; }
    String Name() const override { return "MockVectorDocIterator"; }

    bool Next(RowID doc_id) override {
        while (idx_ < doc_ids_.size() and doc_ids_[idx_] < doc_id) {
            ++idx_;
        }
        if (idx_ < doc_ids_.size()) {
            doc_id_ = doc_ids_[idx_];
            return true;
        }
        doc_id_ = INVALID_ROWID;
        return false;
    }

    float BM25Score() override { return 0.1f; }

    void UpdateScoreThreshold(float threshold) override {}

    u32 MatchCount() const override { return DocID() != INVALID_ROWID; }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final = true) const override {
        os << prefix;
        os << (is_final ? "└──" : "├──");
        os << "MockVectorDocIterator (term: " << term_ << ", column: " << column_ << ")" << '\n';
        int print_child_num = std::min<int>(doc_ids_.size(), 5);
        os << " (first " << print_child_num << " doc_ids_: ";
        for (int i = 0; i < print_child_num; ++i) {
            os << doc_ids_[i].ToUint64();
            if (i != print_child_num - 1) {
                os << ", ";
            }
        }
        os << ")";
        os << '\n';
    }
    Vector<RowID> doc_ids_;
    u32 idx_ = 0;
    String term_;
    String column_;
};

// treat it as TERM in optimization
struct MockQueryNode : public TermQueryNode {
    Vector<RowID> doc_ids_;
    MockQueryNode(Vector<RowID> doc_ids, const String &term, const String &column) : TermQueryNode(), doc_ids_(std::move(doc_ids)) {
        term_ = term;
        column_ = column;
    }

    void PushDownWeight(float factor) final { MultiplyWeight(factor); }
    std::unique_ptr<DocIterator> CreateSearch(const TableEntry *, const IndexReader &, EarlyTermAlgo, u32) const override {
        return MakeUnique<MockVectorDocIterator>(std::move(doc_ids_), term_, column_);
    }
    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const final {
        os << prefix;
        os << (is_final ? "└──" : "├──");
        os << "MockQueryNode (term: " << term_ << ", column: " << column_ << ")";
        int print_child_num = std::min<int>(doc_ids_.size(), 5);
        os << " (first " << print_child_num << " doc_ids_: ";
        for (int i = 0; i < print_child_num; ++i) {
            os << doc_ids_[i].ToUint64();
            if (i != print_child_num - 1) {
                os << ", ";
            }
        }
        os << ")";
        os << '\n';
    }
};

} // namespace infinity

using namespace infinity;

constexpr int DocIDMaxN = 100'000;
constexpr int TestAndVecN = 70'000;
constexpr int TestOrVecN = 10'000;
constexpr int TestNotVecN = 5'000;

// doc id: 0-100'000
// output length: in range [0, param_len]
auto get_random_doc_ids = [](std::mt19937 &rng, u32 param_len) -> Vector<RowID> {
    // generate random doc ids
    Vector<RowID> doc_ids;
    // random size
    u32 size = std::uniform_int_distribution<u32>(0, param_len)(rng);
    doc_ids.reserve(size);
    std::uniform_int_distribution<SegmentOffset> gen_id(0, DocIDMaxN);
    for (u32 i = 0; i < size; ++i) {
        doc_ids.push_back(gen_id(rng));
    }
    // sort and unique
    std::sort(doc_ids.begin(), doc_ids.end());
    doc_ids.erase(std::unique(doc_ids.begin(), doc_ids.end()), doc_ids.end());
    return doc_ids;
};

// test: flatten and, flatten or, flatten and_not
// 1. (A and B) and ((C and D) and E) -> A and B and C and D and E
// 2. (A or B) or ((C or D) or E) -> A or B or C or D or E
// 3. (((A or B) or C) and not D) and not E -> (A or B or C) and not (D, E)
// 4. (((A and B) and not C) and D) and not E -> (A and B and D) and not (C, E)

class QueryBuilderTest : public BaseTest {};

union FakeQueryBuilder {
    char empty_space[2 * sizeof(QueryBuilder)];
    QueryBuilder builder;
    FakeQueryBuilder() : empty_space{} {}
    ~FakeQueryBuilder() {}
};

// 1. (A and B) and ((C and D) and E) -> A and B and C and D and E
TEST_F(QueryBuilderTest, test_and) {
    // prepare random seed
    std::random_device rd;
    std::mt19937 rng{rd()};
    // prepare query node
    auto and_root = MakeUnique<AndQueryNode>();
    Vector<RowID> expect_result;
    {
        auto and_AB = MakeUnique<AndQueryNode>();
        Vector<RowID> vec_AB_and;
        {
            auto vec_A = get_random_doc_ids(rng, TestAndVecN);
            auto vec_B = get_random_doc_ids(rng, TestAndVecN);
            std::set_intersection(vec_A.begin(), vec_A.end(), vec_B.begin(), vec_B.end(), std::back_inserter(vec_AB_and));
            and_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_A), "A", "body"));
            and_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_B), "B", "body"));
        }
        auto and_CDE = MakeUnique<AndQueryNode>();
        Vector<RowID> vec_CDE_and;
        {
            auto and_CD = MakeUnique<AndQueryNode>();
            Vector<RowID> vec_CD_and;
            {
                auto vec_C = get_random_doc_ids(rng, TestAndVecN);
                auto vec_D = get_random_doc_ids(rng, TestAndVecN);
                std::set_intersection(vec_C.begin(), vec_C.end(), vec_D.begin(), vec_D.end(), std::back_inserter(vec_CD_and));
                and_CD->Add(MakeUnique<MockQueryNode>(std::move(vec_C), "C", "body"));
                and_CD->Add(MakeUnique<MockQueryNode>(std::move(vec_D), "D", "body"));
            }
            auto vec_E = get_random_doc_ids(rng, TestAndVecN);
            std::set_intersection(vec_CD_and.begin(), vec_CD_and.end(), vec_E.begin(), vec_E.end(), std::back_inserter(vec_CDE_and));
            and_CDE->Add(std::move(and_CD));
            and_CDE->Add(MakeUnique<MockQueryNode>(std::move(vec_E), "E", "body"));
        }
        and_root->Add(std::move(and_AB));
        and_root->Add(std::move(and_CDE));
        std::set_intersection(vec_AB_and.begin(), vec_AB_and.end(), vec_CDE_and.begin(), vec_CDE_and.end(), std::back_inserter(expect_result));
    }
    OStringStream oss;
    oss << "QueryNode tree:" << std::endl;
    static_cast<QueryNode *>(and_root.get())->PrintTree(oss);
    LOG_INFO(oss.str());
    // apply query builder
    FullTextQueryContext context;
    context.query_tree_ = std::move(and_root);
    FakeQueryBuilder fake_query_builder;
    QueryBuilder &builder = fake_query_builder.builder;
    UniquePtr<DocIterator> result_iter = builder.CreateSearch(context, EarlyTermAlgo::kNaive, MinimumShouldMatchOption{});

    oss.str("");
    oss << "DocIterator tree after optimization:" << std::endl;
    result_iter->PrintTree(oss);
    LOG_INFO(oss.str());

    // check iter tree
    // A and B and C and D and E
    auto and_iter = dynamic_cast<AndIterator *>(result_iter.get());
    ASSERT_NE(and_iter, nullptr);
    ASSERT_EQ(and_iter->GetChildren().size(), u32(5));
    // check result
    UniquePtr<DocIterator> expect_iter_result = MakeUnique<MockVectorDocIterator>(std::move(expect_result), "", "");
    for (RowID doc_id = 0; doc_id < DocIDMaxN + 1'000; ++doc_id) {
        bool ok1 = result_iter->Next(doc_id);
        bool ok2 = expect_iter_result->Next(doc_id);
        ASSERT_EQ(ok1, ok2);
        ASSERT_EQ(result_iter->DocID(), expect_iter_result->DocID());
    }
}

// 2. (A or B) or ((C or D) or E) -> A or B or C or D or E
TEST_F(QueryBuilderTest, test_or) {
    // prepare random seed
    std::random_device rd;
    std::mt19937 rng{rd()};
    // prepare query node
    auto or_root = MakeUnique<OrQueryNode>();
    Vector<RowID> expect_result;
    {
        auto or_AB = MakeUnique<OrQueryNode>();
        Vector<RowID> vec_AB_or;
        {
            auto vec_A = get_random_doc_ids(rng, TestOrVecN);
            auto vec_B = get_random_doc_ids(rng, TestOrVecN);
            std::set_union(vec_A.begin(), vec_A.end(), vec_B.begin(), vec_B.end(), std::back_inserter(vec_AB_or));
            or_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_A), "A", "body"));
            or_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_B), "B", "body"));
        }
        auto or_CDE = MakeUnique<OrQueryNode>();
        Vector<RowID> vec_CDE_or;
        {
            auto or_CD = MakeUnique<OrQueryNode>();
            Vector<RowID> vec_CD_or;
            {
                auto vec_C = get_random_doc_ids(rng, TestOrVecN);
                auto vec_D = get_random_doc_ids(rng, TestOrVecN);
                std::set_union(vec_C.begin(), vec_C.end(), vec_D.begin(), vec_D.end(), std::back_inserter(vec_CD_or));
                or_CD->Add(MakeUnique<MockQueryNode>(std::move(vec_C), "C", "body"));
                or_CD->Add(MakeUnique<MockQueryNode>(std::move(vec_D), "D", "body"));
            }
            auto vec_E = get_random_doc_ids(rng, TestOrVecN);
            std::set_union(vec_CD_or.begin(), vec_CD_or.end(), vec_E.begin(), vec_E.end(), std::back_inserter(vec_CDE_or));
            or_CDE->Add(std::move(or_CD));
            or_CDE->Add(MakeUnique<MockQueryNode>(std::move(vec_E), "E", "body"));
        }
        or_root->Add(std::move(or_AB));
        or_root->Add(std::move(or_CDE));
        std::set_union(vec_AB_or.begin(), vec_AB_or.end(), vec_CDE_or.begin(), vec_CDE_or.end(), std::back_inserter(expect_result));
    }
    OStringStream oss;
    oss << "QueryNode tree:" << std::endl;
    static_cast<QueryNode *>(or_root.get())->PrintTree(oss);
    LOG_INFO(oss.str());
    // apply query builder
    FullTextQueryContext context;
    context.query_tree_ = std::move(or_root);
    FakeQueryBuilder fake_query_builder;
    QueryBuilder &builder = fake_query_builder.builder;
    UniquePtr<DocIterator> result_iter = builder.CreateSearch(context, EarlyTermAlgo::kNaive, MinimumShouldMatchOption{});

    oss.str("");
    oss << "DocIterator tree after optimization:" << std::endl;
    result_iter->PrintTree(oss);
    LOG_INFO(oss.str());

    // check iter tree
    // A or B or C or D or E
    auto or_iter = dynamic_cast<OrIterator *>(result_iter.get());
    ASSERT_NE(or_iter, nullptr);
    ASSERT_EQ(or_iter->GetChildren().size(), u32(5));
    // check result
    UniquePtr<DocIterator> expect_iter_result = MakeUnique<MockVectorDocIterator>(std::move(expect_result), "", "");
    for (RowID doc_id = 0; doc_id < DocIDMaxN + 1'000; ++doc_id) {
        bool ok1 = result_iter->Next(doc_id);
        bool ok2 = expect_iter_result->Next(doc_id);
        ASSERT_EQ(ok1, ok2);
        ASSERT_EQ(result_iter->DocID(), expect_iter_result->DocID());
        if (!ok1 || !ok2)
            break;
    }
}

// 3. (((A or B) or C) and not D) and not E -> (A or B or C) and not (D, E)
TEST_F(QueryBuilderTest, test_and_not) {
    // prepare random seed
    std::random_device rd;
    std::mt19937 rng{rd()};
    // prepare query node
    auto and_not_root = MakeUnique<AndQueryNode>();
    Vector<RowID> expect_result;
    {
        auto and_not_ABC_D = MakeUnique<AndQueryNode>();
        Vector<RowID> vec_ABC_D_and_not;
        {
            auto or_ABC = MakeUnique<OrQueryNode>();
            Vector<RowID> vec_ABC_or;
            {
                auto or_AB = MakeUnique<OrQueryNode>();
                Vector<RowID> vec_AB_or;
                {
                    auto vec_A = get_random_doc_ids(rng, TestOrVecN);
                    auto vec_B = get_random_doc_ids(rng, TestOrVecN);
                    std::set_union(vec_A.begin(), vec_A.end(), vec_B.begin(), vec_B.end(), std::back_inserter(vec_AB_or));
                    or_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_A), "A", "body"));
                    or_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_B), "B", "body"));
                }
                auto vec_C = get_random_doc_ids(rng, TestOrVecN);
                std::set_union(vec_AB_or.begin(), vec_AB_or.end(), vec_C.begin(), vec_C.end(), std::back_inserter(vec_ABC_or));
                or_ABC->Add(std::move(or_AB));
                or_ABC->Add(MakeUnique<MockQueryNode>(std::move(vec_C), "C", "body"));
            }
            auto vec_D = get_random_doc_ids(rng, TestNotVecN);
            std::set_difference(vec_ABC_or.begin(), vec_ABC_or.end(), vec_D.begin(), vec_D.end(), std::back_inserter(vec_ABC_D_and_not));
            and_not_ABC_D->Add(std::move(or_ABC));
            auto not_D = MakeUnique<NotQueryNode>();
            not_D->Add(MakeUnique<MockQueryNode>(std::move(vec_D), "D", "body"));
            and_not_ABC_D->Add(std::move(not_D));
        }
        auto vec_E = get_random_doc_ids(rng, TestNotVecN);
        std::set_difference(vec_ABC_D_and_not.begin(), vec_ABC_D_and_not.end(), vec_E.begin(), vec_E.end(), std::back_inserter(expect_result));
        and_not_root->Add(std::move(and_not_ABC_D));
        auto not_E = MakeUnique<NotQueryNode>();
        not_E->Add(MakeUnique<MockQueryNode>(std::move(vec_E), "E", "body"));
        and_not_root->Add(std::move(not_E));
    }
    OStringStream oss;
    oss << "QueryNode tree:" << std::endl;
    static_cast<QueryNode *>(and_not_root.get())->PrintTree(oss);
    LOG_INFO(oss.str());
    // apply query builder
    FullTextQueryContext context;
    context.query_tree_ = std::move(and_not_root);
    FakeQueryBuilder fake_query_builder;
    QueryBuilder &builder = fake_query_builder.builder;
    UniquePtr<DocIterator> result_iter = builder.CreateSearch(context, EarlyTermAlgo::kNaive, MinimumShouldMatchOption{});

    oss.str("");
    oss << "DocIterator tree after optimization:" << std::endl;
    result_iter->PrintTree(oss);
    LOG_INFO(oss.str());

    // check iter tree
    // (A or B or C) and not (D, E)
    // child: 1. (A or B or C), 2. D, 3. E
    auto and_not_iter = dynamic_cast<AndNotIterator *>(result_iter.get());
    ASSERT_NE(and_not_iter, nullptr);
    ASSERT_EQ(and_not_iter->GetChildren().size(), u32(3));
    // check if child "or" has been flattened to (A or B or C)
    auto &child_or = and_not_iter->GetChildren()[0];
    auto child_or_iter = dynamic_cast<OrIterator *>(child_or.get());
    ASSERT_NE(child_or_iter, nullptr);
    ASSERT_EQ(child_or_iter->GetChildren().size(), u32(3));
    // check result
    UniquePtr<DocIterator> expect_iter_result = MakeUnique<MockVectorDocIterator>(std::move(expect_result), "", "");
    for (RowID doc_id = 0; doc_id < DocIDMaxN + 1'000; ++doc_id) {
        bool ok1 = result_iter->Next(doc_id);
        bool ok2 = expect_iter_result->Next(doc_id);
        ASSERT_EQ(ok1, ok2);
        ASSERT_EQ(result_iter->DocID(), expect_iter_result->DocID());
        if (!ok1 || !ok2)
            break;
    }
}

// 4. (((A and B) and not C) and D) and not E -> (A and B and D) and not (C, E)
TEST_F(QueryBuilderTest, test_and_not2) {
    // prepare random seed
    std::random_device rd;
    std::mt19937 rng{rd()};
    // prepare query node
    auto and_not_root = MakeUnique<AndQueryNode>();
    Vector<RowID> expect_result;
    {
        auto and_ABC_D = MakeUnique<AndQueryNode>();
        Vector<RowID> vec_ABC_D_and;
        {
            auto and_not_AB_C = MakeUnique<AndQueryNode>();
            Vector<RowID> vec_AB_C_and_not;
            {
                auto and_AB = MakeUnique<AndQueryNode>();
                Vector<RowID> vec_AB_and;
                {
                    auto vec_A = get_random_doc_ids(rng, TestAndVecN);
                    auto vec_B = get_random_doc_ids(rng, TestAndVecN);
                    std::set_intersection(vec_A.begin(), vec_A.end(), vec_B.begin(), vec_B.end(), std::back_inserter(vec_AB_and));
                    and_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_A), "A", "body"));
                    and_AB->Add(MakeUnique<MockQueryNode>(std::move(vec_B), "B", "body"));
                }
                auto vec_C = get_random_doc_ids(rng, TestNotVecN);
                std::set_difference(vec_AB_and.begin(), vec_AB_and.end(), vec_C.begin(), vec_C.end(), std::back_inserter(vec_AB_C_and_not));
                and_not_AB_C->Add(std::move(and_AB));
                auto not_C = MakeUnique<NotQueryNode>();
                not_C->Add(MakeUnique<MockQueryNode>(std::move(vec_C), "C", "body"));
                and_not_AB_C->Add(std::move(not_C));
            }
            auto vec_D = get_random_doc_ids(rng, TestAndVecN);
            std::set_intersection(vec_AB_C_and_not.begin(), vec_AB_C_and_not.end(), vec_D.begin(), vec_D.end(), std::back_inserter(vec_ABC_D_and));
            and_ABC_D->Add(std::move(and_not_AB_C));
            and_ABC_D->Add(MakeUnique<MockQueryNode>(std::move(vec_D), "D", "body"));
        }
        auto vec_E = get_random_doc_ids(rng, TestAndVecN);
        std::set_difference(vec_ABC_D_and.begin(), vec_ABC_D_and.end(), vec_E.begin(), vec_E.end(), std::back_inserter(expect_result));
        and_not_root->Add(std::move(and_ABC_D));
        auto not_E = MakeUnique<NotQueryNode>();
        not_E->Add(MakeUnique<MockQueryNode>(std::move(vec_E), "E", "body"));
        and_not_root->Add(std::move(not_E));
    }
    OStringStream oss;
    oss << "QueryNode tree:" << std::endl;
    static_cast<QueryNode *>(and_not_root.get())->PrintTree(oss);
    LOG_INFO(oss.str());
    // apply query builder
    FullTextQueryContext context;
    context.query_tree_ = std::move(and_not_root);
    FakeQueryBuilder fake_query_builder;
    QueryBuilder &builder = fake_query_builder.builder;
    UniquePtr<DocIterator> result_iter = builder.CreateSearch(context, EarlyTermAlgo::kNaive, MinimumShouldMatchOption{});

    oss.str("");
    oss << "DocIterator tree after optimization:" << std::endl;
    result_iter->PrintTree(oss);
    LOG_INFO(oss.str());

    // check iter tree
    // (A and B and D) and not (C, E)
    // child: 1. (A and B and D), 2. C, 3. E
    auto and_not_iter = dynamic_cast<AndNotIterator *>(result_iter.get());
    ASSERT_NE(and_not_iter, nullptr);
    ASSERT_EQ(and_not_iter->GetChildren().size(), u32(3));
    // check if child "and" has been flattened to (A and B and D)
    auto &child_and = and_not_iter->GetChildren()[0];
    auto child_and_iter = dynamic_cast<AndIterator *>(child_and.get());
    ASSERT_NE(child_and_iter, nullptr);
    ASSERT_EQ(child_and_iter->GetChildren().size(), u32(3));
    // check result
    UniquePtr<DocIterator> expect_iter_result = MakeUnique<MockVectorDocIterator>(std::move(expect_result), "", "");
    for (RowID doc_id = 0; doc_id < DocIDMaxN + 1'000; ++doc_id) {
        bool ok1 = result_iter->Next(doc_id);
        bool ok2 = expect_iter_result->Next(doc_id);
        ASSERT_EQ(ok1, ok2);
        ASSERT_EQ(result_iter->DocID(), expect_iter_result->DocID());
        if (!ok1 || !ok2)
            break;
    }
}
