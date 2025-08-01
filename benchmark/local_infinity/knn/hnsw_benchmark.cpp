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

// import :hnsw_lsg_builder;
// import :index_hnsw;
// import internal_types;
// import :index_base;
// import column_def;
// import embedding_info;
// import logical_type;
// import data_type;

using namespace infinity;

enum class ModeType : i8 {
    BUILD,
    QUERY,
    COMPRESS,
};

enum class BenchmarkType : i8 {
    SIFT,
    GIST,
};

String BenchmarkTypeToString(BenchmarkType benchmark_type) {
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
    CompressToLVQ,
    LSGBuild,
    LSGLVQBuild,
    LSGCompressToLVQ,
};

String BuildTypeToString(BuildType build_type) {
    switch (build_type) {
        case BuildType::PLAIN:
            return "plain";
        case BuildType::LVQ:
            return "lvq";
        case BuildType::CompressToLVQ:
            return "clvq";
        case BuildType::LSGBuild:
            return "lsg";
        case BuildType::LSGLVQBuild:
            return "lvq_lsg";
        case BuildType::LSGCompressToLVQ:
            return "clvq_lsg";
    }
}

struct BenchmarkOption {
public:
    BenchmarkOption() : app_("hnsw_benchmark") {}

    static String IndexName(const BenchmarkType &benchmark_type, const BuildType &build_type, SizeT M, SizeT ef_construction) {
        return fmt::format("hnsw_{}_{}_{}_{}", BenchmarkTypeToString(benchmark_type), BuildTypeToString(build_type), M, ef_construction);
    }

