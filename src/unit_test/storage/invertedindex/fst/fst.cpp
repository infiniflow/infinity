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

#include "unit_test/base_test.h"
#include <filesystem>
import stl;
import fst;

using namespace infinity;

class FstTest : public BaseTest {
public:
    Vector<Pair<String, u64>> months = {{"January", 1},
                                        {"February", 2},
                                        {"March", 3},
                                        {"April", 4},
                                        {"May", 5},
                                        {"June", 6},
                                        {"July", 7},
                                        {"August", 8},
                                        {"September", 9},
                                        {"October", 10},
                                        {"November", 11},
                                        {"December", 12}};

protected:
    void SetUp() {
        std::sort(months.begin(), months.end(), [](const Pair<String, u64> &a, const Pair<String, u64> &b) { return a.first < b.first; });
    }
};

TEST_F(FstTest, BuildMem) {
    Vector<u8> buffer;
    BufferWriter wtr(buffer);
    FstBuilder builder(wtr);
    for (auto &month : months) {
        builder.Insert((u8 *)month.first.c_str(), month.first.length(), month.second);
    }
    builder.Finish();
    u64 written = builder.BytesWritten();
    EXPECT_EQ(buffer.size(), written + 4);
}

TEST_F(FstTest, BuildFile) {
    String fst_path = String(GetFullTmpDir()) + "/months.fst";
    std::ofstream ofs(fst_path, std::ios::binary | std::ios::trunc);
    OstreamWriter wtr(ofs);
    FstBuilder builder(wtr);
    for (auto &month : months) {
        builder.Insert((u8 *)month.first.c_str(), month.first.length(), month.second);
    }
    builder.Finish();
    u64 written = builder.BytesWritten();
    ofs.close();
    EXPECT_EQ(std::filesystem::file_size(fst_path), written + 4);
}

TEST_F(FstTest, Get) {
    Vector<u8> buffer;
    BufferWriter wtr(buffer);
    FstBuilder builder(wtr);
    for (auto &month : months) {
        builder.Insert((u8 *)month.first.c_str(), month.first.length(), month.second);
    }
    builder.Finish();

    Fst f(buffer.data(), buffer.size());
    f.Verify();
    u64 val;
    for (auto &month : months) {
        bool found = f.Get((u8 *)month.first.c_str(), month.first.length(), val);
        EXPECT_TRUE(found);
        EXPECT_EQ(val, month.second);
    }
}

TEST_F(FstTest, Iterate) {
    Vector<u8> buffer;
    BufferWriter wtr(buffer);
    FstBuilder builder(wtr);
    for (auto &month : months) {
        builder.Insert((u8 *)month.first.c_str(), month.first.length(), month.second);
    }
    builder.Finish();

    Fst f(buffer.data(), buffer.size());
    f.Verify();
    FstStream s(f);
    Vector<u8> key;
    u64 val;
    SizeT i = 0;
    while (s.Next(key, val)) {
        String name((char *)key.data(), key.size());
        EXPECT_EQ(name, months[i].first);
        EXPECT_EQ(val, months[i].second);
        i++;
    }

    SizeT b1_num = 3, b2_num = 7;
    Bound b1(Bound::kIncluded, (u8 *)months[b1_num].first.data(), months[b1_num].first.length());
    Bound b2(Bound::kExcluded, (u8 *)months[b2_num].first.data(), months[b2_num].first.length());
    s.Reset(b1, b2);
    i = b1_num;
    while (s.Next(key, val)) {
        String name((char *)key.data(), key.size());
        EXPECT_EQ(name, months[i].first);
        EXPECT_EQ(val, months[i].second);
        i++;
    }
    EXPECT_EQ(i, b2_num);
}
