#include "base_profiler.h"
#include "helper.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <thread>

import stl;
import hnsw_alg;
import hnsw_common;
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
    // String base_file = String(test_data_path()) + "/benchmark/text2image_10m/base.fvecs";
    // String query_file = String(test_data_path()) + "/benchmark/text2image_10m/query.fvecs";
    // String groundtruth_file = String(test_data_path()) + "/benchmark/text2image_10m/groundtruth.ivecs";

    String base_file = String(test_data_path()) + "/benchmark/sift_1m/sift_base.fvecs";
    String query_file = String(test_data_path()) + "/benchmark/sift_1m/sift_query.fvecs";
    String groundtruth_file = String(test_data_path()) + "/benchmark/sift_1m/sift_groundtruth.ivecs";

    LocalFileSystem fs;
    std::string save_dir = tmp_data_path();

    size_t dimension = 128;
    size_t M = 16;
    size_t ef_construction = 200;
    size_t embedding_count = 1000000;
    size_t test_top = 100;
    const int build_thread_n = 1;
    const int query_thread_n = 1;

    using LabelT = uint64_t;

    // using Hnsw = KnnHnsw<float, LabelT, PlainStore<float>, PlainL2Dist<float>>;
    // std::tuple<> init_args = {};
    // std::string save_place = save_dir + "/my_sift_plain_l2.hnsw";

    using Hnsw = KnnHnsw<float, LabelT, LVQStore<float, int8_t, LVQL2Cache<float, int8_t>>, LVQL2Dist<float, int8_t>>;
    SizeT init_args = {0};
    std::string save_place = save_dir + "/my_sift_lvq8_l2_1.hnsw";

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

        {
            std::cout << "Build thread number: " << build_thread_n << std::endl;
            auto labels = std::make_unique<LabelT[]>(embedding_count);
            std::iota(labels.get(), labels.get() + embedding_count, 0);
            VertexType start_i = knn_hnsw->StoreDataRaw(input_embeddings, labels.get(), embedding_count);
            delete[] input_embeddings;
            AtomicVtxType next_i = start_i;
            std::vector<std::thread> threads;
            for (int i = 0; i < build_thread_n; ++i) {
                threads.emplace_back([&]() {
                    while (true) {
                        VertexType cur_i = next_i.fetch_add(1);
                        if (cur_i >= VertexType(start_i + embedding_count)) {
                            break;
                        }
                        knn_hnsw->Build<true>(cur_i);
                        if (cur_i && cur_i % 10000 == 0) {
                            std::cout << "Inserted " << cur_i << " / " << embedding_count << std::endl;
                        }
                    }
                });
            }
            for (auto &thread : threads) {
                thread.join();
            }
        }

        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() << "B" << std::endl;

        uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_place, file_flags, FileLockType::kWriteLock);
        knn_hnsw->Save(*file_handler);
        file_handler->Close();
    } else {
        std::cout << "Load index from " << save_place << std::endl;

        uint8_t file_flags = FileFlags::READ_FLAG;
        std::unique_ptr<FileHandler> file_handler = fs.OpenFile(save_place, file_flags, FileLockType::kReadLock);

        knn_hnsw = Hnsw::Load(*file_handler, init_args);
        std::cout << "Loaded" << std::endl;

        // std::ofstream out("dump.txt");
        // knn_hnsw->Dump(out);
        // knn_hnsw->Check();
    }
    return 0;

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
        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(groundtruth_file.c_str(), &top_k, &nq2);
        assert(nq2 >= number_of_queries || !"incorrect nb of ground truth entries");
        assert(top_k >= test_top || !"dataset does not provide enough ground truth data");

        ground_truth_sets.resize(number_of_queries);
        for (size_t i = 0; i < number_of_queries; ++i) {
            for (size_t j = 0; j < test_top; ++j) {
                ground_truth_sets[i].insert(gt_int[i * top_k + j]);
            }
        }
    }

    infinity::BaseProfiler profiler;
    std::cout << "Start!" << std::endl;
    int round = 3;
    Vector<Vector<Pair<float, LabelT>>> results(number_of_queries);
    std::cout << "Query thread number: " << query_thread_n << std::endl;
    for (int ef = 100; ef <= 300; ef += 25) {
        knn_hnsw->SetEf(ef);
        int correct = 0;
        int sum_time = 0;
        for (int i = 0; i < round; ++i) {
            std::atomic_int idx(0);
            std::vector<std::thread> threads;
            profiler.Begin();
            for (int j = 0; j < query_thread_n; ++j) {
                threads.emplace_back([&]() {
                    while (true) {
                        int cur_idx = idx.fetch_add(1);
                        if (cur_idx >= (int)number_of_queries) {
                            break;
                        }
                        const float *query = queries + cur_idx * dimension;
                        auto result = knn_hnsw->KnnSearch<false>(query, test_top);
                        results[cur_idx] = std::move(result);
                    }
                });
            }
            for (auto &thread : threads) {
                thread.join();
            }
            profiler.End();
            if (i == 0) {
                for (size_t query_idx = 0; query_idx < number_of_queries; ++query_idx) {
                    for (const auto &[_, label] : results[query_idx]) {
                        if (ground_truth_sets[query_idx].contains(label)) {
                            ++correct;
                        }
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
