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

#include <algorithm>
#include <iostream>
#include <stdexcept>

import stl;
import file_system;
import local_file_system;
import file_system_type;
import compilation_config;
import third_party;
import profiler;
import linscan_alg;

using namespace infinity;

const f32 error_bound = 1e-6;
const int log_interval = 10000;

struct SparseMatrix {
    UniquePtr<f32[]> data_{};
    UniquePtr<i32[]> indices_{};
    UniquePtr<i64[]> indptr_{};
    i64 nrow_{};
    i64 ncol_{};
    i64 nnz_{};
};

class SparseMatrixIter {
public:
    SparseMatrixIter(const SparseMatrix &mat) : mat_(mat) {}

    bool HasNext() { return row_i_ < mat_.nrow_; }

    void Next() {
        ++row_i_;
        offset_ = mat_.indptr_[row_i_];
    }

    SparseVecRef val() const {
        const float *data = mat_.data_.get() + offset_;
        const auto *indices = reinterpret_cast<const u32 *>(mat_.indices_.get() + offset_);
        i32 nnz = mat_.indptr_[row_i_ + 1] - offset_;
        return SparseVecRef{data, indices, nnz};
    }

    i64 row_id() const { return row_i_; }

private:
    const SparseMatrix &mat_;
    i64 row_i_{};
    i64 offset_{};
};

