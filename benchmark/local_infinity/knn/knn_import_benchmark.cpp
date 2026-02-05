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

import std.compat;

import compilation_config;
import internal_types;
import logical_type;
import embedding_info;
import create_index_info;
import extra_ddl_info;
import knn_expr;
import column_def;
import statement_common;
import data_type;

import column_expr;
import parsed_expr;
import search_expr;
import function_expr;

using namespace infinity;

enum class BuildType : i8 {
    PLAIN,
    LSG,
};

enum class EncodeType : i8 {
    PLAIN,
    LVQ,
    CompressToLVQ,
    CompressToRabitq,
};

struct BenchmarkArgs {
public:
    const static std::map<std::string, BuildType> build_type_map;
    const static std::map<std::string, EncodeType> encode_type_map;

public:
    BenchmarkArgs() : app_("knn_import_benchmark") {
        app_.add_option("--dataset", dataset_, "dataset")->required();
        app_.add_option("--build_type", build_type_, "build type")
            ->required(false)
            ->transform(CLI::CheckedTransformer(build_type_map, CLI::ignore_case));
        app_.add_option("--encode_type", encode_type_, "encode type")
            ->required(false)
            ->transform(CLI::CheckedTransformer(encode_type_map, CLI::ignore_case));
        app_.add_option("--ef_construction", ef_construction_, "ef construction")->required(false);
        app_.add_option("--M", M_, "M")->required(false);
        app_.add_option("--dataset_dir", dataset_dir_, "dataset dir")->required();
        app_.add_option("--infinity_dir", infinity_dir_, "infinity dir")->required(false);
        app_.add_option("--config_path", config_path_, "config path")->required(false);
    }

    void Parse(int argc, char *argv[]) {
        try {
            app_.parse(argc, argv);
        } catch (const CLI::ParseError &e) {
            UnrecoverableError(e.what());
        }

        if (dataset_ == "sift") {
            data_path_ = dataset_dir_ + "/benchmark/sift_1m/sift_base.fvecs";
            table_name_ = "sift_benchmark";
        } else if (dataset_ == "gist") {
            data_path_ = dataset_dir_ + "/benchmark/gist_1m/gist_base.fvecs";
            table_name_ = "gist_benchmark";
        } else if (dataset_ == "msmarco") {
            data_path_ = dataset_dir_ + "/benchmark/msmarco_1m/msmarco_base.fvecs";
            table_name_ = "msmarco_benchmark";
        } else {
            UnrecoverableError("Invalid dataset");
        }
    }

public:
    std::string dataset_ = "invalid";
    BuildType build_type_ = BuildType::PLAIN;
    EncodeType encode_type_ = EncodeType::LVQ;
    size_t ef_construction_ = 200;
    size_t M_ = 16;

    std::string dataset_dir_ = std::string(test_data_path());
    std::string infinity_dir_ = "/var/infinity";
    std::string config_path_;

    std::string data_path_;
    std::string table_name_;

private:
    CLI::App app_;
};

const std::map<std::string, BuildType> BenchmarkArgs::build_type_map = {
    {"plain", BuildType::PLAIN},
    {"lsg", BuildType::LSG},
};
const std::map<std::string, EncodeType> BenchmarkArgs::encode_type_map = {
    {"plain", EncodeType::PLAIN},
    {"lvq", EncodeType::LVQ},
    {"compress_to_lvq", EncodeType::CompressToLVQ},
    {"compress_to_rabitq", EncodeType::CompressToRabitq},
};

template <typename Function>
inline void LoopFor(size_t id_begin, size_t id_end, size_t thread_id, Function fn, const std::string &db_name, const std::string &table_name) {
    std::cout << "thread_id = " << thread_id << " [" << id_begin << ", " << id_end << ")" << std::endl;
    std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
    //    auto [data_base, status1] = infinity->GetDatabase("default_db");
    //    auto [table, status2] = data_base->GetTable(table_name);
    //    std::shared_ptr<Table> shared_table(std::move(table));
    for (auto id = id_begin; id < id_end; ++id) {
        fn(id, thread_id, infinity.get(), db_name, table_name);
    }
}

template <typename Function>
inline void ParallelFor(size_t start, size_t end, size_t numThreads, Function fn, const std::string &db_name, const std::string &table_name) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    size_t avg_cnt = (end - start) / numThreads;
    size_t extra_cnt = (end - start) % numThreads;
    for (size_t id_begin = start, thread_id = 0; thread_id < numThreads; ++thread_id) {
        size_t id_end = id_begin + avg_cnt + (thread_id < extra_cnt);
        threads.emplace_back(
            [id_begin, id_end, thread_id, fn, db_name, table_name] { LoopFor(id_begin, id_end, thread_id, fn, db_name, table_name); });
        id_begin = id_end;
    }
    for (auto &thread : threads) {
        thread.join();
    }
}

