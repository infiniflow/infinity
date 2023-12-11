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

#include "statement/statement_common.h"
#include "unit_test/base_test.h"

import stl;
import index_base;
import index_ivfflat;
import index_hnsw;
import index_base;
import index_full_text;
import index_def;
import parser;

class IndexDefTest : public BaseTest {};

TEST_F(IndexDefTest, ivfflat_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("centroids_count", "100"));
    parameters.emplace_back(new InitParameter("metric", "l2"));

    auto index_base = IndexIVFFlat::Make("idx1", columns, parameters);
    std::cout << "index_base: " << index_base->ToString() << std::endl;
    for (auto parameter : parameters) {
        delete parameter;
    }

    int32_t exp_size = index_base->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_base->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    ptr = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexBase> index_base1 = IndexBase::ReadAdv(ptr, maxbytes);
    std::cout << "index_base1: " << index_base1->ToString() << std::endl;
    EXPECT_EQ(ptr - buf_beg, exp_size);
    EXPECT_NE(index_base.get(), nullptr);
    EXPECT_EQ(*index_base, *index_base1);
}

TEST_F(IndexDefTest, hnsw_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("metric", "l2"));
    parameters.emplace_back(new InitParameter("M", "16"));
    parameters.emplace_back(new InitParameter("ef_construction", "200"));
    parameters.emplace_back(new InitParameter("ef", "200"));
    parameters.emplace_back(new InitParameter("encode", "plain"));

    auto index_base = IndexHnsw::Make("idx1", columns, parameters);
    std::cout << "index_base: " << index_base->ToString() << std::endl;

    for (auto parameter : parameters) {
        delete parameter;
    }

    int32_t exp_size = index_base->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_base->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    ptr = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexBase> index_base1 = IndexBase::ReadAdv(ptr, maxbytes);
    std::cout << "index_base1: " << index_base1->ToString() << std::endl;
    EXPECT_EQ(ptr - buf_beg, exp_size);
    EXPECT_NE(index_base.get(), nullptr);
    EXPECT_EQ(*index_base, *index_base1);
}

TEST_F(IndexDefTest, full_text_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("analyzer", "jieba"));

    auto index_base = IndexFullText::Make("idx1", columns, parameters);
    std::cout << "index_base: " << index_base->ToString() << std::endl;

    for (auto parameter : parameters) {
        delete parameter;
    }

    int32_t exp_size = index_base->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_base->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    ptr = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexBase> index_base1 = IndexBase::ReadAdv(ptr, maxbytes);
    std::cout << "index_base1: " << index_base1->ToString() << std::endl;
    EXPECT_EQ(ptr - buf_beg, exp_size);
    EXPECT_NE(index_base.get(), nullptr);
    EXPECT_EQ(*index_base, *index_base1);
}

TEST_F(IndexDefTest, index_def) {
    using namespace infinity;

    Vector<String> columns1{"col1", "col2"};
    Vector<InitParameter *> parameters1;
    parameters1.emplace_back(new InitParameter("centroids_count", "100"));
    parameters1.emplace_back(new InitParameter("metric", "l2"));

    auto index_base_ivf = IndexIVFFlat::Make("name1", columns1, parameters1);
    for(auto* init_parameter: parameters1) {
        delete init_parameter;
    }

    Vector<String> columns2{"col3"};
    Vector<InitParameter *> parameters2;
    parameters2.emplace_back(new InitParameter("metric", "l2"));
    parameters2.emplace_back(new InitParameter("encode", "plain"));
    parameters2.emplace_back(new InitParameter("M", "16"));
    parameters2.emplace_back(new InitParameter("ef_construction", "200"));
    parameters2.emplace_back(new InitParameter("ef", "200"));

    auto index_base_hnsw = IndexHnsw::Make("name2", columns2, parameters2);
    for(auto* init_parameter: parameters2) {
        delete init_parameter;
    }

    Vector<String> columns3{"col4", "col5"};
    Vector<InitParameter *> parameters3;
    parameters3.emplace_back(new InitParameter("analyzer", "jieba"));

    auto index_base_ft = IndexFullText::Make("name3", columns3, parameters3);
    for(auto* init_parameter: parameters3) {
        delete init_parameter;
    }

    Vector<String> columns4{"col6", "col7"};
    Vector<InitParameter *> parameters4;

    auto index_base_ft1 = IndexFullText::Make("name4", columns4, parameters4);
    for (auto parameter : parameters4) {
        delete parameter;
    }

    auto index_def = MakeShared<IndexDef>(MakeShared<String>("index1"));
    index_def->index_array_.emplace_back(index_base_ivf);
    index_def->index_array_.emplace_back(index_base_hnsw);
    index_def->index_array_.emplace_back(index_base_ft);
    index_def->index_array_.emplace_back(index_base_ft1);

    EXPECT_EQ(*index_def->index_name_, "index1");
    std::cout << "index_def: " << index_def->ToString() << std::endl;

    int32_t exp_size = index_def->GetSizeInBytes();
    Vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_def->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    ptr = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexDef> index_def1 = IndexDef::ReadAdv(ptr, maxbytes);
    std::cout << "index_def1: " << index_def1->ToString() << std::endl;
    EXPECT_EQ(ptr - buf_beg, exp_size);
    EXPECT_NE(index_def1.get(), nullptr);
    EXPECT_EQ(*index_def, *index_def1);
}
