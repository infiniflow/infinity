
// #define bucketcontent

#include "unit_test/base_test.h"

#include "faiss/Index.h"
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <omp.h>

import parser;
import infinity_exception;
import stl;
import global_resource_usage;
import third_party;
import logger;
import parser;
import knn_flat_ip;
import infinity;
import index_data;
import ann_ivf_flat;
import search_top_k;
import knn_result_handler;
import vector_distance;

using namespace infinity;

static const char *sift1m_train = "/home/yzq/sift1M/sift_learn.fvecs";
static const char *sift1m_base = "/home/yzq/sift1M/sift_base.fvecs";
static const char *sift1m_query = "/home/yzq/sift1M/sift_query.fvecs";
static const char *sift1m_ground_truth = "/home/yzq/sift1M/sift_groundtruth.ivecs";

// size_t testnum = 100'000;

int global_nb = 100'000;
int n_lists;
int n_probes = 1;
size_t k;

faiss::idx_t *I1;
RowID *I2;
float *D1, *D2;

float *c1, *c2;

float *fvecs_read(const char *fname, size_t *d_out, size_t *n_out);
/*
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
*/

int *ivecs_read(const char *fname, size_t *d_out, size_t *n_out);
/*
// not very clean, but works as long as sizeof(int) == sizeof(float)
int *ivecs_read(const char *fname, size_t *d_out, size_t *n_out) { return (int *)fvecs_read(fname, d_out, n_out); }
*/

double elapsed();
/*
double elapsed() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}
*/

