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

#include "sparse_benchmark_util.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

import stl;
import file_system;
import local_file_system;
import infinity_exception;
import file_system_type;
import compilation_config;
import third_party;
import profiler;
import linscan_alg;
import bm_index;

using namespace infinity;
using namespace benchmark;

const int LogInterval = 10000;

int main(int argc, char *argv[]) {
    BMPOption opt;
    try {
        opt.Parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return opt.Exit(e);
    }

    BaseProfiler profiler;
    LocalFileSystem fs;

    switch (opt.mode_type_) {
        case ModeType::kImport: {
            SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(opt.data_path_);
            profiler.Begin();
            BMIndexBuilder builder(data_mat.ncol_, opt.block_size_);
            for (SparseMatrixIter<f32, i32> iter(data_mat); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                u32 doc_id = iter.row_id();

                Vector<Pair<i32, f32>> doc;
                for (i32 term_id = 0; term_id < vec.nnz_; ++term_id) {
                    doc.emplace_back(vec.indices_[term_id], vec.data_[term_id]);
                }
                builder.AddDoc(std::move(doc));

                if (LogInterval != 0 && doc_id % LogInterval == 0) {
                    std::cout << fmt::format("Imported {} docs\n", doc_id);
                }
            }
            data_mat.Clear();
            std::cout << "Building index...\n";
            BMIndex index = std::move(builder).Build();
            std::cout << "Index built\n";
            profiler.End();

            std::cout << fmt::format("Import data time: {}\n", profiler.ElapsedToString(1000));
            auto [file_handler, status] = fs.OpenFile(opt.index_save_path_.string(), FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", opt.index_save_path_.string()));
            }
            index.Save(*file_handler);
            break;
        }
        case ModeType::kQuery: {
            i64 query_n = opt.query_n_;
            i32 thread_n = opt.thread_n_;

            auto [file_handler, status] = fs.OpenFile(opt.index_save_path_.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", opt.index_save_path_.string()));
            }
            BMIndex index = BMIndex::Load(*file_handler);

            auto [top_k, all_query_n, _1, _2] = DecodeGroundtruth(opt.groundtruth_path_, true);
            Vector<Pair<Vector<i32>, Vector<f32>>> query_result;
            {
                SparseMatrix<f32, i32> query_mat = DecodeSparseDataset(opt.query_path_);
                if (all_query_n != query_mat.nrow_) {
                    UnrecoverableError(fmt::format("Query number mismatch: {} vs {}", query_n, query_mat.nrow_));
                }
                if (query_n > all_query_n) {
                    UnrecoverableError(fmt::format("Query number: {} is larger than all query number: {}", query_n, all_query_n));
                }
                if (query_n == 0) {
                    query_n = all_query_n;
                }

                profiler.Begin();
                query_result = Search(thread_n, query_mat, top_k, query_n, [&](const SparseVecRef<f32, i32> &query, u32 topk) {
                    return index.SearchKnn(query, topk, opt.alpha_, opt.beta_);
                });
                profiler.End();
                std::cout << fmt::format("Search time: {}\n", profiler.ElapsedToString(1000));
            }
            {
                auto [_1, _2, gt_indices, gt_scores] = DecodeGroundtruth(opt.groundtruth_path_, false);
                f32 recall = CheckGroundtruth(gt_indices.get(), gt_scores.get(), query_result, top_k);
                std::cout << fmt::format("Recall: {}\n", recall);
            }
            break;
        }
    }
}