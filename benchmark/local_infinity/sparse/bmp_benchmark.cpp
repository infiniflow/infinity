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
import bmp_alg;
import bmp_util;

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
        case ModeType::kShuffle: {
            SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(opt.data_path_);
            Vector<SizeT> idx = ShuffleSparseMatrix(data_mat);
            Vector<SizeT> inv_idx(data_mat.nrow_);
            for (i64 i = 0; i < data_mat.nrow_; i++) {
                inv_idx[idx[i]] = i;
            }
            SaveSparseMatrix(data_mat, opt.data_save_path_);

            auto [topk, query_n, indices, scores] = DecodeGroundtruth(opt.groundtruth_path_, false);
            auto new_indices = MakeUniqueForOverwrite<i32[]>(query_n * topk);
            for (SizeT i = 0; i < query_n; i++) {
                for (SizeT j = 0; j < topk; j++) {
                    new_indices.get()[i * topk + j] = inv_idx[indices.get()[i * topk + j]];
                }
            }
            SaveGroundtruth(topk, query_n, new_indices.get(), scores.get(), opt.groundtruth_save_path_);
            break;
        }
        case ModeType::kOptimize: {
            auto [file_handler, status] = fs.OpenFile(opt.index_save_path_.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", opt.index_save_path_.string()));
            }
            auto inner = [&](auto &index) {
                BMPOptimizeOptions optimize_options{.topk_ = opt.topk_, .bp_reorder_ = opt.bp_reorder_};
                std::cout << "Optimizing index...\n";
                index.Optimize(optimize_options);
                std::cout << "Index built\n";

                auto [file_handler, status] = fs.OpenFile(opt.index_save_path_.string(), FileFlags::WRITE_FLAG, FileLockType::kNoLock);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Failed to open file: {}", opt.index_save_path_.string()));
                }
                index.Save(*file_handler);
            };
            switch (opt.type_) {
                case BMPCompressType::kCompressed: {
                    BMPAlg<f32, i16, BMPCompressType::kCompressed> index = BMPAlg<f32, i16, BMPCompressType::kCompressed>::Load(*file_handler);
                    inner(index);
                    break;
                }
                case BMPCompressType::kRaw: {
                    BMPAlg<f32, i16, BMPCompressType::kRaw> index = BMPAlg<f32, i16, BMPCompressType::kRaw>::Load(*file_handler);
                    inner(index);
                    break;
                }
                default: {
                    UnrecoverableError("Unknown compress type");
                }
            }
            break;
        }
        case ModeType::kImport: {
            SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(opt.data_path_);
            profiler.Begin();

            auto inner = [&](auto &index) {
                for (SparseMatrixIter<f32, i32> iter(data_mat); iter.HasNext(); iter.Next()) {
                    SparseVecRef vec = iter.val();
                    u32 doc_id = iter.row_id();
                    Vector<i16> indices(vec.nnz_);
                    for (i32 i = 0; i < vec.nnz_; i++) {
                        indices[i] = static_cast<i16>(vec.indices_[i]);
                    }
                    SparseVecRef<f32, i16> vec1(vec.nnz_, indices.data(), vec.data_);
                    index.AddDoc(vec1, doc_id);

                    if (LogInterval != 0 && doc_id % LogInterval == 0) {
                        std::cout << fmt::format("Imported {} docs\n", doc_id);
                    }
                }
                data_mat.Clear();

                BMPOptimizeOptions optimize_options{.topk_ = opt.topk_, .bp_reorder_ = opt.bp_reorder_};
                std::cout << "Optimizing index...\n";
                index.Optimize(optimize_options);
                std::cout << "Index built\n";

                profiler.End();

                std::cout << fmt::format("Import data time: {}\n", profiler.ElapsedToString(1000));
                auto [file_handler, status] =
                    fs.OpenFile(opt.index_save_path_.string(), FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kNoLock);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Failed to open file: {}", opt.index_save_path_.string()));
                }
                index.Save(*file_handler);
            };
            switch (opt.type_) {
                case BMPCompressType::kCompressed: {
                    BMPAlg<f32, i16, BMPCompressType::kCompressed> index(data_mat.ncol_, opt.block_size_);
                    inner(index);
                    break;
                }
                case BMPCompressType::kRaw: {
                    BMPAlg<f32, i16, BMPCompressType::kRaw> index(data_mat.ncol_, opt.block_size_);
                    inner(index);
                    break;
                }
                default: {
                    UnrecoverableError("Unknown compress type");
                }
            }
            break;
        }
        case ModeType::kQuery: {
            i64 query_n = opt.query_n_;
            i32 thread_n = opt.thread_n_;
            BmpSearchOptions search_options{.alpha_ = opt.alpha_, .beta_ = opt.beta_, .use_tail_ = true, .use_lock_ = false};

            auto [file_handler, status] = fs.OpenFile(opt.index_save_path_.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", opt.index_save_path_.string()));
            }

            auto inner = [&](auto &index) {
                auto [top_k, all_query_n, _1, _2] = DecodeGroundtruth(opt.groundtruth_path_, true);
                if ((int)top_k != opt.topk_) {
                    std::cout << fmt::format("Topk mismatch: {} vs {}", top_k, opt.topk_) << std::endl;
                }
                Vector<Pair<Vector<u32>, Vector<f32>>> query_result;
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
                    query_result = Search(thread_n,
                                          query_mat,
                                          opt.topk_,
                                          query_n,
                                          [&](const SparseVecRef<f32, i32> &query, u32 topk) -> Pair<Vector<u32>, Vector<f32>> {
                                              Vector<i16> indices(query.nnz_);
                                              for (i32 i = 0; i < query.nnz_; i++) {
                                                  indices[i] = static_cast<i16>(query.indices_[i]);
                                              }
                                              SparseVecRef<f32, i16> query1(query.nnz_, indices.data(), query.data_);
                                              return index.SearchKnn(query1, topk, search_options);
                                          });
                    profiler.End();
                    std::cout << fmt::format("Search time: {}\n", profiler.ElapsedToString(1000));
                }
                {
                    auto [_1, _2, gt_indices, gt_scores] = DecodeGroundtruth(opt.groundtruth_path_, false);
                    f32 recall = CheckGroundtruth(gt_indices.get(), gt_scores.get(), query_result, top_k);
                    std::cout << fmt::format("Recall: {}\n", recall);
                }
            };

            switch (opt.type_) {
                case BMPCompressType::kCompressed: {
                    auto index = BMPAlg<f32, i16, BMPCompressType::kCompressed>::Load(*file_handler);
                    inner(index);
                    break;
                }
                case BMPCompressType::kRaw: {
                    auto index = BMPAlg<f32, i16, BMPCompressType::kRaw>::Load(*file_handler);
                    inner(index);
                    break;
                }
                default: {
                    UnrecoverableError("Unknown compress type");
                }
            }
            break;
        }
        default: {
            UnrecoverableError("Unknown mode type");
        }
    }
}