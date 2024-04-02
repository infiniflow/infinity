

/*
 * This benchmark includes:
 * 1. ann_ivfflatl2 (train, add, search, save to / load from file)
 * 2. ann_ivfflatip (train, add, search, save to / load from file)
 */

#include "base_profiler.h"
#include "helper.h"
#include <iomanip>
#include <iostream>

import stl;
import index_base;
import ann_ivf_flat;
import annivfflat_index_data;

import local_file_system;
import knn_expr;

static const char *sift1m_train = "/benchmark_dataset/sift1M/sift_learn.fvecs";
static const char *sift1M_base = "/benchmark_dataset/sift1M/sift_base.fvecs";
static const char *sift1M_query = "/benchmark_dataset/sift1M/sift_query.fvecs";
static const char *sift1M_groundtruth = "/benchmark_dataset/sift1M/sift_groundtruth.ivecs";

static const char *deep10M_train = "/benchmark_dataset/deep10M/deep10M_base.fvecs";
static const char *deep10M_base = "/benchmark_dataset/deep10M/deep10M_base.fvecs";
static const char *deep10M_query = "/benchmark_dataset/deep10M/deep10M_query.fvecs";
static const char *deep10M_groundtruth = "/benchmark_dataset/deep10M/deep10M_groundtruth.ivecs";

static const char *ivfflat_index_name_prefix = "/benchmark_save_index/ivfflat_index/";
static const char *ivfflat_index_name_suffix = "/ivfflat_index.save";

using namespace infinity;

template <typename T>
std::unique_ptr<T[]> load_data(const std::string &filename, size_t &num, size_t &dim) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
    }
    int dim_;
    in.read((char *)&dim_, 4);
    dim = (size_t)dim_;
    in.seekg(0, std::ios::end);
    auto ss = in.tellg();
    num = ((size_t)ss) / (dim + 1) / 4;
    auto data = std::make_unique<T[]>(num * dim);

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char *)(data.get() + i * dim), dim * 4);
    }
    in.close();
    return data;
}

template <typename T1, typename T2>
void output_recall_top_k(const unsigned top_k,
                         std::vector<std::vector<T1>> &results,
                         const std::vector<std::unordered_set<T2>> &groundtruth_1,
                         const std::vector<std::unordered_set<T2>> &groundtruth_10,
                         const std::vector<std::unordered_set<T2>> &groundtruth_100) {
    if (top_k != 100) {
        std::cerr << "top_k can only be 100" << std::endl;
        exit(-1);
    }
    size_t gt_cnt_1 = 0, gt_cnt_10 = 0, gt_cnt_100 = 0;
    for (unsigned i = 0; i < results.size(); i++) {
        for (size_t cnt = 0; cnt < 100; ++cnt) {
            auto id = results[i][cnt];
            if (cnt < 1 && groundtruth_1[i].contains(id)) {
                ++gt_cnt_1;
            }
            if (cnt < 10 && groundtruth_10[i].contains(id)) {
                ++gt_cnt_10;
            }
            if (cnt < 100 && groundtruth_100[i].contains(id)) {
                ++gt_cnt_100;
            }
        }
    }
    std::cout << "R@1:   " << std::fixed << std::setprecision(3) << float(gt_cnt_1) / float(results.size() * 1) << std::endl;
    std::cout << "R@10:  " << std::fixed << std::setprecision(3) << float(gt_cnt_10) / float(results.size() * 10) << std::endl;
    std::cout << "R@100: " << std::fixed << std::setprecision(3) << float(gt_cnt_100) / float(results.size() * 100) << std::endl;
}

