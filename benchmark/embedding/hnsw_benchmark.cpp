//
// Created by jinhai on 23-5-1.
//

#define ANKERL_NANOBENCH_IMPLEMENT

#include "hnswlib/hnswlib.h"
#include "base_profiler.h"
#include "helper.h"

static const char *sift1m_base = "./test/data/fvecs/sift_base.fvecs";
static const char *sift1m_query = "./test/data/fvecs/sift_query.fvecs";
static const char *sift1m_ground_truth = "./test/data/fvecs/sift_groundtruth.ivecs";
static const char *hnsw_index_l2_name = "./tmp/the.hnsw";

using namespace infinity;

auto
main() -> int {
    size_t dimension;
    size_t embedding_count;
    size_t M = 16;
    size_t ef_construction = 200;
    float* input_embeddings = nullptr;
    {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        input_embeddings = fvecs_read(sift1m_base, &dimension, &embedding_count);
        profiler.End();
        std::cout << "Load sift1M base data: " << profiler.ElapsedToString() << std::endl;
    }

    assert(dimension == 128 || !"embedding dimension isn't 128");
    assert(embedding_count == 1000000 || !"embedding size isn't 1000000");
    hnswlib::L2Space l2space(dimension);
    hnswlib::HierarchicalNSW<float>* hnsw_index = nullptr;

    std::ifstream f(hnsw_index_l2_name);
    if(f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&l2space, hnsw_index_l2_name);
    } else {
        size_t max_elements = 1000000; // create index
        hnsw_index = new hnswlib::HierarchicalNSW<float>(&l2space, max_elements, M, ef_construction);

        infinity::BaseProfiler profiler;
        profiler.Begin();
        // insert data into index
        for(size_t idx = 0; idx < embedding_count; ++idx) {
            hnsw_index->addPoint(input_embeddings + idx * dimension, idx);
            if (idx % 10000 == 0) {
                std::cout << idx << ", " << get_current_rss() / 1000000 << " MB, " << profiler.ElapsedToString() << std::endl;
            }
        }
        profiler.End();
        std::cout << "Insert data cost: " << profiler.ElapsedToString()
                  << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        hnsw_index->saveIndex(hnsw_index_l2_name);
    }

    size_t number_of_queries;
    float* queries;
    {
        size_t dim;
        infinity::BaseProfiler profiler;
        profiler.Begin();
        queries = fvecs_read(sift1m_query, &dim, &number_of_queries);
        assert(dimension == dim || !"query does not have same dimension as train set");
        profiler.End();
        std::cout << "Load sift1M query data: " << profiler.ElapsedToString() << std::endl;
    }

    size_t top_k;         // nb of results per query in the GT
    size_t* ground_truth; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    {
        // load ground-truth and convert int to long
        size_t nq2;
        infinity::BaseProfiler profiler;
        profiler.Begin();

        int* gt_int = ivecs_read(sift1m_ground_truth, &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth = new size_t[top_k * number_of_queries];
        for(int i = 0; i < top_k * number_of_queries; ++i) {
            ground_truth[i] = gt_int[i];
        }
        delete[] gt_int;
        profiler.End();
        std::cout << "Load sift1M ground truth and load row id: " << profiler.ElapsedToString() << std::endl;
    }

    {
        // Query and get result;
        size_t n_valid = 0;
        hnsw_index->setEf(ef_construction);
        infinity::BaseProfiler profiler;
        profiler.Begin();
        for(int i = 0; i < number_of_queries; i++) {
            std::priority_queue<std::pair<float, hnswlib::labeltype>> result
                    = hnsw_index->searchKnn(queries + i * dimension, top_k);
            assert(top_k == result.size() || !"incorrect topk value");

            std::unordered_set<size_t> ground_truth_set;
            for(int j = 0; j < top_k; ++j) {
                ground_truth_set.insert(ground_truth[i * top_k + j]);
            }

            for(int j = 0; j < top_k; ++j) {
                if(ground_truth_set.contains(result.top().second)) {
                    ++n_valid;
                }
                result.pop();
            }
        }
        profiler.End();
        printf("Recall = %.4f, Spend: %s\n",
               n_valid / float(top_k * number_of_queries),
               profiler.ElapsedToString().c_str());
    }

    {
        delete[] input_embeddings;
        delete[] queries;
        delete[] ground_truth;
        delete hnsw_index;
    }

    return 0;
}

// For debug hnswlib
// void DumpGraph() {
//     std::fstream fo("./tmp/the_graph.txt", std::ios::out);
//     fo << std::endl << "---------------------------------------------" << std::endl;
//     fo << "M_: " << M_ << std::endl;
//     fo << "Mmax_: " << maxM_ << std::endl;
//     fo << "Mmax0_: " << maxM0_ << std::endl;
//     fo << "ef_: " << ef_ << std::endl;
//     fo << "ef_construction_: " << ef_construction_ << std::endl;
//     fo << "dim_: " << *(size_t *)(dist_func_param_) << std::endl;
//     fo << std::endl;

//     fo << "current element number: " << cur_element_count << std::endl;
//     fo << "max layer: " << maxlevel_ << std::endl;

//     std::vector<std::vector<tableint>> layer2vertex(maxlevel_ + 1);
//     for (tableint v = 0; v < cur_element_count; ++v) {
//         for (size_t layer = 0; layer <= element_levels_[v]; ++layer) {
//             layer2vertex[layer].emplace_back(v);
//         }
//     }
//     for (size_t layer = 0; layer <= maxlevel_; ++layer) {
//         fo << "layer " << layer << std::endl;
//         for (tableint v : layer2vertex[layer]) {
//             fo << v << ": ";
//             linklistsizeint *ll_cur = get_linklist_at_level(v, layer);
//             int size = getListCount(ll_cur);
//             tableint *data = (tableint *)(ll_cur + 1);
//             for (int j = 0; j < size; j++) {
//                 fo << data[j] << ", ";
//             }
//             fo << std::endl;
//         }
//     }
//     fo << "---------------------------------------------" << std::endl;
// }