int main(int argc, char *argv[]) {
    BenchmarkArgs args;
    args.Parse(argc, argv);

    Infinity::LocalInit(args.infinity_dir_, args.config_path_);

    {
        std::cout << ">>> Import Benchmark Start <<<" << std::endl;

        std::vector<std::string> results;

        // hnsw benchmark
        do {
            std::vector<ColumnDef *> column_defs;

            // init column defs
            std::shared_ptr<DataType> col1_type = nullptr;
            std::string base_path = args.data_path_;
            std::string table_name = args.table_name_;
            if (args.dataset_ == "sift") {
                col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 128));
            } else if (args.dataset_ == "gist") {
                col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 960));
            } else if (args.dataset_ == "msmarco") {
                col1_type = std::make_shared<DataType>(LogicalType::kEmbedding, std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, 1024));
            } else {
                UnrecoverableError("Invalid dataset");
            }
            std::cout << "Import from: " << base_path << std::endl;

            std::string col1_name = "col1";
            auto col1_def = std::make_unique<ColumnDef>(0, col1_type, col1_name, std::set<ConstraintType>());
            column_defs.emplace_back(col1_def.release());

            std::string db_name = "default_db";
            std::string index_name = "hnsw_index";

            std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
            CreateDatabaseOptions create_db_options;
            create_db_options.conflict_type_ = ConflictType::kIgnore;
            auto r1 = infinity->CreateDatabase(db_name, std::move(create_db_options), "");

            //        auto [ data_base, status1 ] = infinity->GetDatabase(db_name);
            CreateTableOptions create_tb_options;
            create_tb_options.conflict_type_ = ConflictType::kIgnore;
            auto r2 =
                infinity->CreateTable(db_name, table_name, std::move(column_defs), std::vector<TableConstraint *>{}, std::move(create_tb_options));

            //        auto [ table, status2 ] = data_base->GetTable(table_name);

            if (!VirtualStore::Exists(base_path)) {
                std::cout << "File: " << base_path << " doesn't exist" << std::endl;
                break;
            }

            ImportOptions import_options;
            import_options.copy_file_type_ = CopyFileType::kFVECS;

            infinity::BaseProfiler profiler;
            profiler.Begin();
            QueryResult query_result = infinity->Import(db_name, table_name, base_path, import_options);
            std::cout << "Import data cost: " << profiler.ElapsedToString() << std::endl;

            auto index_info = new IndexInfo();
            index_info->index_type_ = IndexType::kHnsw;
            index_info->column_name_ = col1_name;

            {
                auto index_param_list = new std::vector<InitParameter *>();
                index_param_list->emplace_back(new InitParameter("m", std::to_string(args.M_)));
                index_param_list->emplace_back(new InitParameter("ef_construction", std::to_string(args.ef_construction_)));
                index_param_list->emplace_back(new InitParameter("metric", "l2"));
                if (args.encode_type_ == EncodeType::LVQ) {
                    index_param_list->emplace_back(new InitParameter("encode", "lvq"));
                } else if (args.encode_type_ == EncodeType::PLAIN || args.encode_type_ == EncodeType::CompressToLVQ ||
                           args.encode_type_ == EncodeType::CompressToRabitq) {
                    index_param_list->emplace_back(new InitParameter("encode", "plain"));
                } else {
                    UnrecoverableError("Invalid encode type");
                }
                if (args.build_type_ == BuildType::LSG) {
                    index_param_list->emplace_back(new InitParameter("build_type", "lsg"));
                }
                index_info->index_param_list_ = index_param_list;
            }

            std::string index_comment = "";
            query_result = infinity->CreateIndex(db_name, table_name, index_name, index_comment, index_info, CreateIndexOptions());

            if (!query_result.IsOk()) {
                std::cout << "Fail to create index." << profiler.ElapsedToString() << std::endl;
                profiler.End();
                break;
            }

            if (args.encode_type_ == EncodeType::CompressToLVQ || args.encode_type_ == EncodeType::CompressToRabitq) {
                AlterIndexOptions options;
                options.index_name_ = index_name;
                if (args.encode_type_ == EncodeType::CompressToLVQ) {
                    options.opt_params_.emplace_back(new InitParameter("compress_to_lvq"));
                } else {
                    options.opt_params_.emplace_back(new InitParameter("compress_to_rabitq"));
                }
                query_result = infinity->AlterIndex(db_name, table_name, options);
                if (!query_result.IsOk()) {
                    std::cout << "Fail to alter index." << profiler.ElapsedToString() << std::endl;
                    profiler.End();
                    break;
                }
            }

            std::cout << "Create Index cost: " << profiler.ElapsedToString() << std::endl;
            query_result = infinity->Flush();
            profiler.End();
            std::cout << "Flush data cost: " << profiler.ElapsedToString() << std::endl;
        } while (false);

        std::cout << ">>> Knn Import Benchmark End <<<" << std::endl;
        for (const auto &item : results) {
            std::cout << item << std::endl;
        }
    }

    {
        size_t thread_num = 1;
        size_t total_times = 10;
        size_t ef = 200;
        bool rerank{};
        std::cout << ">>> Query Benchmark Start <<<" << std::endl;
        std::cout << "std::thread Num: " << 1 << ", Times: " << 10 << std::endl;

        std::vector<std::string> results;

        std::string base_path = "/home/inf/code/infinity_new2/test/data/";

        std::string query_path = base_path;
        std::string groundtruth_path = base_path;
        size_t dimension = 0;
        int64_t topk = 100;

        std::string db_name = "default_db";
        std::string table_name;

        dimension = 128;
        query_path += "/benchmark/sift_1m/sift_query.fvecs";
        groundtruth_path += "/benchmark/sift_1m/sift_groundtruth.ivecs";
        table_name = "sift_benchmark";

        std::cout << "query from: " << query_path << std::endl;
        std::cout << "groundtruth is: " << groundtruth_path << std::endl;

        if (!VirtualStore::Exists(query_path)) {
            std::cerr << "File: " << query_path << " doesn't exist" << std::endl;
            exit(-1);
        }
        if (!VirtualStore::Exists(groundtruth_path)) {
            std::cerr << "File: " << groundtruth_path << " doesn't exist" << std::endl;
            exit(-1);
        }
        std::unique_ptr<float[]> queries_ptr;
        size_t query_count;
        {
            int dim = -1;
            std::tie(query_count, dim, queries_ptr) = benchmark::DecodeFvecsDataset<float>(query_path);
            assert((int)dimension == dim || !"query vector dim isn't 128");
        }
        auto queries = queries_ptr.get();
        std::vector<std::unordered_set<int>> ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100;
        {
            std::unique_ptr<int[]> gt;
            size_t gt_count;
            int gt_top_k;
            {
                std::tie(gt_count, gt_top_k, gt) = benchmark::DecodeFvecsDataset<int>(groundtruth_path);
                assert(gt_top_k == topk || !"gt_top_k != topk");
                assert(gt_count == query_count || !"gt_count != query_count");
            }
            ground_truth_sets_1.resize(gt_count);
            ground_truth_sets_10.resize(gt_count);
            ground_truth_sets_100.resize(gt_count);
            for (size_t i = 0; i < gt_count; ++i) {
                for (int j = 0; j < gt_top_k; ++j) {
                    auto x = gt[i * gt_top_k + j];
                    if (j < 1) {
                        ground_truth_sets_1[i].insert(x);
                    }
                    if (j < 10) {
                        ground_truth_sets_10[i].insert(x);
                    }
                    if (j < 100) {
                        ground_truth_sets_100[i].insert(x);
                    }
                }
            }
        }
        float elapsed_s_sum = 0;
        float recall_1 = 0, recall_10 = 0, recall_100 = 0;
        for (size_t times = 0; times < total_times + 2; ++times) {
            std::cout << "--- Start to run search benchmark: " << std::endl;
            std::vector<std::vector<uint64_t>> query_results(query_count);
            for (auto &v : query_results) {
                v.reserve(100);
            }
            auto query_function =
                [&](size_t query_idx, size_t thread_id, Infinity *infinity, const std::string &db_name, const std::string &table_name) {
                    KnnExpr *knn_expr = new KnnExpr();
                    knn_expr->dimension_ = dimension;
                    knn_expr->distance_type_ = KnnDistanceType::kL2;
                    knn_expr->topn_ = topk;
                    knn_expr->opt_params_ = new std::vector<InitParameter *>();
                    {
                        knn_expr->opt_params_->push_back(new InitParameter("ef", std::to_string(ef)));
                        if (rerank) {
                            knn_expr->opt_params_->push_back(new InitParameter("rerank"));
                        }
                    }
                    knn_expr->embedding_data_type_ = EmbeddingDataType::kElemFloat;
                    auto embedding_data_ptr = new float[dimension];
                    knn_expr->embedding_data_ptr_ = embedding_data_ptr;
                    auto src_ptr = queries + query_idx * dimension;
                    memmove(knn_expr->embedding_data_ptr_, src_ptr, dimension * sizeof(float));

                    ColumnExpr *column_expr = new ColumnExpr();
                    column_expr->names_.emplace_back("col1");
                    knn_expr->column_expr_ = column_expr;
                    std::vector<ParsedExpr *> *exprs = new std::vector<ParsedExpr *>();
                    exprs->emplace_back(knn_expr);
                    SearchExpr *search_expr = new SearchExpr();
                    search_expr->SetExprs(exprs);

                    std::vector<ParsedExpr *> *output_columns = new std::vector<ParsedExpr *>;
                    auto select_rowid_expr = new FunctionExpr();
                    select_rowid_expr->func_name_ = "row_id";
                    output_columns->emplace_back(select_rowid_expr);
                    auto result = infinity->Search(db_name,
                                                   table_name,
                                                   search_expr,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   output_columns,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   false);
                    {
                        auto &cv = result.result_table_->GetDataBlockById(0)->column_vectors_;
                        auto &column = *cv[0];
                        auto data = reinterpret_cast<const RowID *>(column.data().get());
                        auto cnt = column.Size();
                        for (size_t i = 0; i < cnt; ++i) {
                            query_results[query_idx].emplace_back(data[i].ToUint64());
                        }
                    }
                };
            BaseProfiler profiler("ParallelFor");
            profiler.Begin();
            ParallelFor(0, query_count, thread_num, query_function, db_name, table_name);
            profiler.End();
            // skip 2 warm up loops
            if (times >= 2) {
                auto elapsed_ns = profiler.Elapsed();
                auto elapsed_s = elapsed_ns / (1'000'000'000.0);
                results.push_back(fmt::format("Total cost : {} s", elapsed_s));
                elapsed_s_sum += elapsed_s;
            }
            {
                size_t correct_1 = 0, correct_10 = 0, correct_100 = 0;
                for (size_t query_idx = 0; query_idx < query_count; ++query_idx) {
                    auto &result = query_results[query_idx];
                    auto &ground_truth_1 = ground_truth_sets_1[query_idx];
                    auto &ground_truth_10 = ground_truth_sets_10[query_idx];
                    auto &ground_truth_100 = ground_truth_sets_100[query_idx];
                    for (size_t i = 0; i < result.size(); ++i) {
                        if (i < 1 and ground_truth_1.contains(result[i])) {
                            ++correct_1;
                        }
                        if (i < 10 and ground_truth_10.contains(result[i])) {
                            ++correct_10;
                        }
                        if (i < 100 and ground_truth_100.contains(result[i])) {
                            ++correct_100;
                        }
                    }
                }
                recall_1 = float(correct_1) / float(query_count * 1);
                recall_10 = float(correct_10) / float(query_count * 10);
                recall_100 = float(correct_100) / float(query_count * 100);
                results.push_back(fmt::format("R@1:   {:.4f}", recall_1));
                results.push_back(fmt::format("R@10:  {:.4f}", recall_10));
                results.push_back(fmt::format("R@100: {:.4f}", recall_100));
            }
        }

        std::cout << ">>> Query Benchmark End <<<" << std::endl;
        for (const auto &item : results) {
            std::cout << item << std::endl;
        }
        float elapsed_s_avg = elapsed_s_sum / total_times;
        size_t QPS = query_count / elapsed_s_avg;
        std::cout << fmt::format(
                         "thread : {}, ef : {}, Average cost : {:.4f} s, QPS : {}, Recall@1 : {:.4f}, Recall@10 : {:.4f}, Recall@100 : {:.4f}",
                         thread_num,
                         ef,
                         elapsed_s_avg,
                         QPS,
                         recall_1,
                         recall_10,
                         recall_100)
                  << std::endl;

        std::shared_ptr<Infinity> infinity = Infinity::LocalConnect();
        QueryResult cache_result = infinity->ShowCache();

        auto &vectors = cache_result.result_table_->GetDataBlockById(0)->column_vectors_;
        std::cout << "columns: " << vectors.size() << std::endl;
        auto column1 = reinterpret_cast<const u64 *>(vectors[1]->data().get());
        auto column2 = reinterpret_cast<const u64 *>(vectors[2]->data().get());
        auto column3 = reinterpret_cast<const u64 *>(vectors[3]->data().get());
        auto column4 = reinterpret_cast<const double *>(vectors[4]->data().get());

        std::cout << "Cache db, items: " << column1[0] << ", hits: " << column2[0] << ", request: " << column3[0] << ", hit rate: " << column4[0]
                  << std::endl;
        std::cout << "Cache table, items: " << column1[1] << ", hits: " << column2[1] << ", request: " << column3[1] << ", hit rate: " << column4[1]
                  << std::endl;
        std::cout << "Cache index, items: " << column1[2] << ", hits: " << column2[2] << ", request: " << column3[2] << ", hit rate: " << column4[2]
                  << std::endl;
    }

    Infinity::LocalUnInit();
}
