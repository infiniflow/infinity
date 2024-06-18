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

#pragma once

#include "CLI11.hpp"

import stl;
import file_system;
import local_file_system;
import infinity_exception;
import file_system_type;
import third_party;
import infinity_exception;
import sparse_util;
import compilation_config;

using namespace infinity;

namespace benchmark {

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

const int kQueryLogInterval = 100;

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

                if (kQueryLogInterval != 0 && query_i % kQueryLogInterval == 0) {
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

enum class ModeType : i8 {
    kImport,
    kQuery,
};

enum class DataSetType : u8 {
    kSmall,
    k1M,
    kFull,
};

struct BenchmarkOption {
public:
    BenchmarkOption() : app_("sparse benchmark") {}

    void Parse(int argc, char *argv[]) {
        Map<String, ModeType> mode_type_map = {
            {"import", ModeType::kImport},
            {"query", ModeType::kQuery},
        };
        Map<String, DataSetType> dataset_type_map = {
            {"small", DataSetType::kSmall},
            {"1M", DataSetType::k1M},
            {"full", DataSetType::kFull},
        };

        app_.add_option("--mode", mode_type_, "Mode type")->required()->transform(CLI::CheckedTransformer(mode_type_map, CLI::ignore_case));
        app_.add_option("--dataset", dataset_type_, "Dataset type")
            ->required()
            ->transform(CLI::CheckedTransformer(dataset_type_map, CLI::ignore_case));
        app_.add_option("--query_n", query_n_, "Test query number")->required(false)->transform(CLI::TypeValidator<i64>());
        app_.add_option("--thread_n", thread_n_, "Thread number")->required(false)->transform(CLI::Range(1, 1024));
        ParseInner(app_);
        app_.parse(argc, argv);

        String index_name = IndexName();
        Path dataset_dir = Path(test_data_path()) / "benchmark" / "splade";
        query_path_ = dataset_dir / "queries.dev.csr";
        data_path_ = dataset_dir;
        groundtruth_path_ = dataset_dir;
        index_save_path_ = tmp_data_path();
        switch (dataset_type_) {
            case DataSetType::kSmall: {
                data_path_ /= "base_small.csr";
                groundtruth_path_ /= "base_small.dev.gt";
                index_save_path_ /= fmt::format("small_{}.bin", index_name);
                break;
            }
            case DataSetType::k1M: {
                data_path_ /= "base_1M.csr";
                groundtruth_path_ /= "base_1M.dev.gt";
                index_save_path_ /= fmt::format("1M_{}.bin", index_name);
                break;
            }
            case DataSetType::kFull: {
                data_path_ /= "base_full.csr";
                groundtruth_path_ /= "base_full.dev.gt";
                index_save_path_ /= fmt::format("full_{}.bin", index_name);
                break;
            }
            default: {
                UnrecoverableError(fmt::format("Unsupported dataset type: {}.", static_cast<u8>(dataset_type_)));
            }
        };
    }

    int Exit(const CLI::ParseError &e) { return app_.exit(e); }

    virtual void ParseInner(CLI::App &app_) = 0;

    virtual String IndexName() const = 0;

public:
    ModeType mode_type_ = ModeType::kImport;
    DataSetType dataset_type_ = DataSetType::kSmall;
    i64 query_n_ = 0;
    i32 thread_n_ = 1;

    Path data_path_;
    Path query_path_;
    Path groundtruth_path_;
    Path index_save_path_;

protected:
    CLI::App app_;
};

struct LinScanOption : public BenchmarkOption {
public:
    void ParseInner(CLI::App &app_) override {
        app_.add_option("--bf", bf_, "Use brute force search")->required(false)->transform(CLI::TypeValidator<bool>());
        app_.add_option("--candidate_ratio", candidate_ratio_, "Candidate ratio")->required(false)->transform(CLI::Range(1.0, 100.0));
        app_.add_option("--budget_ratio", budget_ratio_, "Budget radio")->required(false)->transform(CLI::Range(0.0, 100.0));
    }

    String IndexName() const override { return "linscan"; }

public:
    bool bf_ = false;
    f32 candidate_ratio_ = 1.5;
    f32 budget_ratio_ = 0.75;
};

struct BMPOption : public BenchmarkOption {
public:
    void ParseInner(CLI::App &app_) override {
        app_.add_option("--topk", topk_, "Topk")->required(false)->transform(CLI::Range(1, 1024));
        app_.add_option("--block_size", block_size_, "Block size")->required(false)->transform(CLI::Range(1, 1024));
        app_.add_option("--alpha", alpha_, "Alpha")->required(false)->transform(CLI::Range(0.0, 100.0));
        app_.add_option("--beta", beta_, "Beta")->required(false)->transform(CLI::Range(0.0, 100.0));
    }

    String IndexName() const override { return fmt::format("bmp_block{}", block_size_); }

public:
    i32 topk_ = 10;
    i8 block_size_ = 8;
    f32 alpha_ = 1.0;
    f32 beta_ = 1.0;
};

} // namespace benchmark