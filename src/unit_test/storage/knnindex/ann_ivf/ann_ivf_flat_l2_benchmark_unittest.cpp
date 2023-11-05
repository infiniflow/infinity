
#define centroids
// #define bucketcontent

#include "unit_test/base_test.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/time.h>

#include "faiss/AutoTune.h"
#include "faiss/Index.h"
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVFFlat.h"
#include "faiss/MetricType.h"
#include "faiss/index_factory.h"
#include <omp.h>

import parser;
import infinity_exception;
import infinity_assert;
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
// static const char *sift1m_train = "/home/yzq/sift1M/sift_base.fvecs";
static const char *sift1m_base = "/home/yzq/sift1M/sift_base.fvecs";
static const char *sift1m_query = "/home/yzq/sift1M/sift_query.fvecs";
static const char *sift1m_ground_truth = "/home/yzq/sift1M/sift_groundtruth.ivecs";

// size_t testnum = 100'000;

int global_nb = 100'000;
int n_lists;
int n_probes = 1;
size_t k;
float *D = nullptr;

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

    // this is typically the fastest one.
    // const char *index_key = "IVF4096,Flat";

    // these ones have better memory usage
    // const char *index_key = "Flat";
    // const char *index_key = "PQ32";
    // const char *index_key = "PCA80,Flat";
    // const char *index_key = "IVF4096,PQ8+16";
    // const char *index_key = "IVF4096,PQ32";
    // const char *index_key = "IMI2x8,PQ32";
    // const char *index_key = "IMI2x8,PQ8+16";
    // const char *index_key = "OPQ16_64,IMI2x8,PQ8+16";

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
    }

    { // Perform a search

        printf("[%.3f s] Perform a search on %ld queries\n", elapsed() - t0, nq);

        auto *I = new u64[nq * k];
        D = new float[nq * k];

        if (true) {
            // TODO: ksearch = 1?
            int ksearch = k;
            // output buffers
            faiss::idx_t *I = new faiss::idx_t[nq * ksearch];
            D = new float[nq * ksearch];

            index->search(nq, xq, ksearch, D, I);

            std::cout << "############################" << std::endl;
            printf("[%.3f s] Compute recalls\n", elapsed() - t0);
            std::cout << "############################" << std::endl;

            int n_1 = 0, n_10 = 0, n_100 = 0;
            std::unordered_set<int32_t> gt1, gt10, gt100;
            for (int i = 0; i < nq; i++) {
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
                    int32_t result_id = I[i * k + j];
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
            printf("R@1 = %.4f\n", n_1 / float(nq));
            printf("R@10 = %.4f\n", n_10 / float(nq * 10));
            printf("R@100 = %.4f\n", n_100 / float(nq * 100));
        } else {
            // TODO:test search?
            // index->search(nq, xq, k, D, I);

            using HeapResultHandler = NewHeapResultHandler<faiss::CMax<float, u64>>;
            using HeapSingleHandler = HeapResultHandler::HeapSingleResultHandler;
            using HeapResultHandler_INT = NewHeapResultHandler<faiss::CMax<float, int>>;
            using HeapSingleHandler_INT = HeapResultHandler_INT::HeapSingleResultHandler;

            auto id_array_ = MakeUnique<Vector<idx_t>>(k * nq);
            auto distance_array_ = MakeUnique<float[]>(k * nq);

            auto heap_result_handler_ = MakeUnique<HeapResultHandler>(nq, distance_array_.get(), id_array_->data(), k);
            auto single_heap_result_handler_ = MakeUnique<HeapSingleHandler>(*heap_result_handler_, nq);

            for (SizeT i = 0; i < nq; ++i) {
                single_heap_result_handler_.get()->begin(i);
            }

            for (i64 i = 0; i < nq; i++) {
                const float *x_i = xq + i * d;
                Vector<float> centroid_dists(n_probes);
                Vector<i32> centroid_ids(n_probes);
                HeapResultHandler_INT centroid_heap_result(1, centroid_dists.data(), centroid_ids.data(), n_probes);
                HeapSingleHandler_INT centroid_single_heap_result(centroid_heap_result, 1);
                centroid_single_heap_result.begin(0);
                for (i32 j = 0; j < n_lists; j++) {
                    const float *y_j = (const float *)(quantizer->codes.data()) + j * d;
                    float ip = L2Distance<float>(x_i, y_j, d);
                    centroid_single_heap_result.add_result(ip, j, 0);
                }
                centroid_single_heap_result.end(0);
                for (i32 kk = 0; kk < n_probes; kk++) {
                    const i32 selected_centroid = centroid_ids[kk];
                    const i32 contain_nums = ((faiss::ArrayInvertedLists *)index->invlists)->ids[selected_centroid].size();
                    const float *y_j = (const float *)(((faiss::ArrayInvertedLists *)index->invlists)->codes[selected_centroid].data());
                    for (i32 j = 0; j < contain_nums; j++, y_j += d) {
                        float ip = L2Distance<float>(x_i, y_j, d);
                        single_heap_result_handler_->add_result(ip, ((faiss::ArrayInvertedLists *)index->invlists)->ids[selected_centroid][j], i);
                    }
                }
            }

            for (i32 i = 0; i < nq; ++i) {
                single_heap_result_handler_->end(i);
            }
            I = heap_result_handler_->heap_ids_tab;

            // output first 3 lines of I
            std::cout << "############################" << std::endl;
            for (int i = 0; i < 3; i++) {
                std::cout << "line " << i << ":\t";
                for (int j = 0; j < k; j++) {
                    std::cout << I[i * k + j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "############################" << std::endl;

            int n_1 = 0, n_10 = 0, n_100 = 0;
            std::unordered_set<int32_t> gt1, gt10, gt100;
            for (int i = 0; i < nq; i++) {
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
                    int32_t result_id = I[i * k + j];
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
            printf("R@1 = %.4f\n", n_1 / float(nq));
            printf("R@10 = %.4f\n", n_10 / float(nq * 10));
            printf("R@100 = %.4f\n", n_100 / float(nq * 100));
        }
        /*
         // output D
         std::cout << "############################DDDDDDDDDDDD" << std::endl;
         for (int i = 0; i < nq; i++) {
             std::cout << D[i] << " ";
             if (i % 100 == 0)
                 std::cout << std::endl;
         }
         std::cout << "############################DDDDDDDDDDDD" << std::endl;
            */

        if constexpr (false) {
            int ksearch = 1;
            // evaluate result by hand.
            int n_1 = 0, n_10 = 0, n_100 = 0;
            for (int i = 0; i < nq; i++) {
                int gt_nn = gt[i * k];
                for (int j = 0; j < ksearch; j++) {
                    if (I[i * ksearch + j] == gt_nn) {
                        if (j < 1)
                            n_1++;
                        if (j < 10)
                            n_10++;
                        if (j < 100)
                            n_100++;
                    }
                }
            }
            printf("R@1 = %.4f\n", n_1 / float(nq));
            printf("R@10 = %.4f\n", n_10 / float(nq));
            printf("R@100 = %.4f\n", n_100 / float(nq));
        } else {
        }

        // delete[] I;
        // delete[] D;
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

        ann_index_data = AnnIVFFlatL2<float>::CreateIndex(d, nt, xt, nb, xb, partition_num, 0);

#ifdef centroids
        // output centroids
        {
            auto centroids_ = ann_index_data->centroids_;
            // output content of centroids, 128 per line
            std::cout << "######################################################" << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(3) << elapsed() - t0 << " s] "
                      << "first 10 centroids:\n";
            for (i32 i = 0; i < 10; ++i) {
                std::cout << "partition " << i << ": ";
                for (i32 j = 0; j < d; ++j) {
                    std::cout << centroids_[i * d + j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "######################################################" << std::endl;
            std::cout << std::endl;
        }
#endif

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

        auto I = test_ivf.GetIDs();

        // output first 3 lines of I
        std::cout << "############################" << std::endl;
        for (int i = 0; i < 3; i++) {
            std::cout << "line " << i << ":\t";
            for (int j = 0; j < k; j++) {
                std::cout << I[i * k + j].segment_offset_ << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", elapsed() - t0);
        std::cout << "############################" << std::endl;

        int n_1 = 0, n_10 = 0, n_100 = 0;
        std::unordered_set<int32_t> gt1, gt10, gt100;
        for (int i = 0; i < nq; i++) {
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
                int32_t result_id = I[i * k + j].segment_offset_;
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
        printf("R@1 = %.4f\n", n_1 / float(nq));
        printf("R@10 = %.4f\n", n_10 / float(nq * 10));
        printf("R@100 = %.4f\n", n_100 / float(nq * 100));

        if constexpr (false) {
            // output buffers
            // faiss::idx_t *I = new faiss::idx_t[nq * k];
            // float *D = new float[nq * k];

            /*
            AnnIVFFlatL2<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
            test_ivf.Begin();
            test_ivf.Search(ann_index_data.get(), n_probes);
            test_ivf.End();

            auto I = test_ivf.GetIDs();

            // TODO: check if this is correct.
            auto D = test_ivf.GetDistances();
            // output k elements of D
            std::cout << "############################" << std::endl;
            for (int i = 0; i < k; i++) {
                std::cout << D[i] << std::endl;
            }
            std::cout << "############################" << std::endl;

            */
            auto I = MakeUnique<int[]>(nq);
            auto D2 = MakeUnique<float[]>(nq);
            FakeSearch11(d, nq, xq, I.get(), ann_index_data.get(), D2.get());

            printf("[%.3f s] Compute recalls\n", elapsed() - t0);

            // output D2
            std::cout << "############################" << std::endl;
            for (int i = 0; i < nq; i++) {
                std::cout << D2[i] << " ";
                if (i % 100 == 0)
                    std::cout << std::endl;
            }
            int n_1 = 0, n_10 = 0, n_100 = 0;
            for (int i = 0; i < nq; i++) {
                int gt_nn = gt[i * k];
                if (I[i] == gt_nn) {
                    n_1++;
                    n_10++;
                    n_100++;
                }
            }
            /*
            // evaluate result by hand.
            int n_1 = 0, n_10 = 0, n_100 = 0;
            for (int i = 0; i < nq; i++) {
                int gt_nn = gt[i * k];
                for (int j = 0; j < k; j++) {
                    if (I[i * k + j].block_offset_ == gt_nn) {
                        if (j < 1)
                            n_1++;
                        if (j < 10)
                            n_10++;
                        if (j < 100)
                            n_100++;
                    }
                }
            }
            */
            printf("R@1 = %.4f\n", n_1 / float(nq));
            printf("R@10 = %.4f\n", n_10 / float(nq));
            printf("R@100 = %.4f\n", n_100 / float(nq));

            // delete[] I;
            // delete[] D;

            // output (D2 - D) / D
            float sum = 0, pos = 0, neg = 0;
            std::cout << "############################" << std::endl;
            for (int i = 0; i < nq; i++) {
                std::cout << (D2[i] - D[i]) / (float)D[i] << " ";
                sum += (D2[i] - D[i]);
                if ((D2[i] - D[i]) > 0)
                    pos += D2[i] - D[i];
                else
                    neg += D2[i] - D[i];
                if (i % 100 == 0)
                    std::cout << std::endl;
            }
            // output sum, pos, neg
            std::cout << "############################" << std::endl;
            std::cout << "sum: " << sum << std::endl;
            std::cout << "pos: " << pos << std::endl;
            std::cout << "neg: " << neg << std::endl;
            std::cout << "############################" << std::endl;
            for (int i = 0; i < nq; i++) {
                std::cout << (D2[i] - D[i]) << " ";
                if (i % 100 == 0)
                    std::cout << std::endl;
            }
            std::cout << "############################" << std::endl;
            // output negative values in D and D2
            for (int i = 0; i < nq; i++) {
                if (D[i] < 0 || D2[i] < 0)
                    std::cout << i << ": " << D[i] << " " << D2[i] << std::endl;
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
    std::cout << "max omp threads: " << omp_get_max_threads() << std::endl;
    benchmark_faiss_ivfflatl2();
    std::cout << "Hello ?" << std::endl;
    benchmark_annivfflatl2();
    delete[] D;
}
