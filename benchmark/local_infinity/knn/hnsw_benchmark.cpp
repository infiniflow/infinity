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

#include "hnsw_benchmark_util.h"
#include <cassert>

import infinity_core;
import compilation_config;

using namespace infinity;

enum class ModeType : i8 {
    BUILD,
    QUERY,
    CompressLVQ,
    CompressRabitq,
};

enum class BenchmarkType : i8 {
    SIFT,
    GIST,
};

std::string BenchmarkTypeToString(BenchmarkType benchmark_type) {
    switch (benchmark_type) {
        case BenchmarkType::SIFT:
            return "sift";
        case BenchmarkType::GIST:
            return "gist";
    }
}

enum class BuildType : i8 {
    PLAIN,
    LVQ,
    Rabitq,
    CompressToLVQ,
    CompressToRabitq,
    LSGBuild,
    LSGLVQBuild,
    LSGCompressToLVQ,
    LSGRabitqBuild,
    LSGCompressToRabitq
};

std::string BuildTypeToString(BuildType build_type) {
    switch (build_type) {
        case BuildType::PLAIN:
            return "plain";
        case BuildType::LVQ:
            return "lvq";
        case BuildType::Rabitq:
            return "rabitq";
        case BuildType::CompressToLVQ:
            return "clvq";
        case BuildType::CompressToRabitq:
            return "crabitq";
        case BuildType::LSGBuild:
            return "lsg";
        case BuildType::LSGLVQBuild:
            return "lvq_lsg";
        case BuildType::LSGCompressToLVQ:
            return "clvq_lsg";
        case BuildType::LSGRabitqBuild:
            return "rabitq_lsg";
        case BuildType::LSGCompressToRabitq:
            return "crabitq_lsg";
    }
}

struct BenchmarkOption {
public:
    BenchmarkOption() : app_("hnsw_benchmark") {}

    static std::string IndexName(const BenchmarkType &benchmark_type, const BuildType &build_type, size_t M, size_t ef_construction) {
        return fmt::format("hnsw_{}_{}_{}_{}", BenchmarkTypeToString(benchmark_type), BuildTypeToString(build_type), M, ef_construction);
    }

    void Parse(int argc, char *argv[]) {
        std::map<std::string, ModeType> mode_map = {{"build", ModeType::BUILD},
                                                    {"query", ModeType::QUERY},
                                                    {"clvq", ModeType::CompressLVQ},
                                                    {"crabitq", ModeType::CompressRabitq}};
        std::map<std::string, BenchmarkType> benchmark_type_map = {{"sift", BenchmarkType::SIFT}, {"gist", BenchmarkType::GIST}};
        std::map<std::string, BuildType> build_type_map = {
            {"plain", BuildType::PLAIN},
            {"lvq", BuildType::LVQ},
            {"rabitq", BuildType::Rabitq},
            {"clvq", BuildType::CompressToLVQ},
            {"crabitq", BuildType::CompressToRabitq},
            {"lsg", BuildType::LSGBuild},
            {"lvq_lsg", BuildType::LSGLVQBuild},
            {"clvq_lsg", BuildType::LSGCompressToLVQ},
            {"rabitq_lsg", BuildType::LSGRabitqBuild},
            {"crabitq_lsg", BuildType::LSGCompressToRabitq},
        };

        app_.add_option("--mode", mode_type_, "mode")->required()->transform(CLI::CheckedTransformer(mode_map, CLI::ignore_case));
        app_.add_option("--benchmark_type", benchmark_type_, "benchmark type")
            ->required()
            ->transform(CLI::CheckedTransformer(benchmark_type_map, CLI::ignore_case));
        app_.add_option("--build_type", build_type_, "build type")->required()->transform(CLI::CheckedTransformer(build_type_map, CLI::ignore_case));
        app_.add_option("--thread_n", thread_n_, "thread number")->required(false);

        app_.add_option("--chunk_size", chunk_size_, "chunk size")->required(false);
        app_.add_option("--max_chunk_num", max_chunk_num_, "max chunk size")->required(false);
        app_.add_option("--M", M_, "M")->required(false);
        app_.add_option("--ef_construction", ef_construction_, "ef construction")->required(false);

        app_.add_option("--ef", ef_, "ef")->required(false);
        app_.add_option("--test_n", test_n_, "test n")->required(false);

        app_.add_option("--lsg_k", lsg_k_, "lsg k")->required(false);
        app_.add_option("--topk", query_topk_, "topk")->required(false);

        try {
            app_.parse(argc, argv);
        } catch (const CLI::ParseError &e) {
            UnrecoverableError(e.what());
        }
        ParseInner();
    }