void benchmark_faiss_ivfflatl2() {
    // TODO: limit omp to 1.
    omp_set_num_threads(1);
    // output max omp threads
    std::cout << "max omp threads: " << omp_get_max_threads() << std::endl;
    double t0 = elapsed();

    faiss::IndexFlatL2 *quantizer;
    // faiss::Index *index;
    faiss::IndexIVFFlat *index;

    size_t d;

    {
        printf("[%.3f s] Loading train set\n", elapsed() - t0);

        size_t nt;
        float *xt = fvecs_read(sift1m_train, &d, &nt);

        // printf("[%.3f s] Preparing index \"%s\" d=%ld\n", elapsed() - t0, index_key, d);
        quantizer = new faiss::IndexFlatL2(d);
        n_lists = (size_t)sqrt(nt);
        index = new faiss::IndexIVFFlat(quantizer, d, n_lists, faiss::METRIC_L2);

        printf("[%.3f s] Training on %ld vectors\n, with %d centroids", elapsed() - t0, nt, n_lists);

        index->train(nt, xt);
        delete[] xt;
        {
            // copy content of quantizer->codes to c1, use memcpy
            memcpy(c1, quantizer->codes.data(), n_lists * d * sizeof(float));
        }
    }

    {
        printf("[%.3f s] Loading database\n", elapsed() - t0);

        size_t nb, d2;
        float *xb = fvecs_read(sift1m_base, &d2, &nb);

        //// TODO: change nb to do tests
        // nb = global_nb;
        assert(d == d2 || !"dataset does not have same dimension as train set");

        printf("[%.3f s] Indexing database, size %ld*%ld\n", elapsed() - t0, nb, d);

        index->add(nb, xb);

        delete[] xb;
    }

#ifdef bucketcontent
    {
        // output 100th bucket content
        std::cout << "######################################################" << std::endl;
        std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                  << "the 100th bucket content of IndexIVFFlat:\n";
        auto v_vectors = (const float *)(((faiss::ArrayInvertedLists *)index->invlists)->codes[100].data());
        auto v_ids = ((faiss::ArrayInvertedLists *)index->invlists)->ids[100];
        for (i32 i = 0; i < v_ids.size(); ++i) {
            auto vectors = v_vectors + i * d;
            auto ids = v_ids.data() + i;
            std::cout << "id: " << *ids << std::endl;
            std::cout << "######################################################" << std::endl;
            std::cout << std::endl;
            std::cout << "vector: " << i << "\n";
            for (i32 i = 0; i < d; ++i) {
                std::cout << vectors[i] << " ";
            }
            std::cout << std::endl;
        }
    }
#endif

    size_t nq;
    float *xq;

    {
        printf("[%.3f s] Loading queries\n", elapsed() - t0);

        size_t d2;
        xq = fvecs_read(sift1m_query, &d2, &nq);
        // TODO
        // nq = 100;
        assert(d == d2 || !"query does not have same dimension as train set");
    }

    // size_t k;         // nb of results per query in the GT
    faiss::idx_t *gt; // nq * k matrix of ground-truth nearest-neighbors

    {
        printf("[%.3f s] Loading ground truth for %ld queries\n", elapsed() - t0, nq);

        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(sift1m_ground_truth, &k, &nq2);
        // TODO
        // nq2 = 100;
        // assert(nq2 == nq || !"incorrect nb of ground truth entries");

        gt = new faiss::idx_t[k * nq];
        for (int i = 0; i < k * nq; i++) {
            gt[i] = gt_int[i];
        }
        delete[] gt_int;
        if (false) {
            // output the 1472nd line of gt
            std::cout << "############################" << std::endl;
            std::cout << "gt[1472]:\n";
            for (int j = 0; j < k; j++) {
                std::cout << gt[1472 * k + j] << " ";
            }
            std::cout << std::endl;
        }
    }

    { // Perform a search

        printf("[%.3f s] Perform a search on %ld queries\n", elapsed() - t0, nq);
        {
            // TODO: ksearch = 1?
            int ksearch = k;
            // output buffers
            I1 = new faiss::idx_t[nq * ksearch];
            D1 = new float[nq * ksearch];
            // memset I1 and D1 to 0
            memset(I1, 0, nq * ksearch * sizeof(faiss::idx_t));
            memset(D1, 0, nq * ksearch * sizeof(float));

            index->search(nq, xq, ksearch, D1, I1);

            if (false) {
                // output the 1472nd line of I1
                std::cout << "############################" << std::endl;
                std::cout << "I1[1472]:\n";
                for (int j = 0; j < ksearch; j++) {
                    std::cout << I1[1472 * ksearch + j] << " ";
                }
                std::cout << std::endl;
            }

            if (false) { // output first 3 lines of I1 and D1
                std::cout << "############################" << std::endl;
                for (int i = 0; i < 3; i++) {
                    std::cout << "line " << i << ":\nI:\t";
                    for (int j = 0; j < k; j++) {
                        std::cout << I1[i * k + j] << " ";
                    }
                    std::cout << "\nD:\t";
                    for (int j = 0; j < k; j++) {
                        std::cout << D1[i * k + j] << " ";
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << "############################" << std::endl;
            printf("[%.3f s] Compute recalls\n", elapsed() - t0);
            std::cout << "############################" << std::endl;

            int n_1 = 0, n_10 = 0, n_100 = 0;
            // std::unordered_set<int32_t> gt1, gt10, gt100;
            for (int i = 0; i < nq; i++) {
                std::unordered_set<int32_t> gt1, gt10, gt100;
                for (int j = 0; j < k; ++j) {
                    int32_t gt_id = gt[i * k + j];
                    if (j < 1) {
                        gt1.insert(gt_id);
                    }
                    if (j < 10) {
                        gt10.insert(gt_id);
                    }
                    if (j < 100) {
                        gt100.insert(gt_id);
                    }
                }
                for (int j = 0; j < k; j++) {
                    int32_t result_id = I1[i * k + j];
                    if (j < 1 && gt1.contains(result_id)) {
                        ++n_1;
                    }
                    if (j < 10 && gt10.contains(result_id)) {
                        ++n_10;
                    }
                    if (j < 100 && gt100.contains(result_id)) {
                        ++n_100;
                    }
                }
            }
            // output n_1, n_10, n_100
            std::cout << "############################" << std::endl;
            std::cout << "n_1: " << n_1 << std::endl;
            std::cout << "n_10: " << n_10 << std::endl;
            std::cout << "n_100: " << n_100 << std::endl;
            std::cout << "############################" << std::endl;
            printf("R@1 = %.4f\n", n_1 / float(nq));
            printf("R@10 = %.4f\n", n_10 / float(nq * 10));
            printf("R@100 = %.4f\n", n_100 / float(nq * 100));
        }
    }

    delete[] xq;
    delete[] gt;
    delete index;
}

void benchmark_annivfflatl2() {
    double t0 = elapsed();

    size_t d;

    size_t partition_num;

    SharedPtr<IVFFlatIndexData<float>> ann_index_data;

    {
        size_t nt;

        printf("[%.3f s] Loading train set\n", elapsed() - t0);

        float *xt = fvecs_read(sift1m_train, &d, &nt);

        // printf("[%.3f s] Preparing index \"%s\" d=%ld\n", elapsed() - t0, index_key, d);
        // index = faiss::index_factory(d, index_key);

        // TODO: change nt to do tests
        // nt = testnum;

        partition_num = (size_t)sqrt(nt);

        printf("[%.3f s] Loading database\n", elapsed() - t0);

        size_t nb, d2;
        float *xb = fvecs_read(sift1m_base, &d2, &nb);
        // TODO:nb
        // nb = global_nb;
        assert(d == d2 || !"dataset does not have same dimension as train set");

        printf("[%.3f s] Training and Indexing on %ld vectors\n, with %ld centroids\n", elapsed() - t0, nt, partition_num);

        // ann_index_data = AnnIVFFlatL2<float>::CreateIndex(d, nt, xt, nb, xb, partition_num, 0);
        ann_index_data = AnnIVFFlatL2<float>::CreateIndex(d, nt, xt, nb, xb, partition_num, 0);
        // TODO:remove this
        {
            int c1 = 88, c2 = 286;
            auto v_ids_88 = ann_index_data->ids_[c1];
            auto v_ids_286 = ann_index_data->ids_[c2];
            // output i, selected_centroid, with description
            std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                      << c1 << " contain 567736: "
                      << (std::find_if(v_ids_88.begin(), v_ids_88.end(), [](auto &id) { return id.segment_offset_ == 567736; }) != v_ids_88.end())
                      << std::endl;
            std::cout << "\n"
                      << c2 << " contain 567736: "
                      << (std::find_if(v_ids_286.begin(), v_ids_286.end(), [](auto &id) { return id.segment_offset_ == 567736; }) != v_ids_286.end())
                      << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                      << std::endl;
        }
        // copy content of ann_index_data->centroids_ to c2, use memcpy
        memcpy(c2, ann_index_data->centroids_.data(), partition_num * d * sizeof(float));

        // compare c1 and c2,output difference
        if (false) {
            std::cout << "############################" << std::endl;
            {
                // first 10 lines of c1 and c2
                std::cout << "c1:\n";
                for (i32 i = 0; i < 10; ++i) {
                    std::cout << "c1 row " << i << ": ";
                    for (i32 j = 0; j < d; ++j) {
                        std::cout << c1[i * d + j] << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << "c2:\n";
                for (i32 i = 0; i < 10; ++i) {
                    std::cout << "c2 row " << i << ": ";
                    for (i32 j = 0; j < d; ++j) {
                        std::cout << c2[i * d + j] << " ";
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << "c1 and c2 difference:\n";
            for (i32 i = 0; i < 316 * 128; ++i) {
                if (c1[i] != c2[i]) {
                    std::cout << "c1[" << i << "]: " << c1[i] << " c2[" << i << "]: " << c2[i] << "difference: " << c1[i] - c2[i] << std::endl;
                }
            }
            std::cout << "############################" << std::endl;
        }

#ifdef bucketcontent
        // output the 100th vector content of ann_index_data->vectors_ and ann_index_data->ids_
        {
            std::cout << "######################################################" << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "the 100th bucket content of ann_index_data:\n";
            auto v_vectors = ann_index_data->vectors_[100];
            auto v_ids = ann_index_data->ids_[100];
            for (i32 i = 0; i < v_ids.size(); ++i) {
                auto vectors = v_vectors.data() + i * d;
                auto ids = v_ids.data() + i;
                std::cout << "id: " << ids->block_offset_ << std::endl;
                std::cout << "######################################################" << std::endl;
                std::cout << std::endl;
                std::cout << "vector: " << i << "\n";
                for (i32 i = 0; i < d; ++i) {
                    std::cout << vectors[i] << " ";
                }
                std::cout << std::endl;
            }
        }
#endif

        delete[] xt;
        delete[] xb;
    }

    size_t nq;
    float *xq;

    {
        printf("[%.3f s] Loading queries\n", elapsed() - t0);

        size_t d2;
        xq = fvecs_read(sift1m_query, &d2, &nq);
        // TODO
        // nq = 100;
        assert(d == d2 || !"query does not have same dimension as train set");
    }

    // int n_probes;
    // size_t k;         // nb of results per query in the GT
    faiss::idx_t *gt; // nq * k matrix of ground-truth nearest-neighbors

    {
        printf("[%.3f s] Loading ground truth for %ld queries\n", elapsed() - t0, nq);

        // load ground-truth and convert int to long
        size_t nq2;
        int *gt_int = ivecs_read(sift1m_ground_truth, &k, &nq2);
        // TODO
        // nq2 = 100;
        assert(nq2 == nq || !"incorrect nb of ground truth entries");

        gt = new faiss::idx_t[k * nq];
        for (int i = 0; i < k * nq; i++) {
            gt[i] = gt_int[i];
        }
        delete[] gt_int;
    }

    { // Perform a search

        printf("[%.3f s] Perform a search on %ld queries\n", elapsed() - t0, nq);

        AnnIVFFlatL2<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
        test_ivf.Begin();
        test_ivf.Search(ann_index_data.get(), n_probes);
        test_ivf.End();

        I2 = test_ivf.GetIDs();
        D2 = test_ivf.GetDistances();

        if (false) { // output first 3 lines of I and D
            std::cout << "############################" << std::endl;
            for (int i = 0; i < 3; i++) {
                std::cout << "line " << i << ":\nI:\t";
                for (int j = 0; j < k; j++) {
                    std::cout << I2[i * k + j].segment_offset_ << " ";
                }
                std::cout << "\nD:\t";
                for (int j = 0; j < k; j++) {
                    std::cout << D2[i * k + j] << " ";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "############################" << std::endl;

        if (true) {
            // compare I1,I2, D1,D2
            int dif = 0, difI = 0;
            std::cout << "############################" << std::endl;
            std::cout << "D1 and D2 difference:\n";
            for (i32 i = 0; i < nq * k; ++i) {
                if (D1[i] != D2[i]) {
                    if (++dif > 10)
                        break;
                    std::cout << "D1[" << (i / k) << " : " << (i % k) << "]: " << D1[i] << " D2[" << (i / k) << " : " << (i % k) << "]: " << D2[i]
                              << "\tdifference: " << D1[i] - D2[i] << std::endl;
                }
            }
            std::cout << "############################" << std::endl;
            std::cout << "I1 and I2 difference:\n";
            for (i32 i = 0; i < nq * k; ++i) {
                if (I1[i] != I2[i].segment_offset_) {
                    if (++difI > 10)
                        break;
                    std::cout << "I1[" << (i / k) << " : " << (i % k) << "]: " << I1[i] << " I2[" << (i / k) << " : " << (i % k)
                              << "]: " << I2[i].segment_offset_ << std::endl;
                }
            }
            std::cout << "############################" << std::endl;
        }
        {
            int vacant_1 = 0, vacant_10 = 0, vacant_100 = 0;
            // count D2 that == numeric_limits<float>::max()
            for (i32 i = 0; i < nq; ++i) {
                for (i32 j = 0; j < k; ++j) {
                    if (D2[i * k + j] == std::numeric_limits<float>::max()) {
                        if (j < 1) {
                            ++vacant_1;
                        }
                        if (j < 10) {
                            ++vacant_10;
                        }
                        if (j < 100) {
                            ++vacant_100;
                        }
                    }
                }
            }
            // output vacant_1, vacant_10, vacant_100
            std::cout << "############################" << std::endl;
            std::cout << "vacant_1: " << vacant_1 << std::endl;
            std::cout << "vacant_10: " << vacant_10 << std::endl;
            std::cout << "vacant_100: " << vacant_100 << std::endl;
            // output nq
            std::cout << "nq: " << nq << std::endl;
            std::cout << "############################" << std::endl;
            printf("vacant@1 = %.4f\n", vacant_1 / float(nq));
            printf("vacant@10 = %.4f\n", vacant_10 / float(nq * 10));
            printf("vacant@100 = %.4f\n", vacant_100 / float(nq * 100));
        }

        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", elapsed() - t0);
        std::cout << "############################" << std::endl;

        int n_1 = 0, n_10 = 0, n_100 = 0;
        // std::unordered_set<int32_t> gt1, gt10, gt100;
        for (int i = 0; i < nq; i++) {
            std::unordered_set<int32_t> gt1, gt10, gt100;
            for (int j = 0; j < k; ++j) {
                int32_t gt_id = gt[i * k + j];
                if (j < 1) {
                    gt1.insert(gt_id);
                }
                if (j < 10) {
                    gt10.insert(gt_id);
                }
                if (j < 100) {
                    gt100.insert(gt_id);
                }
            }
            for (int j = 0; j < k; j++) {
                int32_t result_id = I2[i * k + j].segment_offset_;
                if (j < 1 && gt1.contains(result_id)) {
                    ++n_1;
                }
                if (j < 10 && gt10.contains(result_id)) {
                    ++n_10;
                }
                if (j < 100 && gt100.contains(result_id)) {
                    ++n_100;
                }
            }
        }
        // output n_1, n_10, n_100
        std::cout << "############################" << std::endl;
        std::cout << "n_1: " << n_1 << std::endl;
        std::cout << "n_10: " << n_10 << std::endl;
        std::cout << "n_100: " << n_100 << std::endl;
        std::cout << "############################" << std::endl;
        printf("R@1 = %.4f\n", n_1 / float(nq));
        printf("R@10 = %.4f\n", n_10 / float(nq * 10));
        printf("R@100 = %.4f\n", n_100 / float(nq * 100));
    }
    delete[] xq;
    delete[] gt;
    // delete index;
}

class AnnIVFFlatL2Benchmark : public BaseTest {};

TEST_F(AnnIVFFlatL2Benchmark, test1) {
    c1 = new float[316 * 128];
    c2 = new float[316 * 128];
    omp_set_num_threads(1);
    // output max omp threads
    std::cout << "max omp threads: " << omp_get_max_threads() << std::endl;
    benchmark_faiss_ivfflatl2();
    std::cout << "Hello ?" << std::endl;
    benchmark_annivfflatl2();
    delete[] I1;
    delete[] D1;
    delete[] c1;
    delete[] c2;
}
