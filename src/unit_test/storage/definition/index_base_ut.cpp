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
#include "statement/statement_common.h"
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "statement/statement_common.h"
#include "gtest/gtest.h"

module infinity_core:ut.index_base;

import :ut.base_test;
import :stl;
import :index_base;
import :index_ivf;
import :index_hnsw;
import :index_full_text;
#endif

import statement_common;

using namespace infinity;
class IndexBaseTest : public BaseTest {};

TEST_F(IndexBaseTest, ivf_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("metric", "l2"));
    parameters.emplace_back(new InitParameter("plain_storage_data_type", "float"));

    SharedPtr<IndexBase> index_base =
        IndexIVF::Make(MakeShared<String>("idx1"), MakeShared<String>("test comment"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;
    for (auto parameter : parameters) {
        delete parameter;
    }

    int32_t exp_size = index_base->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_base->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    const char *ptr_r = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexBase> index_base1 = IndexBase::ReadAdv(ptr_r, maxbytes);
    //    std::cout << "index_base1: " << index_base1->ToString() << std::endl;
    EXPECT_EQ(ptr_r - buf_beg, exp_size);
    EXPECT_NE(index_base.get(), nullptr);
    EXPECT_EQ(*index_base, *index_base1);
}

TEST_F(IndexBaseTest, hnsw_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("metric", "l2"));
    parameters.emplace_back(new InitParameter("m", "16"));
    parameters.emplace_back(new InitParameter("ef_construction", "200"));
    parameters.emplace_back(new InitParameter("encode", "plain"));

    auto index_base = IndexHnsw::Make(MakeShared<String>("idx1"), MakeShared<String>("test comment"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;

    for (auto parameter : parameters) {
        delete parameter;
    }

    int32_t exp_size = index_base->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_base->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    const char *ptr_r = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexBase> index_base1 = IndexBase::ReadAdv(ptr_r, maxbytes);
    //    std::cout << "index_base1: " << index_base1->ToString() << std::endl;
    EXPECT_EQ(ptr_r - buf_beg, exp_size);
    EXPECT_NE(index_base.get(), nullptr);
    EXPECT_EQ(*index_base, *index_base1);
}

TEST_F(IndexBaseTest, full_text_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    auto index_base = IndexFullText::Make(MakeShared<String>("idx1"), MakeShared<String>("test comment"), "tbl1_idx1", columns, parameters);
    //    std::cout << "index_base: " << index_base->ToString() << std::endl;

    for (auto parameter : parameters) {
        delete parameter;
    }

    int32_t exp_size = index_base->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_base->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    const char *ptr_r = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexBase> index_base1 = IndexBase::ReadAdv(ptr_r, maxbytes);
    //    std::cout << "index_base1: " << index_base1->ToString() << std::endl;
    EXPECT_EQ(ptr_r - buf_beg, exp_size);
    EXPECT_NE(index_base.get(), nullptr);
    EXPECT_EQ(*index_base, *index_base1);
}
