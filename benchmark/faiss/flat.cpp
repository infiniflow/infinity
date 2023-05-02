//
// Created by jinhai on 23-5-1.
//

#define ANKERL_NANOBENCH_IMPLEMENT

#include "nanobench.h"

#include "faiss/index_factory.h"
#include "main/profiler/base_profiler.h"
#include "faiss/IndexFlat.h"
#include "helper.h"

static const char* sift1m_base = "/home/jinhai/Documents/data/sift1M/sift_base.fvecs";
static const char* sift1m_query = "/home/jinhai/Documents/data/sift1M/sift_query.fvecs";
static const char* sift1m_ground_truth = "/home/jinhai/Documents/data/sift1M/sift_groundtruth.ivecs";

void
benchmark_flat_l2() {
    faiss::Index* index;
    int d = 128;
    {
        // Construct index

        infinity::BaseProfiler profiler;
        profiler.Begin();
        index = new faiss::IndexFlatL2(d);
        profiler.End();
        std::cout << "Create Flat index: " << profiler.ElapsedToString() << std::endl;
    }

    {
        size_t nb, d2;
        infinity::BaseProfiler profiler;
        profiler.Begin();
        float* xb = fvecs_read(sift1m_base, &d2, &nb);
        profiler.End();
        std::cout << "Load sift1M base data: " << profiler.ElapsedToString() << std::endl;

        profiler.Begin();
        index->add(nb, xb);
        profiler.End();
        std::cout << "Insert sift1M base data into index: " << profiler.ElapsedToString() << std::endl;
    }

    size_t number_of_queries;
    float* queries;

    {
        size_t d2;
        infinity::BaseProfiler profiler;
        profiler.Begin();
        queries = fvecs_read(sift1m_query, &d2, &number_of_queries);
        assert(d == d2 || !"query does not have same dimension as train set");
        profiler.End();
        std::cout << "Load sift1M query data: " << profiler.ElapsedToString() << std::endl;
    }

    size_t top_k;         // nb of results per query in the GT
    faiss::idx_t* ground_truth; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    {
        // load ground-truth and convert int to long
        size_t nq2;
        infinity::BaseProfiler profiler;
        profiler.Begin();

        int* gt_int = ivecs_read(sift1m_ground_truth, &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth = new faiss::idx_t[top_k * number_of_queries];
        for (int i = 0; i < top_k * number_of_queries; ++ i) {
            ground_truth[i] = gt_int[i];
        }
        delete[] gt_int;
        profiler.End();
        std::cout << "Load sift1M ground truth and load row id: " << profiler.ElapsedToString() << std::endl;
    }

    {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        faiss::idx_t* I = new faiss::idx_t[number_of_queries * top_k];
        float* D = new float[number_of_queries * top_k];

        index->search(number_of_queries, queries, top_k, D, I);
        profiler.End();
        std::cout << "Search: " << profiler.ElapsedToString() << std::endl;

        // evaluate result by hand.
        int n_1 = 0, n_10 = 0, n_100 = 0;
        for (int i = 0; i < number_of_queries; i++) {
            int gt_nn = ground_truth[i * top_k];
            for (int j = 0; j < top_k; j++) {
                if (I[i * top_k + j] == gt_nn) {
                    if (j < 1)
                        n_1++;
                    if (j < 10)
                        n_10++;
                    if (j < 100)
                        n_100++;
                }
            }
        }
        printf("R@1 = %.4f\n", n_1 / float(number_of_queries));
        printf("R@10 = %.4f\n", n_10 / float(number_of_queries));
        printf("R@100 = %.4f\n", n_100 / float(number_of_queries));

        delete[] I;
        delete[] D;
    }

    {
        delete index;
    }
}

void
benchmark_index_factory_flat() {
    faiss::Index* index;
    int d = 128;
    const char *index_key = "Flat";

    {
        // Construct index
        infinity::BaseProfiler profiler;
        profiler.Begin();
        index = faiss::index_factory(d, index_key);
        profiler.End();
        std::cout << profiler.ElapsedToString() << std::endl;
        delete index;
    }

}

auto main () -> int {
    benchmark_flat_l2();
    return 0;
}