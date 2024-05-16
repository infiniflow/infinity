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

#include "unit_test/base_test.h"
#include <random>

import stl;
import index_defines;
import default_values;
import doc_iterator;
import and_iterator;
import or_iterator;
import and_not_iterator;
import internal_types;

using namespace infinity;

class MockVectorDocIterator : public DocIterator {
private:
    u32 GetDF() const override { return 0; }

public:
    MockVectorDocIterator(Vector<RowID> doc_ids) : doc_ids_(std::move(doc_ids)) { DoSeek(0); }

    ~MockVectorDocIterator() override = default;

    void DoSeek(RowID doc_id) override {
        while (idx_ < doc_ids_.size() and doc_ids_[idx_] < doc_id) {
            ++idx_;
        }
        doc_id_ = idx_ < doc_ids_.size() ? doc_ids_[idx_] : INVALID_ROWID;
    }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final = true) const override {}

    Vector<RowID> doc_ids_;
    u32 idx_ = 0;
};

// doc id: 0-100'000
// output length: in range [0, param_len]
auto get_random_doc_ids = [](std::mt19937 &rng, u32 param_len) -> Vector<RowID> {
    // generate random doc ids
    Vector<RowID> doc_ids;
    // random size
    u32 size = std::uniform_int_distribution<u32>(0, param_len)(rng);
    std::uniform_int_distribution<SegmentOffset> gen_id(0, 100'000);
    for (u32 i = 0; i < size; ++i) {
        doc_ids.push_back(RowID(0, gen_id(rng)));
    }
    // sort and unique
    std::sort(doc_ids.begin(), doc_ids.end());
    doc_ids.erase(std::unique(doc_ids.begin(), doc_ids.end()), doc_ids.end());
    return doc_ids;
};

class SearchIteratorTest2 : public BaseTest {
public:
    Vector<RowID> doc_ids_A, doc_ids_B, doc_ids_and, doc_ids_or, doc_ids_and_not;

    SearchIteratorTest2() {}

    void SetUp() override {
        // prepare random seed
        std::random_device rd;
        std::mt19937 rng(rd());
        doc_ids_A = get_random_doc_ids(rng, 10'000);
        doc_ids_B = get_random_doc_ids(rng, 10'000);
        doc_ids_and.clear();
        doc_ids_or.clear();
        doc_ids_and_not.clear();
        std::set_intersection(doc_ids_A.begin(), doc_ids_A.end(), doc_ids_B.begin(), doc_ids_B.end(), std::back_inserter(doc_ids_and));
        std::set_union(doc_ids_A.begin(), doc_ids_A.end(), doc_ids_B.begin(), doc_ids_B.end(), std::back_inserter(doc_ids_or));
        std::set_difference(doc_ids_A.begin(), doc_ids_A.end(), doc_ids_B.begin(), doc_ids_B.end(), std::back_inserter(doc_ids_and_not));
    }

    void TearDown() override {}
};

TEST_F(SearchIteratorTest2, test_and) {
    Vector<UniquePtr<DocIterator>> iterators(2);
    iterators[0] = MakeUnique<MockVectorDocIterator>(doc_ids_A);
    iterators[1] = MakeUnique<MockVectorDocIterator>(doc_ids_B);
    AndIterator and_it(std::move(iterators));
    MockVectorDocIterator expect_res(doc_ids_and);
    for (RowID doc_id = 0; doc_id <= RowID(0, 100'000); ++doc_id) {
        and_it.Seek(doc_id);
        expect_res.Seek(doc_id);
        EXPECT_EQ(and_it.Doc(), expect_res.Doc());
    }
}

TEST_F(SearchIteratorTest2, test_or) {
    Vector<UniquePtr<DocIterator>> iterators(2);
    iterators[0] = MakeUnique<MockVectorDocIterator>(doc_ids_A);
    iterators[1] = MakeUnique<MockVectorDocIterator>(doc_ids_B);
    OrIterator or_it(std::move(iterators));
    MockVectorDocIterator expect_res(doc_ids_or);
    for (RowID doc_id = 0; doc_id <= 100'000; ++doc_id) {
        or_it.Seek(doc_id);
        expect_res.Seek(doc_id);
        EXPECT_EQ(or_it.Doc(), expect_res.Doc());
    }
}

TEST_F(SearchIteratorTest2, test_and_not) {
    Vector<UniquePtr<DocIterator>> iterators(2);
    iterators[0] = MakeUnique<MockVectorDocIterator>(doc_ids_A);
    iterators[1] = MakeUnique<MockVectorDocIterator>(doc_ids_B);
    AndNotIterator and_not_it(std::move(iterators));
    MockVectorDocIterator expect_res(doc_ids_and_not);
    for (RowID doc_id = 0; doc_id <= 100'000; ++doc_id) {
        and_not_it.Seek(doc_id);
        expect_res.Seek(doc_id);
        EXPECT_EQ(and_not_it.Doc(), expect_res.Doc());
    }
}

#define TestN 5

class SearchIteratorTestN : public BaseTest {
public:
    Vector<RowID> doc_ids[TestN], doc_ids_and, doc_ids_or, doc_ids_and_not;

    SearchIteratorTestN() {}

    void SetUp() override {
        // prepare random seed
        std::random_device rd;
        std::mt19937 rng(rd());
        for (int i = 0; i < TestN; ++i) {
            doc_ids[i] = get_random_doc_ids(rng, 30'000);
        }
        // calculate and, or, and_not
        doc_ids_and = doc_ids[0];
        doc_ids_or = doc_ids[0];
        doc_ids_and_not = doc_ids[0];
        for (int i = 1; i < TestN; ++i) {
            Vector<RowID> new_and, new_or, new_and_not;
            std::set_intersection(doc_ids_and.begin(), doc_ids_and.end(), doc_ids[i].begin(), doc_ids[i].end(), std::back_inserter(new_and));
            std::set_union(doc_ids_or.begin(), doc_ids_or.end(), doc_ids[i].begin(), doc_ids[i].end(), std::back_inserter(new_or));
            std::set_difference(doc_ids_and_not.begin(),
                                doc_ids_and_not.end(),
                                doc_ids[i].begin(),
                                doc_ids[i].end(),
                                std::back_inserter(new_and_not));
            doc_ids_and = std::move(new_and);
            doc_ids_or = std::move(new_or);
            doc_ids_and_not = std::move(new_and_not);
        }
    }

    void TearDown() override {}
};

TEST_F(SearchIteratorTestN, test_and) {
    Vector<UniquePtr<DocIterator>> iterators(TestN);
    for (int i = 0; i < TestN; ++i) {
        iterators[i] = MakeUnique<MockVectorDocIterator>(doc_ids[i]);
    }
    AndIterator and_it(std::move(iterators));
    MockVectorDocIterator expect_res(doc_ids_and);
    for (RowID doc_id = 0; doc_id <= 100'000; ++doc_id) {
        and_it.Seek(doc_id);
        expect_res.Seek(doc_id);
        EXPECT_EQ(and_it.Doc(), expect_res.Doc());
    }
}

TEST_F(SearchIteratorTestN, test_or) {
    Vector<UniquePtr<DocIterator>> iterators(TestN);
    for (int i = 0; i < TestN; ++i) {
        iterators[i] = MakeUnique<MockVectorDocIterator>(doc_ids[i]);
    }
    OrIterator or_it(std::move(iterators));
    MockVectorDocIterator expect_res(doc_ids_or);
    for (RowID doc_id = 0; doc_id <= 100'000; ++doc_id) {
        or_it.Seek(doc_id);
        expect_res.Seek(doc_id);
        EXPECT_EQ(or_it.Doc(), expect_res.Doc());
    }
}

TEST_F(SearchIteratorTestN, test_and_not) {
    Vector<UniquePtr<DocIterator>> iterators(TestN);
    for (int i = 0; i < TestN; ++i) {
        iterators[i] = MakeUnique<MockVectorDocIterator>(doc_ids[i]);
    }
    AndNotIterator and_not_it(std::move(iterators));
    MockVectorDocIterator expect_res(doc_ids_and_not);
    for (RowID doc_id = 0; doc_id <= 100'000; ++doc_id) {
        and_not_it.Seek(doc_id);
        expect_res.Seek(doc_id);
        EXPECT_EQ(and_not_it.Doc(), expect_res.Doc());
    }
}