    void ParseInner() {
        std::string index_name = IndexName(benchmark_type_, build_type_, M_, ef_construction_);
        switch (benchmark_type_) {
            case BenchmarkType::SIFT: {
                base_path_ = "test/data/benchmark/sift_1m/sift_base.fvecs";
                query_path_ = "test/data/benchmark/sift_1m/sift_query.fvecs";
                groundtruth_path_ = "test/data/benchmark/sift_1m/sift_groundtruth.ivecs";
                break;
            }
            case BenchmarkType::GIST: {
                base_path_ = "./test/data/benchmark/gist_1m/gist_base.fvecs";
                query_path_ = "./test/data/benchmark/gist_1m/gist_query.fvecs";
                groundtruth_path_ = "./test/data/benchmark/gist_1m/gist_groundtruth.ivecs";
                break;
            }
        }
        index_save_path_ = index_dir_ / fmt::format("{}.bin", index_name);
    }

public:
    ModeType mode_type_;
    BenchmarkType benchmark_type_;
    BuildType build_type_;
    size_t thread_n_ = std::thread::hardware_concurrency();

    size_t chunk_size_ = 8192;
    size_t max_chunk_num_ = 1024;
    size_t M_ = 16;
    size_t ef_construction_ = 200;

    size_t ef_ = 0;
    size_t test_n_ = 1;

    size_t lsg_k_ = 10;
    size_t query_topk_ = 0;

public:
    std::filesystem::path base_path_;
    std::filesystem::path query_path_;
    std::filesystem::path groundtruth_path_;
    std::filesystem::path index_dir_ = std::filesystem::path(tmp_data_path());
    std::filesystem::path index_save_path_;

private:
    CLI::App app_;
};

using LabelT = u32;
using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
using HnswLSG = KnnHnsw<PlainL2VecStoreType<float, true>, LabelT>;
using HnswLVQ = KnnHnsw<LVQL2VecStoreType<float, i8>, LabelT>;
using HnswLVQLSG = KnnHnsw<LVQL2VecStoreType<float, i8, true>, LabelT>;
using HnswRabitq = KnnHnsw<RabitqL2VecStoreType<float>, LabelT>;
using HnswRabitqLSG = KnnHnsw<RabitqL2VecStoreType<float, true>, LabelT>;

std::unique_ptr<float[]> GetAvgBF(size_t vec_num, size_t dim, const float *data, size_t ls_k, size_t sample_num) {
    auto avg = std::make_unique<float[]>(vec_num);
    std::vector<size_t> sample_idx(sample_num);
    for (size_t i = 0; i < sample_num; ++i) {
        sample_idx[i] = rand() % vec_num;
    }
    auto task = [&](size_t start_i, size_t end_i) {
        std::vector<float> distances(sample_num);
        for (size_t i = start_i; i < end_i; ++i) {
            if (i % 1000 == 0) {
                std::cout << fmt::format("Sample {} / {}", i, vec_num) << std::endl;
            }
            const float *v = data + i * dim;
            for (size_t j = 0; j < sample_num; ++j) {
                const float *v2 = data + sample_idx[j] * dim;

                float distance = 0;
                for (size_t k = 0; k < dim; ++k) {
                    float diff = v[k] - v2[k];
                    distance += diff * diff;
                }
                distances[j] = distance;
            }
            std::sort(distances.begin(), distances.end());
            avg[i] = 0;
            for (size_t j = 0; j < ls_k; ++j) {
                avg[i] += distances[j];
            }
            avg[i] /= ls_k;
        }
    };
    std::vector<std::thread> threads;
    size_t thread_num = 16;
    size_t task_size = (vec_num - 1) / thread_num + 1;

    for (size_t i = 0; i < thread_num; ++i) {
        size_t start_i = i * task_size;
        size_t end_i = std::min(start_i + task_size, vec_num);
        threads.emplace_back(task, start_i, end_i);
    }
    for (auto &thread : threads) {
        thread.join();
    }

    return avg;
}

