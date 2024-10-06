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
import virtual_store;
import infinity_exception;
import compilation_config;
import third_party;
import profiler;
import linscan_alg;
import sparse_util;

using namespace infinity;
using namespace benchmark;

// const f32 error_bound = 1e-6;
const int LogInterval = 10000;

struct SearchKnnOption {
    u32 candidate_n_;
    i32 budget_;
};

int main(int argc, char *argv[]) {
    LinScanOption opt;
    try {
        opt.Parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return opt.Exit(e);
    }

    BaseProfiler profiler;

    switch (opt.mode_type_) {
        case ModeType::kImport: {
            SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(opt.data_path_);
            LinScan<f32, i32> index(data_mat.ncol_);
            profiler.Begin();
            for (SparseMatrixIter<f32, i32> iter(data_mat); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                u32 doc_id = iter.row_id();
                index.Insert(vec, doc_id);

                if (LogInterval != 0 && doc_id % LogInterval == 0) {
                    std::cout << fmt::format("Inserting doc {}\n", doc_id);
                }
            }
            profiler.End();
            data_mat.Clear();
            std::cout << fmt::format("Import data time: {}\n", profiler.ElapsedToString(1000));

            auto [file_handle, status] = LocalStore::Open(opt.index_save_path_.string(), FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", opt.index_save_path_.string(), status.message()));
            }
            index.Save(*file_handle);
            break;
        }
        case ModeType::kQuery: {
            Vector<Pair<Vector<u32>, Vector<f32>>> query_result;
            i64 query_n = opt.query_n_;
            i32 thread_n = opt.thread_n_;
            bool bf = opt.bf_;
            f32 candidate_ratio = opt.candidate_ratio_;
            f32 budget_ratio = opt.budget_ratio_;

            auto [top_k, all_query_n, _1, _2] = DecodeGroundtruth(opt.groundtruth_path_, true);
            {
                auto [file_handle, status] = LocalStore::Open(opt.index_save_path_.string(), FileAccessMode::kRead);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", opt.index_save_path_.string(), status.message()));
                }
                LinScan<f32, i32> index = LinScan<f32, i32>::Load(*file_handle);

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

                if (bf) {
                    profiler.Begin();
                    query_result = Search(thread_n,
                                          query_mat,
                                          top_k,
                                          query_n,
                                          [&](const SparseVecRef<f32, i32> &query, u32 top_k) -> Pair<Vector<u32>, Vector<f32>> {
                                              return index.SearchBF(query, top_k);
                                          });
                    profiler.End();
                } else {
                    SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(opt.data_path_);

                    u64 used_budget_all = 0;
                    i32 budget = static_cast<f32>(data_mat.nnz_) * data_mat.nnz_ / (data_mat.nrow_ * data_mat.ncol_) * budget_ratio;
                    u32 candidate_n = top_k * candidate_ratio;
                    SearchKnnOption option{.candidate_n_ = candidate_n, .budget_ = budget};

                    profiler.Begin();
                    query_result = Search(thread_n,
                                          query_mat,
                                          top_k,
                                          query_n,
                                          [&](const SparseVecRef<f32, i32> &query, u32 top_k) -> Pair<Vector<u32>, Vector<f32>> {
                                              auto [candidate_indices, candidate_scores, used_budget] =
                                                  index.SearchKnn(query, option.candidate_n_, option.budget_);
                                              used_budget_all += used_budget;
                                              return SparseVecUtil::Rerank(data_mat, query, candidate_indices, top_k);
                                          });
                    profiler.End();
                    std::cout << fmt::format("avg budget: {}\n", (f32)used_budget_all / query_mat.nrow_);
                }
            }
            std::cout << fmt::format("Query data time: {}\n", profiler.ElapsedToString(1000));

            {
                auto [_1, _2, gt_indices_list, gt_score_list] = DecodeGroundtruth(opt.groundtruth_path_, false);
                f32 recall = CheckGroundtruth(gt_indices_list.get(), gt_score_list.get(), query_result, top_k);
                std::cout << fmt::format("Recall: {}\n", recall);
            }
            break;
        }
        default: {
            UnrecoverableError("Unknown mode type");
        }
    }

    return 0;
}