SparseMatrix DecodeSparseDataset(const Path &data_path) {
    SparseMatrix ret;

    LocalFileSystem fs;
    if (!fs.Exists(data_path)) {
        throw std::runtime_error(fmt::format("Data path: {} does not exist.", data_path.string()));
    }
    UniquePtr<FileHandler> file_handler = fs.OpenFile(data_path.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
    i64 nrow = 0;
    i64 ncol = 0;
    i64 nnz = 0;
    file_handler->Read(&nrow, sizeof(nrow));
    file_handler->Read(&ncol, sizeof(ncol));
    file_handler->Read(&nnz, sizeof(nnz));

    auto indptr = MakeUnique<i64[]>(nrow + 1);
    file_handler->Read(indptr.get(), sizeof(i64) * (nrow + 1));
    if (indptr[nrow] != nnz) {
        throw std::runtime_error("Invalid indptr.");
    }

    auto indices = MakeUnique<i32[]>(nnz);
    file_handler->Read(indices.get(), sizeof(i32) * nnz);
    // assert all element in indices >= 0 and < ncol
    {
        bool check = std::all_of(indices.get(), indices.get() + nnz, [ncol](i32 ele) { return ele >= 0 && ele < ncol; });
        if (!check) {
            throw std::runtime_error("Invalid indices.");
        }
    }

    auto data = MakeUnique<f32[]>(nnz);
    file_handler->Read(data.get(), sizeof(f32) * nnz);
    return {std::move(data), std::move(indices), std::move(indptr), nrow, ncol, nnz};
}

Pair<UniquePtr<u32[]>, UniquePtr<f32[]>> DecodeGroundtruth(const Path &groundtruth_path, u32 top_k, u32 query_n) {
    LocalFileSystem fs;
    if (!fs.Exists(groundtruth_path)) {
        throw std::runtime_error(fmt::format("Groundtruth path: {} does not exist.", groundtruth_path.string()));
    }
    UniquePtr<FileHandler> file_handler = fs.OpenFile(groundtruth_path.string(), FileFlags::READ_FLAG, FileLockType::kNoLock);
    SizeT file_size = fs.GetFileSize(*file_handler);
    if (file_size != sizeof(u32) * 2 + (sizeof(u32) + sizeof(float)) * (query_n * top_k)) {
        throw std::runtime_error("Invalid groundtruth file format");
    }
    {
        u32 ans_n = 0;
        file_handler->Read(&ans_n, sizeof(ans_n));
        u32 top_k1 = 0;
        file_handler->Read(&top_k1, sizeof(top_k1));
        if (ans_n != query_n || top_k1 != top_k) {
            throw std::runtime_error("Invalid groundtruth file format");
        }
    }
    auto indices = MakeUnique<u32[]>(query_n * top_k);
    file_handler->Read(indices.get(), sizeof(u32) * query_n * top_k);
    auto scores = MakeUnique<f32[]>(query_n * top_k);
    file_handler->Read(scores.get(), sizeof(f32) * query_n * top_k);
    return {std::move(indices), std::move(scores)};
}

void ImportData(LinScan &index, const Path &data_path) {
    SparseMatrix mat = DecodeSparseDataset(data_path);
    for (SparseMatrixIter iter(mat); iter.HasNext(); iter.Next()) {
        SparseVecRef vec = iter.val();
        u32 doc_id = iter.row_id();
        index.Insert(vec, doc_id);

        if (log_interval != 0 && doc_id % log_interval == 0) {
            std::cout << fmt::format("Inserting doc {}\n", doc_id);
        }
    }
}

Vector<Pair<Vector<u32>, Vector<f32>>> QueryData(const LinScan &index, u32 top_k, const Path &query_path) {
    Vector<Pair<Vector<u32>, Vector<f32>>> res;
    SparseMatrix mat = DecodeSparseDataset(query_path);
    for (SparseMatrixIter iter(mat); iter.HasNext(); iter.Next()) {
        SparseVecRef query = iter.val();
        auto [indices, score] = index.Query(query, top_k);
        res.emplace_back(std::move(indices), std::move(score));

        if (log_interval != 0 && iter.row_id() % log_interval == 0) {
            std::cout << fmt::format("Querying doc {}\n", iter.row_id());
        }
    }
    return res;
}

void CheckGroundtruth(const Path &groundtruth_path, const Vector<Pair<Vector<u32>, Vector<f32>>> &results, u32 top_k) {
    u32 query_n = results.size();
    auto [gt_indices_list, gt_score_list] = DecodeGroundtruth(groundtruth_path, top_k, query_n);
    for (u32 i = 0; i < results.size(); ++i) {
        const auto &[indices, scores] = results[i];
        const u32 *gt_indices = gt_indices_list.get() + i * top_k;
        const f32 *gt_score = gt_score_list.get() + i * top_k;

        // std::cout << fmt::format("Query {}\n", i);
        // for (u32 j = 0; j < top_k; ++j) {
        //     std::cout << fmt::format("{} {}, ", j, indices[j], scores[j]);
        // }
        // std::cout << "\n";
        // for (u32 j = 0; j < top_k; ++j) {
        //     std::cout << fmt::format("{} {}, ", j, gt_indices[j], gt_score[j]);
        // }
        // std::cout << "\n";

        for (u32 j = 0; j < top_k; ++j) {
            if (indices[j] != gt_indices[j] || std::abs(scores[j] - gt_score[j]) > error_bound) {
                std::cout << fmt::format("Error at {}, {}, {}, {}, {}, {}\n", i, j, indices[j], scores[j], gt_indices[j], gt_score[j]);
            }
        }
    }
}

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

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    Path dataset_dir = Path(test_data_path()) / "benchmark" / "splade";
    Path query_path = dataset_dir / "queries.dev.csr";
    Path data_path = dataset_dir;
    Path groundtruth_path = dataset_dir;
    switch (dataset_type) {
        case DataSetType::kSmall: {
            data_path /= "base_small.csr";
            groundtruth_path /= "base_small.dev.gt";
            break;
        }
        case DataSetType::k1M: {
            data_path /= "base_1M.csr";
            groundtruth_path /= "base_1M.dev.gt";
            break;
        }
        case DataSetType::kFull: {
            data_path /= "base_full.csr";
            groundtruth_path /= "base_full.dev.gt";
            break;
        }
        default: {
            throw std::runtime_error(fmt::format("Unsupported dataset type: {}.", static_cast<u8>(dataset_type)));
        }
    };
    u32 top_k = 10;

    // switch (mode_type) {
    //     case ModeType::kImport: {
    //         ImportData(data_path);
    //         break;
    //     }
    //     case ModeType::kQuery: {
    //         throw std::runtime_error("Not implemented.");
    //         return 1;
    //     }
    //     default: {
    //         throw std::runtime_error(fmt::format("Unsupported mode type: {}.", static_cast<u8>(mode_type)));
    //     }
    // }
    BaseProfiler profiler;

    LinScan index;

    profiler.Begin();
    ImportData(index, data_path);
    profiler.End();
    std::cout << fmt::format("Import data time: {}\n", profiler.ElapsedToString(1000));

    profiler.Begin();
    auto query_result = QueryData(index, top_k, query_path);
    profiler.End();
    std::cout << fmt::format("Query data time: {}\n", profiler.ElapsedToString(1000));

    return 0;
}
