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
import bmp_ivt;
import bmp_fwd;
import compilation_config;
import virtual_store;
import infinity_exception;
import local_file_handle;
import third_party;

using namespace infinity;

class BMPIvtTest : public BaseTest {
protected:
    template <typename DataType, typename IdxType, BMPCompressType CompressType>
    void TestFunc(u32 block_size) {
        using BMPIvt1 = BMPIvt<DataType, CompressType, BMPOwnMem::kTrue>;
        using BMPIvt2 = BMPIvt<DataType, CompressType, BMPOwnMem::kFalse>;

        u32 nrow = 1000;
        u32 ncol = 1000;
        f32 sparsity = 0.05;

        const SparseMatrix dataset = SparseTestUtil<DataType, IdxType>::GenerateDataset(nrow, ncol, sparsity, 0.0, 10.0);

        String save_path = String(tmp_data_path()) + "/bmpivt_test1.index";
        // String save2_path = String(tmp_data_path()) + "/bmpivt_test2.index";

        auto check = [&](const BMPIvt1 &ivt1, const BMPIvt2 &ivt2, BMPBlockID block_num) {
            for (BMPBlockID block_id = 0; block_id < block_num; ++block_id) {
                const auto &postings1 = ivt1.GetPostings(block_id);
                const auto &postings2 = ivt2.GetPostings(block_id);
                EXPECT_EQ(postings1.kth_, postings2.kth_);
                EXPECT_EQ(postings1.kth_score_, postings2.kth_score_);
                const auto &block_data1 = postings1.data();
                const auto &block_data2 = postings2.data();
                if constexpr (CompressType == BMPCompressType::kCompressed) {
                    EXPECT_EQ(block_data1.block_num(), block_data2.block_num());
                    for (SizeT i = 0; i < block_data1.block_num(); ++i) {
                        EXPECT_EQ(block_data1.block_ids()[i], block_data2.block_ids()[i]);
                        EXPECT_EQ(block_data1.max_scores()[i], block_data2.max_scores()[i]);
                    }
                } else {
                    EXPECT_EQ(block_data1.block_num(), block_data2.block_num());
                    for (BMPBlockID block_id = 0; block_id < BMPBlockID(block_data1.block_num()); ++block_id) {
                        EXPECT_EQ(block_data1.max_scores()[block_id], block_data2.max_scores()[block_id]);
                    }
                }
            }
        };

        SizeT filesize1 = 0;
        SizeT filesize2 = 0;
        BMPBlockID block_id = 0;
        {
            SizeT mem_used;
            BMPIvt1 ivt1(ncol);
            auto tail_fwd = MakeUnique<TailFwd<DataType, IdxType>>(block_size);
            for (SparseMatrixIter iter(dataset); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                SizeT tail_size = tail_fwd->AddDoc(vec);
                if (tail_size >= block_size) {
                    auto tail_fwd1 = MakeUnique<TailFwd<DataType, IdxType>>(block_size);
                    std::swap(tail_fwd1, tail_fwd);
                    const auto &tail_terms = tail_fwd1->GetTailTerms();
                    ivt1.AddBlock(block_id, tail_terms, mem_used);
                }
                ++block_id;
            }

            filesize1 = ivt1.GetSizeInBytes();
            auto buffer = MakeUnique<char[]>(filesize1);
            char *p = buffer.get();
            ivt1.WriteAdv(p);
            EXPECT_EQ(p - buffer.get(), filesize1);

            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            file_handle->Append(buffer.get(), filesize1);
        }
        UniquePtr<BMPIvt1> ivt1;
        {
            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kRead);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            auto buffer = MakeUnique<char[]>(filesize1);
            file_handle->Read(buffer.get(), filesize1);
            const char *p = buffer.get();
            ivt1 = MakeUnique<BMPIvt1>(BMPIvt1::ReadAdv(p));
            EXPECT_EQ(p - buffer.get(), filesize1);
        }
        UniquePtr<char[]> buffer2;
        {
            {
                char *p0 = nullptr;
                ivt1->GetSizeToPtr(p0);
                char *p1 = nullptr;
                filesize2 = p0 - p1;
            }
            buffer2 = MakeUnique<char[]>(filesize2);
            char *p2 = buffer2.get();
            ivt1->WriteToPtr(p2);
            EXPECT_EQ(p2 - buffer2.get(), filesize2);
        }
        UniquePtr<BMPIvt2> ivt2;
        {
            const char *p2 = buffer2.get();

            ivt2 = MakeUnique<BMPIvt2>(BMPIvt2::ReadFromPtr(p2));
            EXPECT_EQ(p2 - buffer2.get(), filesize2);

            check(*ivt1, *ivt2, block_id);
        }
        {
            const char *p = buffer2.get();
            ivt1 = MakeUnique<BMPIvt1>(BMPIvt1::ReadFromPtr(p));
            EXPECT_EQ(p - buffer2.get(), filesize2);

            check(*ivt1, *ivt2, block_id);
        }
    };
};

TEST_F(BMPIvtTest, test1) {
    {
        u32 block_size = 8;
        TestFunc<f32, i32, BMPCompressType::kCompressed>(block_size);
    }
    {
        u32 block_size = 8;
        TestFunc<f32, i32, BMPCompressType::kRaw>(block_size);
    }
    {
        u32 block_size = 8;
        TestFunc<f64, i32, BMPCompressType::kCompressed>(block_size);
    }
}
