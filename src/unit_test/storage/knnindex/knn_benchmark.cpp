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
    assert(embedding_count == 1000000 || !"embedding size isn't 100000");
    hnswlib::L2Space l2space(dimension);
    hnswlib::HierarchicalNSW<float> *hnsw_index = nullptr;

    size_t max_elements = 1000000; // create index

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

TEST_F(KnnBenchTest, hnsw_bench) {
    hnsw_bench(infinity::test_data_path(), 1, 16, 200);
}