template <typename HnswT, typename HnswT2>
void Build(const BenchmarkOption &option) {
    BaseProfiler profiler;

    auto [base_num, base_dim, base_data] = benchmark::DecodeFvecsDataset<float>(option.base_path_);

    profiler.Begin();
    auto hnsw = HnswT::Make(option.chunk_size_, option.max_chunk_num_, base_dim, option.M_, option.ef_construction_);

    std::unique_ptr<float[]> avg;
    if constexpr (std::is_same_v<HnswT, HnswLSG> || std::is_same_v<HnswT, HnswLVQLSG> || std::is_same_v<HnswT, HnswRabitqLSG>) {
        size_t sample_num = base_num * 0.01;
        size_t ls_k = 10;
        avg = GetAvgBF(base_num, base_dim, base_data.get(), ls_k, sample_num);

        float alpha = 1.0;
        hnsw->distance().SetLSGParam(alpha, avg.get());
    }

    DenseVectorIter<float, LabelT> iter(base_data.get(), base_dim, base_num);
    hnsw->StoreData(iter);
    base_data.reset();

    std::vector<std::thread> build_threads;
    const size_t kBuildBucketSize = 1024;
    size_t bucket_size = std::max(kBuildBucketSize, base_num / option.thread_n_);
    size_t bucket_num = (base_num - 1) / bucket_size + 1;
    assert(bucket_num <= option.thread_n_);

    for (size_t i = 0; i < bucket_num; ++i) {
        size_t i1 = i * bucket_size;
        size_t i2 = std::min(i1 + bucket_size, base_num);
        build_threads.emplace_back([&, i1, i2] {
            for (size_t j = i1; j < i2; ++j) {
                if (j % 10000 == 0) {
                    std::cout << fmt::format("Build {} / {}", j, base_num) << std::endl;
                }
                hnsw->Build(j);
            }
        });
    }
    for (auto &thread : build_threads) {
        thread.join();
    }
    build_threads.clear();

    profiler.End();
    std::cout << "Build time: " << profiler.ElapsedToString(10) << std::endl;

    auto save = [&](auto &hnsw) {
        auto [index_file, index_status] = VirtualStore::Open(option.index_save_path_.string(), FileAccessMode::kWrite);
        if (!index_status.ok()) {
            UnrecoverableError(index_status.message());
        }
        hnsw->Save(*index_file);
    };
    if constexpr (std::is_same_v<HnswT, HnswT2>) {
        save(hnsw);
    } else {
        profiler.Begin();
        std::unique_ptr<HnswT2> hnsw_compress;
        if constexpr (std::is_same_v<HnswT2, HnswLVQ>) {
            hnsw_compress = std::move(*hnsw).CompressToLVQ();
        } else if (std::is_same_v<HnswT2, HnswRabitq>) {
            hnsw_compress = std::move(*hnsw).CompressToRabitq();
        } else {
            UnrecoverableError("Compress failed: Compress Type not exists!");
        }
        profiler.End();
        std::cout << "Compress time: " << profiler.ElapsedToString(10) << std::endl;
        save(hnsw_compress);
    }
}

