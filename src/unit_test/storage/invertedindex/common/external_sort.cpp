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
#include <cassert>
#include <filesystem>
#include <iostream>

import stl;
import third_party;
import external_sort_merger;

using namespace infinity;

template <class T>
Vector<char> RandStr(T key) {
    u32 len = rand() % 100 + sizeof(T);
    while (len == sizeof(T))
        len = rand() % 100 + sizeof(T);

    Vector<char> str;
    str.reserve(len);
    for (u32 i = 0; i < len; ++i)
        str.push_back('a' + rand() % 26);

    *(T *)str.data() = key;
    return str;
}

class ExternalSortTest : public BaseTest {
public:
    ExternalSortTest() {}
    ~ExternalSortTest() {}
    void SetUp() override {}
    void TearDown() override {}

protected:
    template <class KeyType, class LenType>
    void CheckMerger(const u64 SIZE, u32 bs = 100000000) {
        std::filesystem::remove("./tt");

        Vector<char> str;
        FILE *f = fopen("./tt", "w+");
        fwrite(&SIZE, sizeof(u64), 1, f);

        u32 run_num = rand() % 300;
        while (run_num < 100 || SIZE % run_num != 0)
            run_num = rand() % 300;

        // run_num = 800;
        for (u32 i = 0; i < run_num; ++i) {
            u64 pos = ftell(f);
            fseek(f, 2 * sizeof(u32) + sizeof(u64), SEEK_CUR);
            u32 s = 0;
            for (u32 j = 0; j < SIZE / run_num; ++j) {
                str = RandStr<KeyType>(i * SIZE / run_num + j);
                LenType len = str.size();
                fwrite(&len, sizeof(LenType), 1, f);
                fwrite(str.data(), len, 1, f);
                s += len + sizeof(LenType);

                // cout<<"\rAdd data: "<<(double)(i*SIZE/run_num+j)/SIZE*100.<<"%"<<std::Flush;
            }
            u64 next_run_pos = ftell(f);
            fseek(f, pos, SEEK_SET);
            fwrite(&s, sizeof(u32), 1, f);
            s = SIZE / run_num;
            fwrite(&s, sizeof(u32), 1, f);
            fwrite(&next_run_pos, sizeof(u64), 1, f);
            fseek(f, 0, SEEK_END);
        }
        fclose(f);

        SortMerger<KeyType, LenType> merger("./tt", run_num, bs, 2);
        merger.Run();

        f = fopen("./tt", "r");
        u64 count = 0;
        fread(&count, sizeof(u64), 1, f);
        EXPECT_EQ(count, SIZE);
        for (u32 i = 0; i < count; ++i) {
            LenType len = 0;
            fread(&len, sizeof(LenType), 1, f);
            char *buf = new char[len];
            fread(buf, len, 1, f);
            EXPECT_EQ(*(KeyType *)buf, i);
            if (*(KeyType *)buf != i) {
                std::cout << "\nERROR: " << *(KeyType *)buf << ":" << i << std::endl;
                break;
            }
            delete[] buf;
        }
        std::filesystem::remove("./tt");
    }
};

TEST_F(ExternalSortTest, test1) {
    CheckMerger<u32, u8>(1000, 100000);
    CheckMerger<u32, u8>(10000, 1000000);
}
