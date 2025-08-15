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

module infinity_core:ut.test_bmp_fwd;

import :ut.base_test;
import :stl;
import :sparse_util;
import :bmp_util;
import :sparse_test_util;
import :bmp_fwd;
import :virtual_store;
import :infinity_exception;
import :local_file_handle;
import third_party;
#endif

import compilation_config;

using namespace infinity;

class BMPFwdTest : public BaseTest {
protected:
    template <typename DataType, typename IdxType>
    void TestFunc(u32 block_size) {
        using BMPFwd1 = BlockFwd<DataType, IdxType, BMPOwnMem::kTrue>;
        using BMPFwd2 = BlockFwd<DataType, IdxType, BMPOwnMem::kFalse>;

        u32 nrow = 1000;
        u32 ncol = 1000;
        f32 sparsity = 0.05;

        const SparseMatrix dataset = SparseTestUtil<DataType, IdxType>::GenerateDataset(nrow, ncol, sparsity, 0.0, 10.0);

        String save_path = String(tmp_data_path()) + "/bmpfwd_test1.index";
        // String save2_path = String(tmp_data_path()) + "/bmpfwd_test2.index";

        auto check = [&](const BMPFwd1 &fwd1, const BMPFwd2 &fwd2) {
            EXPECT_EQ(fwd1.block_size(), fwd2.block_size());
            EXPECT_EQ(fwd1.block_num(), fwd2.block_num());
            for (BMPBlockID block_id = 0; block_id < BMPBlockID(fwd1.block_num()); ++block_id) {
                const auto &block_terms1 = fwd1.GetBlockTerms(block_id);
                const auto &block_terms2 = fwd2.GetBlockTerms(block_id);
                auto iter1 = block_terms1.Iter();
                auto iter2 = block_terms2.Iter();
                while (iter1.HasNext() && iter2.HasNext()) {
                    const auto [term_id, block_size, block_offsets, values] = iter1.Value();
                    const auto [term_id2, block_size2, block_offsets2, values2] = iter2.Value();
                    EXPECT_EQ(term_id, term_id2);
                    EXPECT_EQ(block_size, block_size2);
                    for (size_t i = 0; i < block_size; ++i) {
                        EXPECT_EQ(block_offsets[i], block_offsets2[i]);
                        EXPECT_EQ(values[i], values2[i]);
                    }
                    iter1.Next();
                    iter2.Next();
                }
                EXPECT_FALSE(iter1.HasNext());
                EXPECT_FALSE(iter2.HasNext());
            }
        };

        size_t filesize1 = 0;
        size_t filesize2 = 0;
        {
            size_t mem_used;
            BMPFwd1 fwd1(block_size);
            for (SparseMatrixIter iter(dataset); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                fwd1.AddDoc(vec, mem_used);
            }
            filesize1 = fwd1.GetSizeInBytes();
            auto buffer = std::make_unique<char[]>(filesize1);
            char *p = buffer.get();
            fwd1.WriteAdv(p);
            EXPECT_EQ(p - buffer.get(), filesize1);

            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            file_handle->Append(buffer.get(), filesize1);
        }
        std::unique_ptr<BMPFwd1> fwd1;
        {
            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kRead);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            auto buffer = std::make_unique<char[]>(filesize1);
            file_handle->Read(buffer.get(), filesize1);
            const char *p = buffer.get();
            fwd1 = std::make_unique<BMPFwd1>(BMPFwd1::ReadAdv(p));
            EXPECT_EQ(p - buffer.get(), filesize1);
        }
        std::unique_ptr<char[]> buffer2;
        {
            {
                char *p0 = nullptr;
                fwd1->GetSizeToPtr(p0);
                char *p1 = nullptr;
                filesize2 = p0 - p1;
            }
            buffer2 = std::make_unique<char[]>(filesize2);
            char *p2 = buffer2.get();
            fwd1->WriteToPtr(p2);
            EXPECT_EQ(p2 - buffer2.get(), filesize2);
        }
        std::unique_ptr<BMPFwd2> fwd2;
        {
            const char *p = buffer2.get();

            fwd2 = std::make_unique<BMPFwd2>(BMPFwd2::LoadFromPtr(p));
            EXPECT_EQ(p - buffer2.get(), filesize2);

            check(*fwd1, *fwd2);
        }
        {
            const char *p = buffer2.get();
            fwd1 = std::make_unique<BMPFwd1>(BMPFwd1::LoadFromPtr(p));
            EXPECT_EQ(p - buffer2.get(), filesize2);

            check(*fwd1, *fwd2);
        }
    };
};

TEST_F(BMPFwdTest, test1) {
    {
        u32 block_size = 8;
        TestFunc<f32, i32>(block_size);
    }
    {
        u32 block_size = 8;
        TestFunc<f64, i32>(block_size);
    }
}
