

/*
 * This benchmark includes:
 * 1. faiss IndexIVFFlatL2 (train, add, search)
 * 2. faiss IndexIVFFlatIP (train, add, search)
 * 3. ann_ivfflatl2 (train, add, search, save to / load from file)
 * 4. ann_ivfflatip (train, add, search, save to / load from file)
 */

#include "base_profiler.h"
#include "faiss/Index.h"
#include "faiss/IndexFlat.h"
#include "faiss/IndexIVF.h"
#include "faiss/IndexIVFFlat.h"
#include "helper.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sys/time.h>

import stl;
import ann_ivf_flat;
import index_def;
import annivfflat_index_data;
import parser;
import local_file_system;

static const char *sift1m_train = "./test/data/fvecs/sift_learn.fvecs";
static const char *sift1m_base = "./test/data/fvecs/sift_base.fvecs";
static const char *sift1m_query = "./test/data/fvecs/sift_query.fvecs";
static const char *sift1m_ground_truth = "./test/data/fvecs/sift_groundtruth.ivecs";

using namespace infinity;

constexpr bool ann_ivfflat_benchmark_verbose = false;

float *fvecs_read(const char *fname, size_t *d_out, size_t *n_out);

int *ivecs_read(const char *fname, size_t *d_out, size_t *n_out);

double ann_benchmark_elapsed() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

template <typename T1, typename T2>
void compute_recall(const T1 *Igt, const T2 I_comp, size_t nq, size_t k) {
    int n_1 = 0, n_10 = 0, n_100 = 0;
    for (int i = 0; i < nq; i++) {
        std::unordered_set<int32_t> gt1, gt10, gt100;
        for (int j = 0; j < k; ++j) {
            auto gt_id = Igt[i * k + j];
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
            auto result_id = I_comp(i * k + j);
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
    std::cout << "############################" << std::endl;
}

void benchmark_faiss_ivfflatl2(double t0,
                               size_t d,
                               size_t nt,
                               const float *xt,
                               size_t nb,
                               const float *xb,
                               size_t nq,
                               const float *xq,
                               int k,
                               const int *gt,
                               size_t n_probes,
                               size_t n_centroids) {
    auto I_faiss_l2 = std::make_unique<int64_t[]>(nq * k);
    auto D_faiss_l2 = std::make_unique<float[]>(nq * k);
    faiss::IndexFlatL2 quantizer(d);
    quantizer.verbose = true;
    faiss::IndexIVFFlat index(&quantizer, d, n_centroids, faiss::METRIC_L2);
    index.verbose = true;
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Training on %ld vectors, with %d centroids\n", ann_benchmark_elapsed() - t0, nt, n_centroids);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        index.train(nt, xt);
        profiler.End();
        std::cout << "training data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Indexing database, size %ld*%ld\n", ann_benchmark_elapsed() - t0, nb, d);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        index.add(nb, xb);
        profiler.End();
        std::cout << "adding data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Perform a search on %ld queries\n", ann_benchmark_elapsed() - t0, nq);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        faiss::IVFSearchParameters p;
        p.nprobe = n_probes;
        index.search(nq, xq, k, D_faiss_l2.get(), I_faiss_l2.get(), &p);
        profiler.End();
        std::cout << "searching data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }
    {
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", ann_benchmark_elapsed() - t0);
        std::cout << "############################" << std::endl;
        auto I = [&I_faiss_l2](size_t i) { return I_faiss_l2[i]; };
        compute_recall(gt, I, nq, k);
    }
}

void benchmark_faiss_ivfflatip(double t0,
                               size_t d,
                               size_t nt,
                               const float *xt,
                               size_t nb,
                               const float *xb,
                               size_t nq,
                               const float *xq,
                               int k,
                               const int *gt,
                               size_t n_probes,
                               size_t n_centroids) {
    auto I_faiss_ip = std::make_unique<int64_t[]>(nq * k);
    auto D_faiss_ip = std::make_unique<float[]>(nq * k);

    faiss::IndexFlatL2 quantizer(d);
    quantizer.verbose = true;
    faiss::IndexIVFFlat index(&quantizer, d, n_centroids, faiss::METRIC_INNER_PRODUCT);
    index.verbose = true;
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Training on %ld vectors , with %d centroids\n", ann_benchmark_elapsed() - t0, nt, n_centroids);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        index.train(nt, xt);
        profiler.End();
        std::cout << "training data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Indexing database, size %ld*%ld\n", ann_benchmark_elapsed() - t0, nb, d);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        index.add(nb, xb);
        profiler.End();
        std::cout << "adding data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Perform a search on %ld queries\n", ann_benchmark_elapsed() - t0, nq);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        faiss::IVFSearchParameters p;
        p.nprobe = n_probes;
        index.search(nq, xq, k, D_faiss_ip.get(), I_faiss_ip.get(), &p);
        profiler.End();
        std::cout << "searching data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
    }
    {
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", ann_benchmark_elapsed() - t0);
        std::cout << "############################" << std::endl;
        auto I = [&I_faiss_ip](size_t i) { return I_faiss_ip[i]; };
        compute_recall(gt, I, nq, k);
    }
}

