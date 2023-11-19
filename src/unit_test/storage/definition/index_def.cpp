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
import ivfflat_index_def;
import hnsw_index_def;
import index_def;
import parser;

class IndexDefTest : public BaseTest {};

TEST_F(IndexDefTest, ivfflat_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("centroids_count", "100"));
    parameters.emplace_back(new InitParameter("metric", "l2"));

    auto index_def = IVFFlatIndexDef::Make(MakeShared<String>("idx1"), columns, parameters);
    std::cout << "index_def: " << index_def->ToString() << std::endl;
    for(auto parameter: parameters) {
        delete parameter;
    }

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

TEST_F(IndexDefTest, hnsw_readwrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("metric", "l2"));
    parameters.emplace_back(new InitParameter("M", "16"));
    parameters.emplace_back(new InitParameter("ef_construction", "200"));
    parameters.emplace_back(new InitParameter("ef", "200"));

    auto index_def = HnswIndexDef::Make(MakeShared<String>("idx1"), columns, parameters);
    std::cout << "index_def: " << index_def->ToString() << std::endl;

    for(auto parameter: parameters) {
        delete parameter;
    }

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