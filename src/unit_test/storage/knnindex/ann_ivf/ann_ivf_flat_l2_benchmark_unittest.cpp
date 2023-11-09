
// #define bucketcontent

#include "unit_test/base_test.h"

#include "faiss/Index.h"
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVF.h"
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
import annivfflat_index_data;
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
int n_probes = 16;
size_t k;

faiss::idx_t *I1;
faiss::idx_t *I11;
RowID *I2;
RowID *I3;
float *D1, *D2, *D3, *D11;

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
    // std::cout << "max omp threads: " << omp_get_max_threads() << std::endl;
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
        quantizer->verbose = true;

        n_lists = (size_t)sqrt(nt);
        index = new faiss::IndexIVFFlat(quantizer, d, n_lists, faiss::METRIC_L2);
        index->verbose = true;

        printf("[%.3f s] Training on %ld vectors\n, with %d centroids", elapsed() - t0, nt, n_lists);

        index->train(nt, xt);
        delete[] xt;
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

            faiss::IVFSearchParameters p;
            p.nprobe = n_probes;
            index->search(nq, xq, ksearch, D1, I1, &p);

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

void benchmark_faiss_ivfflatip() {
    // TODO: limit omp to 1.
    omp_set_num_threads(1);
    // output max omp threads
    // std::cout << "max omp threads: " << omp_get_max_threads() << std::endl;
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
        quantizer->verbose = true;

        n_lists = (size_t)sqrt(nt);
        index = new faiss::IndexIVFFlat(quantizer, d, n_lists, faiss::METRIC_INNER_PRODUCT);
        index->verbose = true;

        printf("[%.3f s] Training on %ld vectors\n, with %d centroids", elapsed() - t0, nt, n_lists);

        index->train(nt, xt);
        delete[] xt;
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
    }

    { // Perform a search

        printf("[%.3f s] Perform a search on %ld queries\n", elapsed() - t0, nq);
        {
            // TODO: ksearch = 1?
            int ksearch = k;
            // output buffers
            I11 = new faiss::idx_t[nq * ksearch];
            D11 = new float[nq * ksearch];
            // memset I1 and D1 to 0
            memset(I11, 0, nq * ksearch * sizeof(faiss::idx_t));
            memset(D11, 0, nq * ksearch * sizeof(float));

            faiss::IVFSearchParameters p;
            p.nprobe = n_probes;
            index->search(nq, xq, ksearch, D11, I11, &p);

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
                    int32_t result_id = I11[i * k + j];
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

    UniquePtr<AnnIVFFlatIndexData<float>> ann_index_data;

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
        ann_index_data = AnnIVFFlatL2<float>::CreateIndex(d, nt, xt, nb, xb, partition_num);
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
        test_ivf.Search(ann_index_data.get(), 0, n_probes);
        test_ivf.End();

        I2 = test_ivf.GetIDs();
        D2 = test_ivf.GetDistances();

        std::cout << "############################" << std::endl;

        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", elapsed() - t0);
        std::cout << "############################" << std::endl;

        int n_1 = 0, n_10 = 0, n_100 = 0;
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

        std::cout << "\n######################################\n" << std::endl;
        if (false) {
            // compare I1,I2, D1,D2
            {
                std::cout << "############################" << std::endl;
                std::cout << "D1 and D2 difference:\n";
                for (int id1 = 0, id2 = 0, diffc1 = 0, q = 0; diffc1 < 500 || id1 > ((nq / 2) * k) || id2 > ((nq / 2) * k);) {
                    if (abs(D1[id1] - D2[id2]) < 0.01f) {
                        ++id1;
                        ++id2;
                    } else {
                        ++diffc1;
                        if (D1[id1] < D2[id2]) {
                            // warn: D1 found result which is not in D2
                            std::cout << "D1 found result which is not in D2: "
                                      << "D1[" << (id1 / k) << " : " << (id1 % k) << "]: " << D1[id1] << " D2[" << (id2 / k) << " : " << (id2 % k)
                                      << "]: " << D2[id2] << " difference: " << D1[id1] - D2[id2] << std::endl;
                            ++id1;
                        } else {
                            // warn: D2 found result which is not in D1
                            std::cout << "D2 found result which is not in D1: "
                                      << "D1[" << (id1 / k) << " : " << (id1 % k) << "]: " << D1[id1] << " D2[" << (id2 / k) << " : " << (id2 % k)
                                      << "]: " << D2[id2] << " difference: " << D1[id1] - D2[id2] << std::endl;
                            ++id2;
                        }
                    }
                    if (((id1 % k) == 0) || ((id2 % k) == 0)) {
                        ++q;
                        id1 = q * k;
                        id2 = q * k;
                    }
                }
                std::cout << "############################" << std::endl;
            }
        }
    }

    delete[] xq;
    delete[] gt;
    // delete index;
}