template <typename HnswT>
void Query(const BenchmarkOption &option) {
    BaseProfiler profiler;

    auto [index_file, index_status] = VirtualStore::Open(option.index_save_path_.string(), FileAccessMode::kRead);
    if (!index_status.ok()) {
        UnrecoverableError(index_status.message());
    }

    auto hnsw = HnswT::Load(*index_file);

    auto [base_num, base_dim, base_data] = benchmark::DecodeFvecsDataset<float>(option.base_path_);
    auto [query_num, query_dim, query_data] = benchmark::DecodeFvecsDataset<float>(option.query_path_);
    auto [gt_num, topk, gt_data] = benchmark::DecodeFvecsDataset<i32>(option.groundtruth_path_);
    size_t query_topk = topk;
    if (option.query_topk_ != 0) {
        query_topk = option.query_topk_;
    }
    if (gt_num != query_num) {
        UnrecoverableError("gt_num != query_num");
    }
    std::vector<std::vector<LabelT>> results(query_num, std::vector<LabelT>(query_topk));
    auto rerank = [&](const float *query_vec, std::vector<std::pair<float, LabelT>> &pairs) -> void {
        auto l2_distance = GetSIMD_FUNCTIONS().L2Distance_func_ptr_;
        for (size_t i = 0; i < pairs.size(); ++i) {
            const auto &base_id = pairs[i].second;
            const auto &base_vec = base_data.get() + base_id * base_dim;
            pairs[i].first = l2_distance(query_vec, base_vec, base_dim);
        }
        std::sort(pairs.begin(), pairs.end());
    };
    auto test = [&](const KnnSearchOption &search_option) -> float {
        profiler.Begin();
        std::vector<std::thread> query_threads;
        std::atomic<i32> cur_i = 0;
        for (size_t i = 0; i < option.thread_n_; ++i) {
            query_threads.emplace_back([&] {
                size_t i;
                while ((i = cur_i.fetch_add(1)) < query_num) {
                    const float *query = query_data.get() + i * query_dim;
                    std::vector<std::pair<float, LabelT>> pairs = hnsw->KnnSearchSorted(query, query_topk, search_option);
                    if (std::is_same_v<HnswT, HnswRabitq> || std::is_same_v<HnswT, HnswRabitqLSG>) {
                        rerank(query, pairs);
                    }
                    if (pairs.size() < query_topk) {
                        UnrecoverableError("result_n != topk");
                    }
                    for (size_t j = 0; j < query_topk; ++j) {
                        results[i][j] = pairs[j].second;
                    }
                }
            });
        }
        for (auto &thread : query_threads) {
            thread.join();
        }
        profiler.End();
        return 1e9 * query_num / profiler.Elapsed();
    };
    auto cal_recall = [&]() -> float {
        i32 correct = 0;
        for (size_t i = 0; i < query_num; ++i) {
            std::unordered_set<LabelT> gt_set(gt_data.get() + i * topk, gt_data.get() + i * topk + query_topk);
            for (size_t j = 0; j < results[i].size(); ++j) {
                if (gt_set.contains(results[i][j])) {
                    correct++;
                }
            }
        }
        return float(correct) / (query_num * query_topk);
    };
    if (option.ef_ == 0) {
        std::cout << "test_n = " << option.test_n_ << std::endl;
        for (size_t ef = 100; ef <= 1000; ef += 100) {
            KnnSearchOption search_option{.ef_ = ef};
            float QPS = 0;
            for (size_t i = 0; i < option.test_n_; ++i) {
                QPS += test(search_option);
            }
            QPS /= option.test_n_;
            float recall = cal_recall();
            std::cout << fmt::format("efsearch={},QPS={:.2f},Recall@{}={:.4f}", ef, QPS, query_topk, recall) << std::endl;
        }
    } else {
        for (size_t i = 0; i < option.test_n_; ++i) {
            KnnSearchOption search_option{.ef_ = option.ef_};
            float QPS = test(search_option);
            float recall = cal_recall();
            std::cout << fmt::format("efsearch={},QPS={:.2f},Recall@{}={:.4f}", option.ef_, QPS, query_topk, recall) << std::endl;
        }
    }
}

template <typename HnswT, typename HnswT2>
void Compress(const BenchmarkOption &option) {
    if (option.build_type_ != BuildType::PLAIN && option.build_type_ != BuildType::LSGBuild) {
        UnrecoverableError("Compress only support plain build or lsg build");
    }

    auto [index_file, index_status] = VirtualStore::Open(option.index_save_path_.string(), FileAccessMode::kRead);
    if (!index_status.ok()) {
        UnrecoverableError(index_status.message());
    }
    auto hnsw = HnswT::Load(*index_file);

    std::string new_index_name;
    if constexpr (std::is_same_v<HnswT, Hnsw>) {
        new_index_name = BenchmarkOption::IndexName(option.benchmark_type_, BuildType::CompressToLVQ, option.M_, option.ef_construction_);
    } else if constexpr (std::is_same_v<HnswT, HnswLSG>) {
        new_index_name = BenchmarkOption::IndexName(option.benchmark_type_, BuildType::LSGCompressToLVQ, option.M_, option.ef_construction_);
    } else {
        UnrecoverableError("Unsupport compress type");
    }
    std::filesystem::path new_index_save_path = option.index_dir_ / fmt::format("{}.bin", new_index_name);

    std::unique_ptr<HnswT2> hnsw_compress;
    if constexpr (std::is_same_v<HnswT2, HnswLVQ>) {
        hnsw_compress = std::move(*hnsw).CompressToLVQ();
    } else if (std::is_same_v<HnswT2, HnswRabitq>) {
        hnsw_compress = std::move(*hnsw).CompressToRabitq();
    } else {
        UnrecoverableError("Compress failed: Compress Type not exists!");
    }
    auto [index_file_compress, index_status_compress] = VirtualStore::Open(new_index_save_path.string(), FileAccessMode::kWrite);
    if (!index_status_compress.ok()) {
        UnrecoverableError(index_status_compress.message());
    }
    hnsw_compress->Save(*index_file_compress);
}

