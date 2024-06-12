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

#include "CLI11.hpp"
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
import sparse_util;

using namespace infinity;

// const f32 error_bound = 1e-6;
const int log_interval = 10000;
const int query_log_interval = 100;

SparseMatrix<f32, i32> DecodeSparseDataset(const Path &data_path) {
    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(data_path.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", data_path.string(), status.message()));
    }
    return SparseMatrix<f32, i32>::Load(*file_handler);
}

void SaveSparseMatrix(const SparseMatrix<f32, i32> &mat, const Path &data_path) {
    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(data_path.string(), FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kNoLock);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", data_path.string(), status.message()));
    }
    mat.Save(*file_handler);
}

Tuple<u32, u32, UniquePtr<i32[]>, UniquePtr<f32[]>> DecodeGroundtruth(const Path &groundtruth_path, bool meta) {
    LocalFileSystem fs;
    auto [file_handler, status] = fs.OpenFile(groundtruth_path.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", groundtruth_path.string(), status.message()));
    }

    u32 top_k = 0;
    u32 query_n = 0;
    file_handler->Read(&query_n, sizeof(query_n));
    file_handler->Read(&top_k, sizeof(top_k));
    SizeT file_size = fs.GetFileSize(*file_handler);
    if (file_size != sizeof(u32) * 2 + (sizeof(i32) + sizeof(float)) * (query_n * top_k)) {
        UnrecoverableError("Invalid groundtruth file format");
    }
    if (meta) {
        return {top_k, query_n, nullptr, nullptr};
    }

    auto indices = MakeUnique<i32[]>(query_n * top_k);
    file_handler->Read(indices.get(), sizeof(i32) * query_n * top_k);
    auto scores = MakeUnique<f32[]>(query_n * top_k);
    file_handler->Read(scores.get(), sizeof(f32) * query_n * top_k);
    return {top_k, query_n, std::move(indices), std::move(scores)};
}

LinScan<f32, i32> ImportData(const SparseMatrix<f32, i32> &data_mat) {
    LinScan<f32, i32> index;
    for (SparseMatrixIter<f32, i32> iter(data_mat); iter.HasNext(); iter.Next()) {
        SparseVecRef vec = iter.val();
        u32 doc_id = iter.row_id();
        index.Insert(vec, doc_id);

        if (log_interval != 0 && doc_id % log_interval == 0) {
            std::cout << fmt::format("Inserting doc {}\n", doc_id);
        }
    }
    return index;
}

Vector<Pair<Vector<i32>, Vector<f32>>> Search(i32 thread_n,
                                              const SparseMatrix<f32, i32> &query_mat,
                                              u32 top_k,
                                              i64 query_n,
                                              std::function<Pair<Vector<i32>, Vector<f32>>(const SparseVecRef<f32, i32> &, u32)> search_fn) {
    Vector<Pair<Vector<i32>, Vector<f32>>> res(query_n);
    Atomic<i64> query_idx = 0;
    Vector<Thread> threads;
    for (i32 thread_id = 0; thread_id < thread_n; ++thread_id) {
        threads.emplace_back([&]() {
            while (true) {
                i64 query_i = query_idx.fetch_add(1);
                if (query_i >= query_n) {
                    break;
                }
                SparseVecRef query = query_mat.at(query_i);
                auto [indices, scores] = search_fn(query, top_k);
                res[query_i] = {std::move(indices), std::move(scores)};

                if (query_log_interval != 0 && query_i % query_log_interval == 0) {
                    std::cout << fmt::format("Querying doc {}\n", query_i);
                }
            }
        });
    };
    for (auto &thread : threads) {
        thread.join();
    }
    return res;
}

void PrintQuery(u32 query_id, const i32 *gt_indices, const f32 *gt_scores, u32 gt_size, const Vector<i32> &indices, const Vector<f32> &scores) {
    std::cout << fmt::format("Query {}\n", query_id);
    std::cout << "Result:\n";
    for (u32 i = 0; i < gt_size; ++i) {
        std::cout << fmt::format("{} {}, ", indices[i], scores[i]);
    }
    std::cout << "\n";
    std::cout << "Groundtruth:\n";
    for (u32 i = 0; i < gt_size; ++i) {
        std::cout << fmt::format("{} {}, ", gt_indices[i], gt_scores[i]);
    }
    std::cout << "\n";
}

f32 CheckGroundtruth(i32 *gt_indices_list, f32 *gt_score_list, const Vector<Pair<Vector<i32>, Vector<f32>>> &results, u32 top_k) {
    u32 query_n = results.size();

    SizeT recall_n = 0;
    for (u32 i = 0; i < results.size(); ++i) {
        const auto &[indices, scores] = results[i];
        const i32 *gt_indices = gt_indices_list + i * top_k;

        // const f32 *gt_score = gt_score_list + i * top_k;
        // PrintQuery(i, gt_indices, gt_score, top_k, indices, scores);
        HashSet<i32> indices_set(indices.begin(), indices.end());
        for (u32 j = 0; j < top_k; ++j) {
            if (indices_set.contains(gt_indices[j])) {
                ++recall_n;
            }
        }
    }
    f32 recall = static_cast<f32>(recall_n) / (query_n * top_k);
    return recall;
}

struct SearchKnnOption {
    u32 candidate_n_;
    i32 budget_;
};