void benchmark_annivfflatip() {
    double t0 = elapsed();

    size_t d;

    size_t partition_num;

    UniquePtr<AnnIVFFlatIndexData<float>> ann_index_data;

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
        ann_index_data = AnnIVFFlatIP<float>::CreateIndex(d, nt, xt, nb, xb, partition_num);

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

        AnnIVFFlatIP<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
        test_ivf.Begin();
        test_ivf.Search(ann_index_data.get(), 0, n_probes);
        test_ivf.End();

        I3 = test_ivf.GetIDs();
        D3 = test_ivf.GetDistances();

        std::cout << "############################" << std::endl;

        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", elapsed() - t0);
        std::cout << "############################" << std::endl;

        int n_1 = 0, n_10 = 0, n_100 = 0;
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
                int32_t result_id = I3[i * k + j].segment_offset_;
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

        std::cout << "\n######################################\n" << std::endl;
        if (false) {
            // compare I11,I3, D11,D3
            {
                std::cout << "############################" << std::endl;
                std::cout << "D11 and D3 difference:\n";
                for (int id1 = 0, id2 = 0, diffc1 = 0, q = 0; diffc1 < 500 || id1 > ((nq / 2) * k) || id2 > ((nq / 2) * k);) {
                    if (abs(D11[id1] - D3[id2]) < 0.01f) {
                        ++id1;
                        ++id2;
                    } else {
                        ++diffc1;
                        if (D11[id1] > D3[id2]) {
                            // warn: D1 found result which is not in D3
                            std::cout << "D11 found result which is not in D3: "
                                      << "D11[" << (id1 / k) << " : " << (id1 % k) << "]: " << D11[id1] << " D3[" << (id2 / k) << " : " << (id2 % k)
                                      << "]: " << D3[id2] << " difference: " << D11[id1] - D3[id2] << std::endl;
                            ++id1;
                        } else {
                            // warn: D3 found result which is not in D1
                            std::cout << "D3 found result which is not in D1: "
                                      << "D11[" << (id1 / k) << " : " << (id1 % k) << "]: " << D11[id1] << " D3[" << (id2 / k) << " : " << (id2 % k)
                                      << "]: " << D3[id2] << " difference: " << D11[id1] - D3[id2] << std::endl;
                            ++id2;
                        }
                    }
                    if (((id1 % k) == 0) || ((id2 % k) == 0)) {
                        ++q;
                        id1 = q * k;
                        id2 = q * k;
                    }
                }
                std::cout << "############################" << std::endl;
            }
        }
    }

    delete[] xq;
    delete[] gt;
    // delete index;
}

class AnnIVFFlatL2Benchmark : public BaseTest {};

TEST_F(AnnIVFFlatL2Benchmark, test1) {
    omp_set_num_threads(1);
    // output max omp threads
    // std::cout << "max omp threads: " << omp_get_max_threads() << std::endl;
    benchmark_faiss_ivfflatip();
    std::cout << "##########################################################" << std::endl;
    std::cout << "Hello ?" << std::endl;
    std::cout << "##########################################################" << std::endl;
    benchmark_annivfflatip();
    std::cout << "##########################################################" << std::endl;
    std::cout << "Hello ?" << std::endl;
    std::cout << "##########################################################" << std::endl;
    benchmark_faiss_ivfflatl2();
    std::cout << "##########################################################" << std::endl;
    std::cout << "Hello ?" << std::endl;
    std::cout << "##########################################################" << std::endl;
    benchmark_annivfflatl2();
    delete[] I1;
    delete[] I11;
    delete[] D1;
    delete[] D11;
}