void benchmark_annivfflatl2(double t0,
                            size_t d,
                            size_t nt,
                            const float *xt,
                            size_t nb,
                            const float *xb,
                            size_t nq,
                            const float *xq,
                            int k,
                            const int *gt,
                            size_t n_probes,
                            size_t n_centroids) {
    UniquePtr<AnnIVFFlatIndexData<float>> ann_index_data;
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Training and Indexing on %ld vectors, with %ld centroids\n", ann_benchmark_elapsed() - t0, nt, n_centroids);
        }

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        ann_index_data = AnnIVFFlatL2<float>::CreateIndex(d, nt, xt, nb, xb, n_centroids);
        profiler.End();
        std::cout << "training and adding data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB"
                  << std::endl;
    }
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Perform a search on %ld queries\n", ann_benchmark_elapsed() - t0, nq);
        }

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        AnnIVFFlatL2<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
        test_ivf.Begin();
        test_ivf.Search(ann_index_data.get(), 0, n_probes);
        test_ivf.End();
        profiler.End();
        std::cout << "searching data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        auto ID = test_ivf.GetIDs();
        auto D = test_ivf.GetDistances();
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", ann_benchmark_elapsed() - t0);
        std::cout << "############################" << std::endl;
        auto I = [ID](size_t i) { return ID[i].segment_offset_; };
        compute_recall(gt, I, nq, k);
    }
    {
        // test read/write file
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Save index to file\n", ann_benchmark_elapsed() - t0);
        }
        const String save_file_tmp = "/tmp/benchmark_annivfflatl2_tmp";
        ann_index_data->SaveIndex(save_file_tmp, MakeUnique<LocalFileSystem>());
        ann_index_data.reset();
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Old index deleted. Loading index from file\n", ann_benchmark_elapsed() - t0);
        }
        auto ann_index_data2 = AnnIVFFlatIndexData<f32>::LoadIndex(save_file_tmp, MakeUnique<LocalFileSystem>());
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Perform a search on %ld queries\n", ann_benchmark_elapsed() - t0, nq);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        AnnIVFFlatL2<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
        test_ivf.Begin();
        test_ivf.Search(ann_index_data2.get(), 0, n_probes);
        test_ivf.End();
        profiler.End();
        std::cout << "searching data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        auto ID = test_ivf.GetIDs();
        auto D = test_ivf.GetDistances();
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", ann_benchmark_elapsed() - t0);
        std::cout << "############################" << std::endl;
        auto I = [ID](size_t i) { return ID[i].segment_offset_; };
        compute_recall(gt, I, nq, k);
    }
}

void benchmark_annivfflatip(double t0,
                            size_t d,
                            size_t nt,
                            const float *xt,
                            size_t nb,
                            const float *xb,
                            size_t nq,
                            const float *xq,
                            int k,
                            const int *gt,
                            size_t n_probes,
                            size_t n_centroids) {
    UniquePtr<AnnIVFFlatIndexData<float>> ann_index_data;
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Training and Indexing on %ld vectors, with %ld centroids\n", ann_benchmark_elapsed() - t0, nt, n_centroids);
        }

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        ann_index_data = AnnIVFFlatIP<float>::CreateIndex(d, nt, xt, nb, xb, n_centroids);
        profiler.End();
        std::cout << "training and adding data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB"
                  << std::endl;
    }
    {
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Perform a search on %ld queries\n", ann_benchmark_elapsed() - t0, nq);
        }

        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        AnnIVFFlatIP<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
        test_ivf.Begin();
        test_ivf.Search(ann_index_data.get(), 0, n_probes);
        test_ivf.End();
        profiler.End();
        std::cout << "searching data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        auto ID = test_ivf.GetIDs();
        auto D = test_ivf.GetDistances();
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", ann_benchmark_elapsed() - t0);
        std::cout << "############################" << std::endl;
        auto I = [ID](size_t i) { return ID[i].segment_offset_; };
        compute_recall(gt, I, nq, k);
    }
    {
        // test read/write file
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Save index to file\n", ann_benchmark_elapsed() - t0);
        }
        const String save_file_tmp = "/tmp/benchmark_annivfflatip_tmp";
        ann_index_data->SaveIndex(save_file_tmp, MakeUnique<LocalFileSystem>());
        ann_index_data.reset();
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Old index deleted. Loading index from file\n", ann_benchmark_elapsed() - t0);
        }
        auto ann_index_data2 = AnnIVFFlatIndexData<f32>::LoadIndex(save_file_tmp, MakeUnique<LocalFileSystem>());
        if (ann_ivfflat_benchmark_verbose) {
            printf("[%.3f s] Perform a search on %ld queries\n", ann_benchmark_elapsed() - t0, nq);
        }
        infinity::BaseProfiler profiler;
        std::cout << "Begin memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        profiler.Begin();
        AnnIVFFlatIP<float> test_ivf(xq, nq, k, d, EmbeddingDataType::kElemFloat);
        test_ivf.Begin();
        test_ivf.Search(ann_index_data2.get(), 0, n_probes);
        test_ivf.End();
        profiler.End();
        std::cout << "searching data cost: " << profiler.ElapsedToString() << " memory cost: " << get_current_rss() / 1000000 << "MB" << std::endl;
        auto ID = test_ivf.GetIDs();
        auto D = test_ivf.GetDistances();
        std::cout << "############################" << std::endl;
        printf("[%.3f s] Compute recalls\n", ann_benchmark_elapsed() - t0);
        std::cout << "############################" << std::endl;
        auto I = [ID](size_t i) { return ID[i].segment_offset_; };
        compute_recall(gt, I, nq, k);
    }
}

