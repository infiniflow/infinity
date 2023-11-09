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

#include "unit_test/base_test.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/time.h>

#include <faiss/AutoTune.h>
#include <faiss/index_factory.h>

#include "hnswlib/hnswlib.h"

// import stl;
import compilation_config;

class KnnBenchTest : public BaseTest {};

size_t get_current_rss() {
    // Only for linux, copy from hnswlib
    long rss = 0L;
    FILE *fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r")) == NULL)
        return (size_t)0L; /* Can't open? */
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
        fclose(fp);
        return (size_t)0L; /* Can't read? */
    }
    fclose(fp);
    return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);
}

/**
 * To run this demo, please download the ANN_SIFT1M dataset from
 *
 *   http://corpus-texmex.irisa.fr/
 *
 * and unzip it to the sudirectory sift1M.
 **/

/*****************************************************
 * I/O functions for fvecs and ivecs
 *****************************************************/

float *fvecs_read(const char *fname, size_t *d_out, size_t *n_out) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
    int d;
    fread(&d, 1, sizeof(int), f);
    assert((d > 0 && d < 1000000) || !"unreasonable dimension");
    fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
    assert(sz % ((d + 1) * 4) == 0 || !"weird file size");
    size_t n = sz / ((d + 1) * 4);

    *d_out = d;
    *n_out = n;
    float *x = new float[n * (d + 1)];
    size_t nr = fread(x, sizeof(float), n * (d + 1), f);
    assert(nr == n * (d + 1) || !"could not read whole file");

    // shift array to remove row headers
    for (size_t i = 0; i < n; i++)
        memmove(x + i * d, x + 1 + i * (d + 1), d * sizeof(*x));

    fclose(f);
    return x;
}

// not very clean, but works as long as sizeof(int) == sizeof(float)
int *ivecs_read(const char *fname, size_t *d_out, size_t *n_out) { return (int *)fvecs_read(fname, d_out, n_out); }

double elapsed() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

const std::string sift_learn_name = "sift_learn.fvecs";
const std::string sift_base_name = "sift_base.fvecs";
const std::string sift_query_name = "sift_query.fvecs";
const std::string sift_ground_truth_name = "sift_groundtruth.ivecs";

bool test_file_existence(const std::string &data_dir) {
    std::string sift_file_path = data_dir + "/sift1M/";
    std::error_code error_code;
    {
        std::filesystem::path p{sift_file_path + sift_learn_name};
        bool is_exists = std::filesystem::exists(p, error_code);
        if (!is_exists) {
            std::cout << sift_file_path + sift_learn_name << " doesn't exist." << std::endl;
            return false;
        }
    }

    {
        std::filesystem::path p{sift_file_path + sift_base_name};
        bool is_exists = std::filesystem::exists(p, error_code);
        if (!is_exists) {
            std::cout << sift_file_path + sift_base_name << " doesn't exist." << std::endl;
            return false;
        }
    }

    {
        std::filesystem::path p{sift_file_path + sift_query_name};
        bool is_exists = std::filesystem::exists(p, error_code);
        if (!is_exists) {
            std::cout << sift_file_path + sift_query_name << " doesn't exist." << std::endl;
            return false;
        }
    }

    {
        std::filesystem::path p{sift_file_path + sift_ground_truth_name};
        bool is_exists = std::filesystem::exists(p, error_code);
        if (!is_exists) {
            std::cout << sift_file_path + sift_ground_truth_name << " doesn't exist." << std::endl;
            return false;
        }
    }

    return true;
}

