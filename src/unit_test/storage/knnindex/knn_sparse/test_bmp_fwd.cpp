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

#include "gtest/gtest.h"

import stl;
import sparse_util;
import bmp_util;
import base_test;
import sparse_test_util;
import bmp_fwd;
import compilation_config;
import virtual_store;
import infinity_exception;
import local_file_handle;
import third_party;

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
        String save2_path = String(tmp_data_path()) + "/bmpfwd_test2.index";

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
                    for (SizeT i = 0; i < block_size; ++i) {
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

        SizeT filesize1 = 0;
        SizeT filesize2 = 0;
        {
            SizeT mem_used;
            BMPFwd1 fwd1(block_size);
            for (SparseMatrixIter iter(dataset); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                fwd1.AddDoc(vec, mem_used);
            }
            filesize1 = fwd1.GetSizeInBytes();
            auto buffer = MakeUnique<char[]>(filesize1);
            char *p = buffer.get();
            fwd1.WriteAdv(p);
            EXPECT_EQ(p - buffer.get(), filesize1);

            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            file_handle->Append(buffer.get(), filesize1);
        }
        {
            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kRead);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            auto buffer = MakeUnique<char[]>(filesize1);
            file_handle->Read(buffer.get(), filesize1);
            const char *p = buffer.get();
            [[maybe_unused]] auto fwd1 = BMPFwd1::ReadAdv(p);
            EXPECT_EQ(p - buffer.get(), filesize1);

            auto [file_handle2, status2] = VirtualStore::Open(save2_path, FileAccessMode::kWrite);
            if (!status2.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save2_path));
            }

            {
                char *p0 = nullptr;
                fwd1.GetSizeToPtr(p0);
                char *p1 = nullptr;
                filesize2 = p0 - p1;
            }
            auto buffer2 = MakeUnique<char[]>(filesize2);
            char *p2 = buffer2.get();
            fwd1.WriteToPtr(p2);
            EXPECT_EQ(p2 - buffer2.get(), filesize2);
            file_handle2->Append(buffer2.get(), filesize2);

// #define USE_MMAP
#ifdef USE_MMAP
            unsigned char *data_ptr;
            int ret = VirtualStore::MmapFile(save2_path, data_ptr, filesize2);
            if (ret < 0) {
                UnrecoverableError("mmap failed");
            }
            p = reinterpret_cast<const char *>(data_ptr);
#else
            std::tie(file_handle2, status2) = VirtualStore::Open(save2_path, FileAccessMode::kRead);
            if (!status2.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save2_path));
            }
            file_handle2->Read(buffer2.get(), filesize2);
            p = buffer2.get();
#endif
            const char *start_p = p;
            auto fwd2 = BMPFwd2::LoadFromPtr(p);
            EXPECT_EQ(p - start_p, filesize2);

            check(fwd1, fwd2);

#ifdef USE_MMAP
            VirtualStore::MunmapFile(save2_path);
#endif
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