int main() {
    auto t0 = ann_benchmark_elapsed();
    if (ann_ivfflat_benchmark_verbose) {
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] Loading train set\n", ann_benchmark_elapsed() - t0);
    }
    size_t d, k, n_probes = 10, n_centroids;
    size_t nt;
    float *xt = fvecs_read(sift1m_train, &d, &nt);
    UniquePtr<float> xt_destructor(xt);
    if (ann_ivfflat_benchmark_verbose) {
        printf("[%.3f s] Loading database\n", ann_benchmark_elapsed() - t0);
    }
    size_t nb, d2;
    float *xb = fvecs_read(sift1m_base, &d2, &nb);
    UniquePtr<float> xb_destructor(xb);
    assert(d == d2 || !"dataset does not have same dimension as train set");
    n_centroids = (size_t)sqrt(nb);
    if (ann_ivfflat_benchmark_verbose) {
        printf("[%.3f s] Loading queries\n", ann_benchmark_elapsed() - t0);
    }
    size_t nq, d3;
    float *xq = fvecs_read(sift1m_query, &d3, &nq);
    UniquePtr<float> xq_destructor(xq);
    assert(d == d2 || !"query does not have same dimension as train set");
    if (ann_ivfflat_benchmark_verbose) {
        printf("[%.3f s] Loading ground truth for %ld queries\n", ann_benchmark_elapsed() - t0, nq);
    }
    size_t nq2;
    int *gt_int = ivecs_read(sift1m_ground_truth, &k, &nq2);
    UniquePtr<int> gt_int_destructor(gt_int);
    assert(nq2 == nq || !"incorrect nb of ground truth entries");
    if (ann_ivfflat_benchmark_verbose) {
        printf("[%.3f s] Data loaded\n", ann_benchmark_elapsed() - t0);
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] Begin faiss ivfflatl2\n", ((t0 = ann_benchmark_elapsed()), 0.0));
        std::cout << "##########################################################" << std::endl;
    }
    benchmark_faiss_ivfflatl2(t0 = ann_benchmark_elapsed(), d, nt, xt, nb, xb, nq, xq, k, gt_int, n_probes, n_centroids);
    if (ann_ivfflat_benchmark_verbose) {
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] End faiss ivfflatl2\n", ann_benchmark_elapsed() - t0);
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] Begin ann ivfflatl2\n", ((t0 = ann_benchmark_elapsed()), 0.0));
        std::cout << "##########################################################" << std::endl;
    }
    benchmark_annivfflatl2(t0 = ann_benchmark_elapsed(), d, nt, xt, nb, xb, nq, xq, k, gt_int, n_probes, n_centroids);
    if (ann_ivfflat_benchmark_verbose) {
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] End ann ivfflatl2\n", ann_benchmark_elapsed() - t0);
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] Test inner product\n", ann_benchmark_elapsed() - t0);
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] Begin faiss ivfflatip\n", ((t0 = ann_benchmark_elapsed()), 0.0));
        std::cout << "##########################################################" << std::endl;
    }
    benchmark_faiss_ivfflatip(t0 = ann_benchmark_elapsed(), d, nt, xt, nb, xb, nq, xq, k, gt_int, n_probes, n_centroids);
    if (ann_ivfflat_benchmark_verbose) {
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] End faiss ivfflatip\n", ann_benchmark_elapsed() - t0);
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] Begin ann ivfflatip\n", ((t0 = ann_benchmark_elapsed()), 0.0));
        std::cout << "##########################################################" << std::endl;
    }
    benchmark_annivfflatip(t0 = ann_benchmark_elapsed(), d, nt, xt, nb, xb, nq, xq, k, gt_int, n_probes, n_centroids);
    if (ann_ivfflat_benchmark_verbose) {
        std::cout << "##########################################################" << std::endl;
        printf("[%.3f s] End ann ivfflatip\n", ann_benchmark_elapsed() - t0);
        std::cout << "##########################################################" << std::endl;
    }
    return 0;
}