int main(int argc, char *argv[]) {
    BenchmarkOption option;
    option.Parse(argc, argv);
    switch (option.mode_type_) {
        case ModeType::BUILD: {
            switch (option.build_type_) {
                case BuildType::PLAIN: {
                    Build<Hnsw, Hnsw>(option);
                    break;
                }
                case BuildType::LVQ: {
                    Build<HnswLVQ, HnswLVQ>(option);
                    break;
                }
                case BuildType::Rabitq: {
                    Build<HnswRabitq, HnswRabitq>(option);
                    break;
                }
                case BuildType::CompressToLVQ: {
                    Build<Hnsw, HnswLVQ>(option);
                    break;
                }
                case BuildType::CompressToRabitq: {
                    Build<Hnsw, HnswRabitq>(option);
                    break;
                }
                case BuildType::LSGBuild: {
                    Build<HnswLSG, HnswLSG>(option);
                    break;
                }
                case BuildType::LSGLVQBuild: {
                    Build<HnswLVQLSG, HnswLVQLSG>(option);
                    break;
                }
                case BuildType::LSGCompressToLVQ: {
                    Build<HnswLSG, HnswLVQ>(option);
                    break;
                }
                case BuildType::LSGRabitqBuild: {
                    Build<HnswRabitqLSG, HnswRabitqLSG>(option);
                    break;
                }
                case BuildType::LSGCompressToRabitq: {
                    Build<HnswLSG, HnswRabitq>(option);
                    break;
                }
            }
            break;
        }
        case ModeType::QUERY: {
            switch (option.build_type_) {
                case BuildType::PLAIN: {
                    Query<Hnsw>(option);
                    break;
                }
                case BuildType::LVQ:
                case BuildType::CompressToLVQ:
                case BuildType::LSGCompressToLVQ: {
                    Query<HnswLVQ>(option);
                    break;
                }
                case BuildType::Rabitq:
                case BuildType::CompressToRabitq:
                case BuildType::LSGCompressToRabitq: {
                    Query<HnswRabitq>(option);
                    break;
                }
                case BuildType::LSGBuild: {
                    Query<HnswLSG>(option);
                    break;
                }
                case BuildType::LSGLVQBuild: {
                    Query<HnswLVQLSG>(option);
                    break;
                }
                case BuildType::LSGRabitqBuild: {
                    Query<HnswRabitqLSG>(option);
                    break;
                }
            }
            break;
        }
        case ModeType::CompressLVQ: {
            switch (option.build_type_) {
                case BuildType::PLAIN: {
                    Compress<Hnsw, HnswLVQ>(option);
                    break;
                }
                case BuildType::LSGBuild: {
                    Compress<HnswLSG, HnswLVQ>(option);
                    break;
                }
                default: {
                    UnrecoverableError("Unsupport compress type");
                }
            }
            break;
        }
        case ModeType::CompressRabitq: {
            switch (option.build_type_) {
                case BuildType::PLAIN: {
                    Compress<Hnsw, HnswRabitq>(option);
                    break;
                }
                case BuildType::LSGBuild: {
                    Compress<HnswLSG, HnswRabitq>(option);
                    break;
                }
                default: {
                    UnrecoverableError("Unsupport compress type");
                }
            }
            break;
        }
    }
    return 0;
}

// int main() {
//     BenchmarkOption option;
//     option.mode_type_ = ModeType::BUILD;
//     option.benchmark_type_ = BenchmarkType::GIST;
//     for (size_t ef_construction = 200; ef_construction <= 1000; ef_construction += 200) {
//         option.ef_construction_ = ef_construction;
//         {
//             option.build_type_ = BuildType::PLAIN;
//             option.ParseInner();
//             std::cout << fmt::format("ef_construction: {}, build_type: {}, ", ef_construction, BuildTypeToString(option.build_type_)) << std::endl;
//             Build<Hnsw, Hnsw>(option);
//             Compress<Hnsw, HnswLVQ>(option);
//         }
//         {
//             option.build_type_ = BuildType::LVQ;
//             option.ParseInner();
//             std::cout << fmt::format("ef_construction: {}, build_type: {}, ", ef_construction, BuildTypeToString(option.build_type_)) << std::endl;
//             Build<HnswLVQ, HnswLVQ>(option);
//         }
//     }
// }