void faiss_bench(const char *index_key, const char *test_data_path, int64_t to_query) {
    if (index_key == nullptr) {
        printf("No index key\n");
        return;
    }

    if (test_data_path == nullptr) {
        printf("invalid test data path\n");
        return;
    }

    if (!test_file_existence(test_data_path)) {
        return;
    }

    std::string sift_file_path = std::string(test_data_path) + "/sift1M/";

    double t0 = elapsed();

    // this is typically the fastest one.
    //    const char* index_key = "IVF4096,Flat";

    // these have better memory usage
    // const char *index_key = "Flat";
    // const char *index_key = "PQ32";
    // const char *index_key = "PCA80,Flat";
    // const char *index_key = "IVF4096,PQ8+16";
    // const char *index_key = "IVF4096,PQ32";
    // const char *index_key = "IMI2x8,PQ32";
    // const char *index_key = "IMI2x8,PQ8+16";
    // const char *index_key = "OPQ16_64,IMI2x8,PQ8+16";

    faiss::Index *index;

    size_t dimension;

    {
        printf("[%.3f s] Loading train set\n", elapsed() - t0);

        size_t training_vector_count;

        std::string sift_learn_fvecs = sift_file_path + sift_learn_name;
        float *xt = fvecs_read(sift_learn_fvecs.c_str(), &dimension, &training_vector_count);

        printf("[%.3f s] Preparing index \"%s\" dimension=%ld\n", elapsed() - t0, index_key, dimension);
        index = faiss::index_factory(dimension, index_key);

        printf("[%.3f s] Training on %ld vectors\n", elapsed() - t0, training_vector_count);

        index->train(training_vector_count, xt);
        delete[] xt;
    }

    {
        printf("[%.3f s] Loading database\n", elapsed() - t0);

        size_t base_vector_count, query_dimension;

        std::string sift_base_fvecs = sift_file_path + sift_base_name;
        float *xb = fvecs_read(sift_base_fvecs.c_str(), &query_dimension, &base_vector_count);
        assert(dimension == query_dimension || !"dataset does not have same dimension as train set");

        printf("[%.3f s] Indexing database, size %ld*%ld\n", elapsed() - t0, base_vector_count, dimension);

        index->add(base_vector_count, xb);

        delete[] xb;
    }

    size_t query_count;
    float *query_vector_ptr;

    {
        printf("[%.3f s] Loading queries\n", elapsed() - t0);

        size_t query_dimension;
        std::string sift_query_fvecs = sift_file_path + sift_query_name;
        query_vector_ptr = fvecs_read(sift_query_fvecs.c_str(), &query_dimension, &query_count);
        assert(dimension == query_dimension || !"query does not have same dimension as train set");
    }

    size_t top_k;         // base_vector_count of results per query in the GT
    faiss::idx_t *idx_gt; // query_count * top_k matrix of ground-truth nearest-neighbors

    {
        printf("[%.3f s] Loading ground truth for %ld queries\n", elapsed() - t0, query_count);

        // load ground-truth and convert int to long
        size_t query_count_gt;
        std::string sift_groundtruth_fvecs = sift_file_path + sift_ground_truth_name;
        int *gt_int = ivecs_read(sift_groundtruth_fvecs.c_str(), &top_k, &query_count_gt);
        assert(query_count_gt == query_count || !"incorrect base_vector_count of ground truth entries");

        idx_gt = new faiss::idx_t[top_k * query_count];
        for (int i = 0; i < top_k * query_count; i++) {
            idx_gt[i] = gt_int[i];
        }
        delete[] gt_int;
    }

    if (to_query <= query_count) {
        query_count = to_query;
    }

    // Result of the auto-tuning
    std::string selected_params;

    { // run auto-tuning

        printf("[%.3f s] Preparing auto-tune criterion 1-recall at 1 "
               "criterion, with top_k=%ld query_count=%ld\n",
               elapsed() - t0,
               top_k,
               query_count);

        faiss::OneRecallAtRCriterion crit(query_count, 1);
        crit.set_groundtruth(top_k, nullptr, idx_gt);
        crit.nnn = top_k; // by default, the criterion will request only 1 NN

        printf("[%.3f s] Preparing auto-tune parameters\n", elapsed() - t0);

        faiss::ParameterSpace params;
        params.initialize(index);

        printf("[%.3f s] Auto-tuning over %ld parameters (%ld combinations)\n",
               elapsed() - t0,
               params.parameter_ranges.size(),
               params.n_combinations());

        faiss::OperatingPoints ops;
        params.explore(index, query_count, query_vector_ptr, crit, &ops);

        printf("[%.3f s] Found the following operating points: \n", elapsed() - t0);

        ops.display();

        // keep the first parameter that obtains > 0.5 1-recall@1
        for (int i = 0; i < ops.optimal_pts.size(); i++) {
            if (ops.optimal_pts[i].perf > 0.5) {
                selected_params = ops.optimal_pts[i].key;
                break;
            }
        }
        assert(selected_params.size() >= 0 || !"could not find good enough op point");
    }

    { // Use the found configuration to perform a search

        faiss::ParameterSpace params;

        printf("[%.3f s] Setting parameter configuration \"%s\" on index\n", elapsed() - t0, selected_params.c_str());

        params.set_index_parameters(index, selected_params.c_str());

        // output buffers
        faiss::idx_t *result_idx = new faiss::idx_t[query_count * top_k];
        float *result_distance = new float[query_count * top_k];

        double before_search_ts = elapsed() - t0;
        index->search(query_count, query_vector_ptr, top_k, result_distance, result_idx);
        double after_search_ts = elapsed() - t0;

        printf("[%.3f s] Perform a search on %ld queries\n", before_search_ts, query_count);
        printf("[%.3f s] Compute recalls, cost: %0.3f\n", after_search_ts, after_search_ts - before_search_ts);

        // evaluate result by hand.
        int yes_1 = 0, yes_10 = 0, yes_100 = 0;
        int no_1 = 0, no_10 = 0, no_100 = 0;
        for (int i = 0; i < query_count; i++) {
            int64_t *gt_nn = &(idx_gt[i * top_k]);
            std::unordered_set<int64_t> hash_set1;
            std::unordered_set<int64_t> hash_set10;
            std::unordered_set<int64_t> hash_set100;
            hash_set1.reserve(top_k);
            hash_set10.reserve(top_k);
            hash_set100.reserve(top_k);
            for (int j = 0; j < top_k; ++j) {
                if (j < 1) {
                    hash_set1.insert(gt_nn[j]);
                }
                if (j < 10) {
                    hash_set10.insert(gt_nn[j]);
                }
                if (j < 100) {
                    hash_set100.insert(gt_nn[j]);
                }
            }

            for (int j = 0; j < top_k; j++) {
                if (j < 1) {
                    if (hash_set1.contains(result_idx[i * top_k + j])) {
                        ++yes_1;
                    } else {
                        ++no_1;
                    }
                }

                if (j < 10) {
                    if (hash_set10.contains(result_idx[i * top_k + j])) {
                        ++yes_10;
                    } else {
                        ++no_10;
                    }
                }

                if (j < 100) {
                    if (hash_set100.contains(result_idx[i * top_k + j])) {
                        ++yes_100;
                    } else {
                        ++no_100;
                    }
                }
            }
        }
        printf("R@1 = %.4f(%d: %d)\n", yes_1 / float(query_count * 1), yes_1, no_1);
        printf("R@10 = %.4f(%d: %d)\n", yes_10 / float(query_count * 10), yes_10, no_10);
        printf("R@100 = %.4f(%d: %d)\n", yes_100 / float(query_count * 100), yes_100, no_100);

        delete[] result_idx;
        delete[] result_distance;
    }

    delete[] query_vector_ptr;
    delete[] idx_gt;
    delete index;
}