int main(int argc, char *argv[]) {
    CLI::App app{"sparse_benchmark"};

    enum class ModeType : i8 {
        kImport,
        kQuery,
    };
    Map<String, ModeType> mode_type_map = {
        {"import", ModeType::kImport},
        {"query", ModeType::kQuery},
    };
    ModeType mode_type = ModeType::kImport;
    app.add_option("--mode", mode_type, "Mode type")->required()->transform(CLI::CheckedTransformer(mode_type_map, CLI::ignore_case));

    enum class DataSetType : u8 {
        kSmall,
        k1M,
        kFull,
    };
    Map<String, DataSetType> dataset_type_map = {
        {"small", DataSetType::kSmall},
        {"1M", DataSetType::k1M},
        {"full", DataSetType::kFull},
    };
    DataSetType dataset_type = DataSetType::kSmall;
    app.add_option("--dataset", dataset_type, "Dataset type")->required()->transform(CLI::CheckedTransformer(dataset_type_map, CLI::ignore_case));

    i64 query_n = 0;
    app.add_option("--query_n", query_n, "Test query number")->required(false)->transform(CLI::TypeValidator<i64>());

    bool bf = false;
    app.add_option("--bf", bf, "Use brute force search")->required(false)->transform(CLI::TypeValidator<bool>());

    i32 thread_n = 1;
    app.add_option("--thread_n", thread_n, "Thread number")->required(false)->transform(CLI::Range(1, 1024));

    f32 candidate_ratio = 1.5;
    app.add_option("--candidate_ratio", candidate_ratio, "Candidate ratio")->required(false)->transform(CLI::Range(1.0, 100.0));

    f32 budget_radio = 0.75;
    app.add_option("--budget_ratio", budget_radio, "Budget radio")->required(false)->transform(CLI::Range(0.0, 100.0));

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    Path dataset_dir = Path(test_data_path()) / "benchmark" / "splade";
    Path query_path = dataset_dir / "queries.dev.csr";
    Path data_path = dataset_dir;
    Path groundtruth_path = dataset_dir;
    Path index_save_path = tmp_data_path();
    switch (dataset_type) {
        case DataSetType::kSmall: {
            data_path /= "base_small.csr";
            groundtruth_path /= "base_small.dev.gt";
            index_save_path /= "small_linscan.bin";
            break;
        }
        case DataSetType::k1M: {
            data_path /= "base_1M.csr";
            groundtruth_path /= "base_1M.dev.gt";
            index_save_path /= "1M_linscan.bin";
            break;
        }
        case DataSetType::kFull: {
            data_path /= "base_full.csr";
            groundtruth_path /= "base_full.dev.gt";
            index_save_path /= "full_linscan.bin";
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unsupported dataset type: {}.", static_cast<u8>(dataset_type)));
        }
    };

    BaseProfiler profiler;

    LocalFileSystem fs;
    switch (mode_type) {
        case ModeType::kImport: {
            LinScan<f32, i32> index;

            {
                SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(data_path);
                profiler.Begin();
                index = ImportData(data_mat);
                profiler.End();
            }
            std::cout << fmt::format("Import data time: {}\n", profiler.ElapsedToString(1000));

            auto [file_handler, status] =
                fs.OpenFile(index_save_path.string(), FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", index_save_path.string(), status.message()));
            }
            index.Save(*file_handler);
            break;
        }
        case ModeType::kQuery: {
            Vector<Pair<Vector<i32>, Vector<f32>>> query_result;

            auto [top_k, all_query_n, _1, _2] = DecodeGroundtruth(groundtruth_path, true);
            {
                auto [file_handler, status] = fs.OpenFile(index_save_path.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
                if (!status.ok()) {
                    UnrecoverableError(fmt::format("Can't open file: {}, reason: {}", index_save_path.string(), status.message()));
                }
                LinScan<f32, i32> index = LinScan<f32, i32>::Load(*file_handler);

                SparseMatrix<f32, i32> query_mat = DecodeSparseDataset(query_path);
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
                    query_result = Search(thread_n, query_mat, top_k, query_n, [&](const SparseVecRef<f32, i32> &query, u32 top_k) {
                        return index.SearchBF(query, top_k);
                    });
                    profiler.End();
                } else {
                    SparseMatrix<f32, i32> data_mat = DecodeSparseDataset(data_path);

                    u64 used_budget_all = 0;
                    i32 budget = static_cast<f32>(data_mat.nnz_) * data_mat.nnz_ / (data_mat.nrow_ * data_mat.ncol_) * budget_radio;
                    u32 candidate_n = top_k * candidate_ratio;
                    SearchKnnOption option{.candidate_n_ = candidate_n, .budget_ = budget};

                    profiler.Begin();
                    query_result = Search(thread_n, query_mat, top_k, query_n, [&](const SparseVecRef<f32, i32> &query, u32 top_k) {
                        auto [candidate_indices, candidate_scores, used_budget] = index.SearchKnn(query, option.candidate_n_, option.budget_);
                        used_budget_all += used_budget;
                        return SparseVecUtil::Rerank(data_mat, query, candidate_indices, top_k);
                    });
                    profiler.End();
                    std::cout << fmt::format("avg budget: {}\n", (f32)used_budget_all / query_mat.nrow_);
                }
            }
            std::cout << fmt::format("Query data time: {}\n", profiler.ElapsedToString(1000));

            {
                auto [_1, _2, gt_indices_list, gt_score_list] = DecodeGroundtruth(groundtruth_path, false);
                f32 recall = CheckGroundtruth(gt_indices_list.get(), gt_score_list.get(), query_result, top_k);
                std::cout << fmt::format("Recall: {}\n", recall);
            }
            break;
        }
    }

    return 0;
}