    void Parse(int argc, char *argv[]) {
        Map<String, ModeType> mode_map = {{"build", ModeType::BUILD}, {"query", ModeType::QUERY}, {"compress", ModeType::COMPRESS}};
        Map<String, BenchmarkType> benchmark_type_map = {{"sift", BenchmarkType::SIFT}, {"gist", BenchmarkType::GIST}};
        Map<String, BuildType> build_type_map = {
            {"plain", BuildType::PLAIN},
            {"lvq", BuildType::LVQ},
            {"clvq", BuildType::CompressToLVQ},
            {"lsg", BuildType::LSGBuild},
            {"lvq_lsg", BuildType::LSGLVQBuild},
            {"clvq_lsg", BuildType::LSGCompressToLVQ},
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
        String index_name = IndexName(benchmark_type_, build_type_, M_, ef_construction_);
        switch (benchmark_type_) {
            case BenchmarkType::SIFT: {
                data_path_ = "test/data/benchmark/sift_1m/sift_base.fvecs";
                query_path_ = "test/data/benchmark/sift_1m/sift_query.fvecs";
                groundtruth_path_ = "test/data/benchmark/sift_1m/sift_groundtruth.ivecs";
                break;
            }
            case BenchmarkType::GIST: {
                data_path_ = "./test/data/benchmark/gist_1m/gist_base.fvecs";
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
    SizeT thread_n_ = std::thread::hardware_concurrency();

    SizeT chunk_size_ = 8192;
    SizeT max_chunk_num_ = 1024;
    SizeT M_ = 16;
    SizeT ef_construction_ = 200;

    SizeT ef_ = 0;
    SizeT test_n_ = 1;

    SizeT lsg_k_ = 10;
    SizeT query_topk_ = 0;

public:
    Path data_path_;
    Path query_path_;
    Path groundtruth_path_;
    Path index_dir_ = Path(tmp_data_path());
    Path index_save_path_;

private:
    CLI::App app_;
};

using LabelT = u32;
using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
using HnswLSG = KnnHnsw<PlainL2VecStoreType<float, true>, LabelT>;
using HnswLVQ = KnnHnsw<LVQL2VecStoreType<float, i8>, LabelT>;
using HnswLVQLSG = KnnHnsw<LVQL2VecStoreType<float, i8, true>, LabelT>;

// SharedPtr<String> index_name = MakeShared<String>("index_name");
// String filename = "filename";
// Vector<String> column_names = {"col_name"};

// UniquePtr<IndexHnsw> MakeLSGIndexHnsw(const BenchmarkOption &option) {
//     MetricType metric_type = MetricType::kMetricL2;
//     HnswEncodeType encode_type = HnswEncodeType::kPlain;
//     HnswBuildType build_type = HnswBuildType::kLSG;
//     SizeT M = option.M_;
//     SizeT ef_construction = option.ef_construction_;
//     SizeT block_size = option.chunk_size_;
//     return MakeUnique<IndexHnsw>(index_name, nullptr, filename, column_names, metric_type, encode_type, build_type, M, ef_construction, block_size);
// }

// UniquePtr<ColumnDef> MakeColumnDef(SizeT dim) {
//     auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, dim);
//     auto data_type = MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo);
//     return MakeUnique<ColumnDef>(0, data_type, column_names[0], std::set<ConstraintType>());
// }

UniquePtr<float[]> GetAvgBF(SizeT vec_num, SizeT dim, const float *data, SizeT ls_k, SizeT sample_num) {
    auto avg = MakeUnique<float[]>(vec_num);
    Vector<SizeT> sample_idx(sample_num);
    for (SizeT i = 0; i < sample_num; ++i) {
        sample_idx[i] = rand() % vec_num;
    }
    auto task = [&](SizeT start_i, SizeT end_i) {
        Vector<float> distances(sample_num);
        for (SizeT i = start_i; i < end_i; ++i) {
            const float *v = data + i * dim;
            for (SizeT j = 0; j < sample_num; ++j) {
                const float *v2 = data + sample_idx[j] * dim;

                float distance = 0;
                for (SizeT k = 0; k < dim; ++k) {
                    float diff = v[k] - v2[k];
                    distance += diff * diff;
                }
                distances[j] = distance;
            }
            std::sort(distances.begin(), distances.end());
            avg[i] = 0;
            for (SizeT j = 0; j < ls_k; ++j) {
                avg[i] += distances[j];
            }
            avg[i] /= ls_k;
        }
    };
    Vector<std::thread> threads;
    SizeT thread_num = 16;
    SizeT task_size = (vec_num - 1) / thread_num + 1;

    for (SizeT i = 0; i < thread_num; ++i) {
        SizeT start_i = i * task_size;
        SizeT end_i = std::min(start_i + task_size, vec_num);
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

    auto [vec_num, dim, data] = benchmark::DecodeFvecsDataset<float>(option.data_path_);

    profiler.Begin();
    auto hnsw = HnswT::Make(option.chunk_size_, option.max_chunk_num_, dim, option.M_, option.ef_construction_);

    if constexpr (std::is_same_v<HnswT, HnswLSG> || std::is_same_v<HnswT, HnswLVQLSG>) {
        // auto column_def = MakeColumnDef(dim);
        // auto index_hnsw = MakeLSGIndexHnsw(option);
        // LSGConfig lsg_config;
        // lsg_config.ls_k_ = 10;
        // HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def), lsg_config);
        // DenseVectorIter<float, LabelT> iter(data.get(), dim, vec_num);
        // UniquePtr<float[]> avg = lsg_builder.GetLSAvg<decltype(iter), float, float>(std::move(iter), vec_num, RowID(0, 0));

        SizeT sample_num = 10000;
        SizeT ls_k = 10;
        auto avg = GetAvgBF(vec_num, dim, data.get(), ls_k, sample_num);

        float alpha = 1.0;
        hnsw->distance().SetLSGParam(alpha, std::move(avg));
    }

    DenseVectorIter<float, LabelT> iter(data.get(), dim, vec_num);
    hnsw->StoreData(iter);
    data.reset();

    Vector<std::thread> build_threads;
    const SizeT kBuildBucketSize = 1024;
    SizeT bucket_size = std::max(kBuildBucketSize, vec_num / option.thread_n_);
    SizeT bucket_num = (vec_num - 1) / bucket_size + 1;
    assert(bucket_num <= option.thread_n_);

    for (SizeT i = 0; i < bucket_num; ++i) {
        SizeT i1 = i * bucket_size;
        SizeT i2 = std::min(i1 + bucket_size, vec_num);
        build_threads.emplace_back([&, i1, i2] {
            for (SizeT j = i1; j < i2; ++j) {
                if (j % 10000 == 0) {
                    std::cout << fmt::format("Build {} / {}", j, vec_num) << std::endl;
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
    std::cout << "Build time: " << profiler.ElapsedToString(1000) << std::endl;

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
        auto hnsw_lvq = std::move(*hnsw).CompressToLVQ();
        save(hnsw_lvq);
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

    auto [query_num, query_dim, query_data] = benchmark::DecodeFvecsDataset<float>(option.query_path_);
    auto [gt_num, topk, gt_data] = benchmark::DecodeFvecsDataset<i32>(option.groundtruth_path_);
    SizeT query_topk = topk;
    if (option.query_topk_ != 0) {
        query_topk = option.query_topk_;
    }
    if (gt_num != query_num) {
        UnrecoverableError("gt_num != query_num");
    }
    Vector<Vector<LabelT>> results(query_num, Vector<LabelT>(query_topk));

    auto test = [&](SizeT i, const KnnSearchOption &search_option) {
        profiler.Begin();
        Vector<std::thread> query_threads;
        Atomic<i32> cur_i = 0;

        for (SizeT i = 0; i < option.thread_n_; ++i) {
            query_threads.emplace_back([&] {
                SizeT i;
                while ((i = cur_i.fetch_add(1)) < query_num) {
                    const float *query = query_data.get() + i * query_dim;
                    Vector<Pair<float, LabelT>> pairs = hnsw->KnnSearchSorted(query, query_topk, search_option);
                    if (pairs.size() < SizeT(query_topk)) {
                        UnrecoverableError("result_n != topk");
                    }
                    for (SizeT j = 0; j < query_topk; ++j) {
                        results[i][j] = pairs[j].second;
                    }
                }
            });
        }
        for (auto &thread : query_threads) {
            thread.join();
        }
        profiler.End();

        std::cout << fmt::format("Test {} / {}, Query time: {}", i + 1, option.test_n_, profiler.ElapsedToString(1000)) << std::endl;
    };
    auto cal_recall = [&](const KnnSearchOption &search_option) {
        i32 correct = 0;
        for (SizeT i = 0; i < query_num; ++i) {
            HashSet<LabelT> gt_set(gt_data.get() + i * topk, gt_data.get() + i * topk + query_topk);
            for (SizeT j = 0; j < query_topk; ++j) {
                if (gt_set.contains(results[i][j])) {
                    correct++;
                }
            }
        }
        float recall = float(correct) / (query_num * query_topk);
        std::cout << fmt::format("ef: {}, recall: {}", search_option.ef_, recall) << std::endl;
    };
    if (option.ef_ == 0) {
        for (SizeT ef = 100; ef <= 1000; ef += 100) {
            KnnSearchOption search_option{.ef_ = ef};
            for (SizeT i = 0; i < option.test_n_; ++i) {
                test(i, search_option);
            }
            cal_recall(search_option);
        }
    } else {
        for (SizeT i = 0; i < option.test_n_; ++i) {
            KnnSearchOption search_option{.ef_ = option.ef_};
            test(i, search_option);
            cal_recall(search_option);
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

    String new_index_name;
    if constexpr (std::is_same_v<HnswT, Hnsw>) {
        new_index_name = BenchmarkOption::IndexName(option.benchmark_type_, BuildType::CompressToLVQ, option.M_, option.ef_construction_);
    } else if constexpr (std::is_same_v<HnswT, HnswLSG>) {
        new_index_name = BenchmarkOption::IndexName(option.benchmark_type_, BuildType::LSGCompressToLVQ, option.M_, option.ef_construction_);
    } else {
        UnrecoverableError("Unsupport compress type");
    }
    Path new_index_save_path = option.index_dir_ / fmt::format("{}.bin", new_index_name);

    auto hnsw_lvq = std::move(*hnsw).CompressToLVQ();
    auto [index_file_lvq, index_status_lvq] = VirtualStore::Open(new_index_save_path.string(), FileAccessMode::kWrite);
    if (!index_status_lvq.ok()) {
        UnrecoverableError(index_status_lvq.message());
    }
    hnsw_lvq->Save(*index_file_lvq);
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
                case BuildType::CompressToLVQ: {
                    Build<Hnsw, HnswLVQ>(option);
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
                case BuildType::LSGBuild: {
                    Query<HnswLSG>(option);
                    break;
                }
                case BuildType::LSGLVQBuild: {
                    Query<HnswLVQLSG>(option);
                    break;
                }
            }
            break;
        }
        case ModeType::COMPRESS: {
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
    }
    return 0;
}

// int main() {
//     BenchmarkOption option;
//     option.mode_type_ = ModeType::BUILD;
//     option.benchmark_type_ = BenchmarkType::GIST;
//     for (SizeT ef_construction = 200; ef_construction <= 1000; ef_construction += 200) {
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
