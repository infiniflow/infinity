//
// Created by jinhai on 23-5-1.
//

#define ANKERL_NANOBENCH_IMPLEMENT

#include "nanobench.h"

#include "common/types/internal_types.h"
#include "faiss/index_factory.h"
#include "main/profiler/base_profiler.h"
#include "faiss/IndexFlat.h"
#include "helper.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/index_io.h"

using namespace infinity;

static const char* sift1m_base = "/home/jinhai/Documents/data/sift1M/sift_base.fvecs";
static const char* sift1m_train = "/home/jinhai/Documents/data/sift1M/sift_learn.fvecs";
static const char* sift1m_query = "/home/jinhai/Documents/data/sift1M/sift_query.fvecs";
static const char* sift1m_ground_truth = "/home/jinhai/Documents/data/sift1M/sift_groundtruth.ivecs";
static const char* ivf_index_name = "ivf_index.bin";
static const char* flat_index_name = "flat_index.bin";

static float*
float_vector_read(const char* fname, size_t* dimension, size_t* row_count, const char* comment) {
    infinity::BaseProfiler profiler;
    profiler.Begin();
    float* float_vec = fvecs_read(fname, dimension, row_count);
    profiler.End();
    if(comment != nullptr) {
        std::cout << "Load " << comment << ": " << profiler.ElapsedToString() << ", "
                  << get_current_rss() / 1000000 << " MB" << std::endl;
    }
    return float_vec;
}

static int*
integer_vector_read(const char* fname, size_t* dimension, size_t* row_count, const char* comment) {
    infinity::BaseProfiler profiler;
    profiler.Begin();
    int* float_vec = ivecs_read(fname, dimension, row_count);
    profiler.End();
    if(comment != nullptr) {
        std::cout << "Load " << comment << ": " << profiler.ElapsedToString() << ", "
                  << get_current_rss() / 1000000 << " MB" << std::endl;
    }
    return float_vec;
}

void
benchmark_flat() {
    faiss::Index* index;
    int dimension = 128;
    {
        // Construct index
        infinity::BaseProfiler profiler;
        profiler.Begin();
        index = new faiss::IndexFlatL2(dimension);
        profiler.End();
        std::cout << "Create Flat index: " << profiler.ElapsedToString() << ", "
                  << get_current_rss() / 1000000 << " MB" << std::endl;
    }

    std::ifstream f(flat_index_name);
    if(f.good()) {
        // Found index file
        std::cout << "Found flat index file ... " << std::endl;
        index = faiss::read_index(flat_index_name);
    } else {
        // Read base vector data
        SizeT base_vector_row_count{0};
        SizeT base_vector_dimension{0};
        float* base_vectors{nullptr};
        {
            base_vectors = float_vector_read(sift1m_base, &base_vector_dimension, &base_vector_row_count, "Base Vector");
        }

        {
            // Build index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            index->add(base_vector_row_count, base_vectors);
            profiler.End();
            std::cout << "Insert Base Vector into index: " << profiler.ElapsedToString() << ", "
                      << get_current_rss() / 1000000 << " MB" << std::endl;
        }

        delete[] base_vectors;

        faiss::write_index(index, flat_index_name);
    }

    // Read query vector data
    SizeT query_vector_row_count{0};
    SizeT query_vector_dimension{0};
    float* query_vectors;
    {
        query_vectors = float_vector_read(sift1m_query, &query_vector_dimension, &query_vector_row_count, "Query Vector");
        assert(query_vector_dimension == base_vector_dimension || !"query does not have same dimension as base set");
    }

    // Read ground truth
    SizeT top_k{0};                // number of results per query in the GT
    i32*  ground_truth{nullptr};   // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    SizeT ground_truth_row_count{0};
    {
        // load ground-truth
        ground_truth = ivecs_read(sift1m_ground_truth, &top_k, &ground_truth_row_count);
        assert(query_vector_row_count == ground_truth_row_count || !"incorrect nb of ground truth entries");
    }

    {
        infinity::BaseProfiler profiler;
        profiler.Begin();
        faiss::idx_t* I = new faiss::idx_t[query_vector_row_count * top_k];
        float* D = new float[query_vector_row_count * top_k];

        index->search(query_vector_row_count, query_vectors, top_k, D, I);
        profiler.End();
        std::cout << "Search: " << profiler.ElapsedToString() << ", "
                  << get_current_rss() / 1000000 << " MB" << std::endl;

        // evaluate result by hand.
        int n_1 = 0, n_10 = 0, n_100 = 0;
        for (int i = 0; i < query_vector_row_count; i++) {
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
        printf("R@1 = %.4f\n", n_1 / float(query_vector_row_count));
        printf("R@10 = %.4f\n", n_10 / float(query_vector_row_count));
        printf("R@100 = %.4f\n", n_100 / float(query_vector_row_count));

        delete[] I;
        delete[] D;
    }

    {
        delete[] query_vectors;
        delete[] ground_truth;
        delete index;
    }
    std::cout << "Memory: " << get_current_rss() / 1000000 << " MB" << std::endl;
}

void
benchmark_ivfflat(bool l2) {
    int d = 128;
    size_t centroids_count = 4096; //
    faiss::IndexFlatL2 quantizer(d);
    faiss::Index* index = nullptr;

    std::ifstream f(ivf_index_name);
    if(f.good()) {
        // Found index file
        std::cout << "Found index file ... " << std::endl;
        index = faiss::read_index(ivf_index_name);
    } else {
        {
            // Construct index
            infinity::BaseProfiler profiler;
            profiler.Begin();
            if(l2) {
                index = new faiss::IndexIVFFlat(&quantizer, d, centroids_count, faiss::MetricType::METRIC_L2);
            } else {
                index = new faiss::IndexIVFFlat(&quantizer, d, centroids_count, faiss::MetricType::METRIC_INNER_PRODUCT);
            }

            profiler.End();
            std::cout << "Create Flat index: " << profiler.ElapsedToString() << std::endl;
        }

        // No index file
        std::cout << "No index file found: constructing ... " << std::endl;
        {
            size_t nb, d2;
            infinity::BaseProfiler profiler;
            profiler.Begin();
            float* xb = fvecs_read(sift1m_train, &d2, &nb);
            profiler.End();
            std::cout << "Load sift1M learn data: " << profiler.ElapsedToString() << std::endl;

            profiler.Begin();
            index->train(nb, xb);
            profiler.End();
            std::cout << "Train sift1M learn data into quantizer: " << profiler.ElapsedToString() << std::endl;
            delete[] xb;
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
            delete[] xb;
        }
        faiss::write_index(index, ivf_index_name);
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

        faiss::IndexIVFFlat* ivf_flat_index = (faiss::IndexIVFFlat*)index;

        ivf_flat_index->nprobe = 4; // Default value is 1;
        ivf_flat_index->search(number_of_queries, queries, top_k, D, I);
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
        delete[] ground_truth;
        delete[] queries;
    }
}

void
benchmark_ivf_single_cpu(bool l2) {

}

auto main () -> int {
    benchmark_flat();
//    benchmark_ivfflat(true);
    return 0;
}