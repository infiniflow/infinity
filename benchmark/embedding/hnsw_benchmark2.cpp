#include "base_profiler.h"
#include "helper.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <thread>

import stl;
import hnsw_alg;
import local_file_system;
import file_system_type;
import file_system;
import plain_store;
import lvq_store;
import dist_func_l2;
import dist_func_ip;
import compilation_config;

using namespace infinity;

int main() {
    String base_file = String(benchmark_data_path()) + "/sift/base.fvecs";
    String query_file = String(benchmark_data_path()) + "/sift/query.fvecs";
    String groundtruth_file = String(benchmark_data_path()) + "/sift/l2_groundtruth.ivecs";

    LocalFileSystem fs;
    std::string save_dir = tmp_data_path();

    size_t dimension = 128;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t embedding_count = 1000000;
    size_t test_top = 100;
    const int thread_n = 1;

    using LabelT = uint64_t;

    // using Hnsw = KnnHnsw<float, LabelT, PlainStore<float>, PlainL2Dist<float>>;
    // std::tuple<> init_args = {};
    // std::string save_place = save_dir + "/my_sift_plain_l2.hnsw";

    using Hnsw = KnnHnsw<float, LabelT, LVQStore<float, int8_t, LVQL2Cache<float, int8_t>>, LVQL2Dist<float, int8_t>>;
    SizeT init_args = {0};
    std::string save_place = save_dir + "/my_sift_lvq8_l2.hnsw";

    // using Hnsw = KnnHnsw<float, LabelT, PlainStore<float>, PlainIPDist<float>>;
    // std::tuple<> init_args = {};
    // std::string save_place = save_dir + "/my_sift_plain_ip.hnsw";

    // using Hnsw = KnnHnsw<float, LabelT, LVQStore<float, int8_t, LVQIPCache<float, int8_t>>, LVQIPDist<float, int8_t>>;
    // SizeT init_args = {0};
    // std::string save_place = save_dir + "/my_sift_lvq8_ip.hnsw";

    std::unique_ptr<Hnsw> knn_hnsw = nullptr;

    std::ifstream f(save_place);
    if (!f.good()) {
        std::cout << "Build index" << std::endl;

        size_t dim = -1;
        size_t eb_cnt = -1;
        float *input_embeddings = fvecs_read(base_file.c_str(), &dim, &eb_cnt);
        assert(dimension == dim || !"embedding dimension isn't correct");
        assert(embedding_count == eb_cnt || !"embedding size isn't correct");

        knn_hnsw = Hnsw::Make(embedding_count, dimension, M, ef_construction, init_args);

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() << "B" << std::endl;
        profiler.Begin();

        if (false) {
            for (size_t idx = 0; idx < embedding_count; ++idx) {
                // insert data into index
                const float *query = input_embeddings + idx * dimension;
                knn_hnsw->Insert(query, idx);
                if (idx % 100000 == 0) {
                    std::cout << idx << ", " << get_current_rss() << " B, " << profiler.ElapsedToString() << std::endl;
                }
            }
        } else {
            auto labels = std::make_unique<LabelT[]>(embedding_count);
            std::iota(labels.get(), labels.get() + embedding_count, 0);
            knn_hnsw->Insert(input_embeddings, labels.get(), embedding_count);
        }

        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() << "B" << std::endl;

        delete[] input_embeddings;

        uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_place, file_flags, FileLockType::kWriteLock);
        knn_hnsw->Save(*file_handler);
        file_handler->Close();
    } else {
        std::cout << "Load index from " << save_place << std::endl;

        uint8_t file_flags = FileFlags::READ_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_place, file_flags, FileLockType::kReadLock);

        knn_hnsw = Hnsw::Load(*file_handler, init_args);
    }

    size_t number_of_queries;
    const float *queries = nullptr;
    {
        size_t dim = -1;
        queries = const_cast<const float *>(fvecs_read(query_file.c_str(), &dim, &number_of_queries));
        assert(dimension == dim || !"query does not have same dimension as train set");
    }

    size_t top_k;                           // nb of results per query in the GT
    Vector<HashSet<int>> ground_truth_sets; // number_of_queries * top_k matrix of ground-truth nearest-neighbors

    {
        // size_t *ground_truth;
        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(groundtruth_file.c_str(), &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");
        assert(top_k >= test_top || !"dataset does not provide enough ground truth data");

        ground_truth_sets.resize(number_of_queries);
        for (int i = 0; i < number_of_queries; ++i) {
            for (int j = 0; j < test_top; ++j) {
                ground_truth_sets[i].insert(gt_int[i * top_k + j]);
            }
        }
    }

    infinity::BaseProfiler profiler;
    int round = 10;
    Vector<MaxHeap<Pair<float, LabelT>>> results;
    results.reserve(number_of_queries);
    std::cout << "thread number: " << thread_n << std::endl;
    for (int ef = 100; ef <= 300; ef += 25) {
        knn_hnsw->SetEf(ef);
        int correct = 0;
        int sum_time = 0;
        for (size_t i = 0; i < round; ++i) {
            std::atomic_int idx(0);
            std::vector<std::thread> threads;
            profiler.Begin();
            for (int i = 0; i < thread_n; ++i) {
                threads.emplace_back([&]() {
                    while (true) {
                        int cur_idx = idx.fetch_add(1);
                        if (cur_idx >= number_of_queries) {
                            break;
                        }
                        const float *query = queries + cur_idx * dimension;
                        MaxHeap<Pair<float, LabelT>> result = knn_hnsw->KnnSearch(query, test_top);
                        results[cur_idx] = std::move(result);
                    }
                });
            }
            for (auto &thread : threads) {
                thread.join();
            }
            profiler.End();
            if (i == 0) {
                for (int idx = 0; idx < number_of_queries; ++idx) {
                    auto &result = results[idx];
                    while (!result.empty()) {
                        if (ground_truth_sets[idx].contains(result.top().second)) {
                            ++correct;
                        }
                        result.pop();
                    }
                }
                printf("Recall = %.4f\n", correct / float(test_top * number_of_queries));
            }
            sum_time += profiler.ElapsedToMs();
        }
        sum_time /= round;
        printf("ef = %d, Spend: %d\n", ef, sum_time);

        std::cout << "----------------------------" << std::endl;
    }

    delete[] queries;
}