void hnsw_bench(const char *test_data_path, int64_t to_query, size_t M, size_t ef_construction) {
    if (test_data_path == nullptr) {
        printf("invalid test data path\n");
        return;
    }

    if (!test_file_existence(test_data_path)) {
        return;
    }

    std::string sift_file_path = std::string(test_data_path) + "/sift1M/";

    std::string sift_learn_fvecs = sift_file_path + sift_learn_name;
    std::string sift_base_fvecs = sift_file_path + sift_base_name;
    std::string sift_query_fvecs = sift_file_path + sift_query_name;
    std::string sift_groundtruth_fvecs = sift_file_path + sift_ground_truth_name;

    double t0 = elapsed();

    size_t dimension;
    size_t embedding_count;
    float *input_embeddings = fvecs_read(sift_base_fvecs.c_str(), &dimension, &embedding_count);
    printf("[%.3f s] Load base data\n", elapsed() - t0);

    assert(dimension == 128 || !"embedding dimension isn't 128");
    assert(embedding_count == 10000000 || !"embedding size isn't 1000000");
    hnswlib::L2Space l2space(dimension);
    hnswlib::HierarchicalNSW<float> *hnsw_index = nullptr;

    size_t max_elements = 10000000; // create index

    hnsw_index = new hnswlib::HierarchicalNSW<float>(&l2space, max_elements, M, ef_construction);

    // insert data into index
    for (size_t idx = 0; idx < embedding_count; ++idx) {
        hnsw_index->addPoint(input_embeddings + idx * dimension, idx);

        if (idx % 100000 == 0) {
            printf("[%.3f s] Build Index: %zu, Mem: %ld MB\n", elapsed() - t0, idx, get_current_rss() / 1000000);
        }
    }

    printf("[%.3f s] Index build finish memory cost: %ld MB\n", elapsed() - t0, get_current_rss() / 1000000);

    size_t number_of_queries;
    float *queries;
    {
        size_t dim;
        queries = fvecs_read(sift_query_fvecs.c_str(), &dim, &number_of_queries);
        assert(dimension == dim || !"query does not have same dimension as train set");

        printf("[%.3f s] Load query data\n", elapsed() - t0);
    }

    size_t top_k;         // nb of results per query in the GT
    size_t *ground_truth; // number_of_queries * top_k matrix of ground-truth nearest-neighbors
    {
        // load ground-truth and convert int to long
        size_t nq2;

        int *gt_int = ivecs_read(sift_groundtruth_fvecs.c_str(), &top_k, &nq2);
        assert(nq2 == number_of_queries || !"incorrect nb of ground truth entries");

        ground_truth = new size_t[top_k * number_of_queries];
        for (int i = 0; i < top_k * number_of_queries; ++i) {
            ground_truth[i] = gt_int[i];
        }
        delete[] gt_int;
        printf("[%.3f s] Load ground truth data\n", elapsed() - t0);
    }

    {
        // Query and get result;
        size_t n_valid = 0;
        hnsw_index->setEf(ef_construction);
        for (int i = 0; i < number_of_queries; i++) {
            std::priority_queue<std::pair<float, hnswlib::labeltype>> result = hnsw_index->searchKnn(queries + i * dimension, top_k);
            assert(top_k == result.size() || !"incorrect topk value");

            std::unordered_set<size_t> ground_truth_set;
            for (int j = 0; j < top_k; ++j) {
                ground_truth_set.insert(ground_truth[i * top_k + j]);
            }

            for (int j = 0; j < top_k; ++j) {
                if (ground_truth_set.contains(result.top().second)) {
                    ++n_valid;
                }
                result.pop();
            }
        }
        printf("[%.3f s] Recall = %.4f(%zu/%zu), Top K: %zu\n",
               elapsed() - t0,
               float(n_valid) / float(top_k * number_of_queries),
               n_valid,
               top_k * number_of_queries,
               top_k);
    }

    {
        delete[] input_embeddings;
        delete[] queries;
        delete[] ground_truth;
        delete hnsw_index;
    }
}

TEST_F(KnnBenchTest, faiss_bench) {
    // const char *index_key = "Flat";
    // faiss_bench("IVF4096,Flat", infinity::test_data_path());
    faiss_bench("Flat", infinity::test_data_path(), 1);

    hnsw_bench(infinity::test_data_path(), 1, 16, 200);
}