int main() {
    const char *homeDir = getenv("HOME");
    std::string train_file = homeDir;
    std::string base_file = homeDir;
    std::string query_file = homeDir;
    std::string groundtruth_file = homeDir;
    std::string data_type;
    size_t dimension = {};
    size_t base_counts = {};
    size_t centroids = 1000;
    size_t n_probes_begin = 1;
    size_t n_probes_end = 10;
    size_t n_probes_step = 1;
    // let user choose to test sift1M or deep10M
    enum test_type { invalid = 0, sift1M = 1, deep10M = 2 };
    std::cout << "please choose to test sift1M or deep10M, input 1 or 2" << std::endl;
    std::cout << "1. sift1M" << std::endl;
    std::cout << "2. deep10M" << std::endl;
    int choose = 0;
    std::cin >> choose;
    switch (choose) {
        case test_type::sift1M: {
            data_type = "sift1M";
            dimension = 128;
            base_counts = 1'000'000;
            train_file += sift1m_train;
            base_file += sift1M_base;
            query_file += sift1M_query;
            groundtruth_file += sift1M_groundtruth;
            break;
        }
        case test_type::deep10M: {
            data_type = "deep10M";
            dimension = 96;
            base_counts = 10'000'000;
            train_file += deep10M_train;
            base_file += deep10M_base;
            query_file += deep10M_query;
            groundtruth_file += deep10M_groundtruth;
            break;
        }
        default:
            std::cout << "invalid input, exit." << std::endl;
            return -1;
    }
    MetricType metric = MetricType::kInvalid;
    std::string metric_str;
    // let user choose metric
    std::cout << "please choose metric, input 1 or 2" << std::endl;
    std::cout << "1. L2" << std::endl;
    std::cout << "2. IP" << std::endl;
    int choose_metric = 0;
    std::cin >> choose_metric;
    switch (choose_metric) {
        case 1: {
            metric = MetricType::kMetricL2;
            metric_str = "L2";
            break;
        }
        case 2: {
            metric = MetricType::kMetricInnerProduct;
            metric_str = "IP";
            break;
        }
        default:
            std::cout << "invalid input, exit." << std::endl;
            return -1;
    }
    // let user choose centroids, n_probes_begin, n_probes_end, n_probes_step
    std::cout << "please input centroids, n_probes_begin, n_probes_end, n_probes_step. input 0 to use default values" << std::endl;
    size_t choose_centroids = 0, choose_n_probes_begin = 0, choose_n_probes_end = 0, choose_n_probes_step = 0;
    std::cin >> choose_centroids >> choose_n_probes_begin >> choose_n_probes_end >> choose_n_probes_step;
    if (choose_centroids != 0) {
        centroids = choose_centroids;
    }
    if (choose_n_probes_begin != 0) {
        n_probes_begin = choose_n_probes_begin;
    }
    if (choose_n_probes_end != 0) {
        n_probes_end = choose_n_probes_end;
    }
    if (choose_n_probes_step != 0) {
        n_probes_step = choose_n_probes_step;
    }
    std::string build_parameter = "." + data_type + "." + metric_str + ".C." + std::to_string(centroids);
    std::string ivfflat_index_name =
        std::string(homeDir) + std::string(ivfflat_index_name_prefix) + data_type + std::string(ivfflat_index_name_suffix) + build_parameter;
    std::cout << "data_type: " << data_type << std::endl;
    std::cout << "metric: " << metric_str << std::endl;
    std::cout << "dimension: " << dimension << std::endl;
    std::cout << "base_counts: " << base_counts << std::endl;
    std::cout << "centroids: " << centroids << std::endl;
    std::cout << "n_probes_begin: " << n_probes_begin << std::endl;
    std::cout << "n_probes_end: " << n_probes_end << std::endl;
    std::cout << "n_probes_step: " << n_probes_step << std::endl;
    std::cout << "ivfflat_index_name: " << ivfflat_index_name << std::endl;
    UniquePtr<AnnIVFFlatIndexData<f32>> ann_index_data;

    std::ifstream f(ivfflat_index_name);
    if (f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        ann_index_data = AnnIVFFlatIndexData<f32>::LoadIndex(ivfflat_index_name, MakeUnique<LocalFileSystem>());
    } else {
        BaseProfiler profiler;
        {
            size_t embedding_count;
            std::unique_ptr<f32[]> input_embeddings;
            {
                size_t dim;
                BaseProfiler profiler_in;
                profiler_in.Begin();
                input_embeddings = load_data<f32>(base_file, embedding_count, dim);
                profiler_in.End();
                assert(dimension == dim || !"embedding dimension wrong");
                assert(base_counts == embedding_count || !"embedding count wrong");
                std::cout << "Load base data: " << profiler_in.ElapsedToString() << std::endl;
                // output embedding_count, dimension
                std::cout << "embedding_count: " << embedding_count << std::endl;
                std::cout << "dimension: " << dim << std::endl;
            }
            size_t train_count;
            std::unique_ptr<f32[]> input_train;
            f32 *input_train_ptr = nullptr;
            if (train_file == base_file) {
                train_count = embedding_count;
                input_train_ptr = input_embeddings.get();
                std::cout << "Use base data as train data" << std::endl;
            } else {
                size_t dim;
                BaseProfiler profiler_in;
                profiler_in.Begin();
                input_train = load_data<f32>(train_file, train_count, dim);
                profiler_in.End();
                input_train_ptr = input_train.get();
                assert(dimension == dim || !"embedding dimension wrong");
                std::cout << "Load train data: " << profiler_in.ElapsedToString() << std::endl;
                // output embedding_count, dimension
                std::cout << "train_count: " << train_count << std::endl;
                std::cout << "dimension: " << dim << std::endl;
            }
            profiler.Begin();
            switch (metric) {
                case MetricType::kMetricL2: {
                    ann_index_data =
                        AnnIVFFlatL2<f32>::CreateIndex(dimension, train_count, input_train_ptr, base_counts, input_embeddings.get(), centroids);
                    break;
                }
                case MetricType::kMetricInnerProduct: {
                    ann_index_data =
                        AnnIVFFlatIP<f32>::CreateIndex(dimension, train_count, input_train_ptr, base_counts, input_embeddings.get(), centroids);
                    break;
                }
                default:
                    std::cout << "invalid metric" << std::endl;
                    return -1;
            }
            profiler.End();
            std::cout << "before clearing input vector, memory cost: " << get_current_rss() / (1024 * 1024) << " MiB" << std::endl;
        }
        std::cout << "Build index cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / (1024 * 1024) << " MiB"
                  << std::endl;
        ann_index_data->SaveIndex(ivfflat_index_name, MakeUnique<LocalFileSystem>());
    }

    size_t number_of_queries;
    std::unique_ptr<f32[]> queries;
    {
        size_t dim;
        BaseProfiler profiler;
        profiler.Begin();
        queries = load_data<f32>(query_file, number_of_queries, dim);
        profiler.End();
        assert(dimension == dim || !"query does not have same dimension as train set");
        std::cout << "Load query data: " << profiler.ElapsedToString() << std::endl;
    }

    size_t top_k{}; // nb of results per query in the GT
    std::vector<std::unordered_set<int>> ground_truth_sets_1, ground_truth_sets_10,
        ground_truth_sets_100; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    {
        // load ground-truth and convert int to long
        size_t nq2;
        BaseProfiler profiler;
        profiler.Begin();

        auto gt_int = load_data<int>(groundtruth_file, nq2, top_k);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth_sets_1.resize(number_of_queries);
        ground_truth_sets_10.resize(number_of_queries);
        ground_truth_sets_100.resize(number_of_queries);
        for (size_t i = 0; i < number_of_queries; ++i) {
            for (size_t j = 0; j < top_k; ++j) {
                auto gt = gt_int[i * top_k + j];
                if (j < 1) {
                    ground_truth_sets_1[i].insert(gt);
                }
                if (j < 10) {
                    ground_truth_sets_10[i].insert(gt);
                }
                if (j < 100) {
                    ground_truth_sets_100[i].insert(gt);
                }
            }
        }
        profiler.End();
        std::cout << "Load ground truth: " << profiler.ElapsedToString() << std::endl;
    }
    for (size_t n_probes = n_probes_begin; n_probes <= n_probes_end; n_probes += n_probes_step) {
        BaseProfiler profiler;
        std::chrono::duration<double> t_val = std::chrono::duration<double>::zero();
        Vector<Vector<u32>> results;
        auto search_f = [&]<typename AnnIVFFlat>() {
            AnnIVFFlat test_ivf(queries.get(), number_of_queries, top_k, dimension, EmbeddingDataType::kElemFloat);
            test_ivf.Begin();
            profiler.Begin();
            auto t0 = std::chrono::high_resolution_clock::now();
            test_ivf.Search(ann_index_data.get(), 0, n_probes);
            t_val += std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t0);
            profiler.End();
            test_ivf.End();
            auto ID = test_ivf.GetIDs();
            results.resize(number_of_queries);
            for (size_t i = 0; i < number_of_queries; ++i) {
                results[i].resize(top_k);
                for (size_t j = 0; j < top_k; ++j) {
                    results[i][j] = ID[i * top_k + j].segment_offset_;
                }
            }
        };
        switch (metric) {
            case MetricType::kMetricL2: {
                search_f.operator()<AnnIVFFlatL2<f32>>();
                break;
            }
            case MetricType::kMetricInnerProduct: {
                search_f.operator()<AnnIVFFlatIP<f32>>();
                break;
            }
            default:
                std::cout << "invalid metric" << std::endl;
                return -1;
        }
        std::cout << "\nn_probes = " << n_probes << ", Spend: " << profiler.ElapsedToString() << std::endl;
        std::cout << "time = " << std::fixed << std::setprecision(3) << t_val.count() << " s" << std::endl;
        std::cout << "QPS = " << std::fixed << std::setprecision(3) << f64(number_of_queries) / t_val.count() << std::endl;
        output_recall_top_k(top_k, results, ground_truth_sets_1, ground_truth_sets_10, ground_truth_sets_100);
    }
    return 0;
}
