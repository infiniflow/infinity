
#include "helper.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>

static const char *sift1m_train = "./test/data/fvecs/sift_learn.fvecs";
static const char *sift1m_base = "./test/data/fvecs/sift_base.fvecs";
static const char *sift1m_query = "./test/data/fvecs/sift_query.fvecs";
static const char *sift1m_ground_truth = "./test/data/fvecs/sift_groundtruth.ivecs";
static const char *sift1m_base_split_1 = "./test/data/fvecs/sift_base_split_1.fvecs";
static const char *sift1m_base_split_2 = "./test/data/fvecs/sift_base_split_2.fvecs";
static const char *sift1m_base_split_3 = "./test/data/fvecs/sift_base_split_3.fvecs";
static const char *sift1m_base_split_4 = "./test/data/fvecs/sift_base_split_4.fvecs";
static const char *sift1m_base_split_5 = "./test/data/fvecs/sift_base_split_5.fvecs";

constexpr bool create_split_file = false;
constexpr bool check_split_file = false;

float *fvecs_read(const char *fname, size_t *d_out, size_t *n_out);

int *ivecs_read(const char *fname, size_t *d_out, size_t *n_out);

int main() {
    size_t d, k, n_probes = 10, n_centroids;
    size_t nt;
    float *xt = fvecs_read(sift1m_train, &d, &nt);
    std::unique_ptr<float> xt_destructor(xt);
    size_t nb, d2;
    float *xb = fvecs_read(sift1m_base, &d2, &nb);
    std::unique_ptr<float> xb_destructor(xb);
    assert(d == d2 || !"dataset does not have same dimension as train set");
    n_centroids = (size_t)sqrt(nb);
    size_t nq, d3;
    float *xq = fvecs_read(sift1m_query, &d3, &nq);
    std::unique_ptr<float> xq_destructor(xq);
    assert(d == d3 || !"query does not have same dimension as train set");
    size_t nq2;
    int *gt_int = ivecs_read(sift1m_ground_truth, &k, &nq2);
    std::unique_ptr<int> gt_int_destructor(gt_int);
    assert(nq2 == nq || !"incorrect nb of ground truth entries");
    size_t split_size = nb / 5;
    if constexpr (check_split_file) {
        size_t pos = 0;
        size_t nb_1, d_1;
        float *xb_1 = fvecs_read(sift1m_base_split_1, &d_1, &nb_1);
        std::unique_ptr<float> xb_1_destructor(xb_1);
        size_t nb_2, d_2;
        float *xb_2 = fvecs_read(sift1m_base_split_2, &d_2, &nb_2);
        std::unique_ptr<float> xb_2_destructor(xb_2);
        size_t nb_3, d_3;
        float *xb_3 = fvecs_read(sift1m_base_split_3, &d_3, &nb_3);
        std::unique_ptr<float> xb_3_destructor(xb_3);
        size_t nb_4, d_4;
        float *xb_4 = fvecs_read(sift1m_base_split_4, &d_4, &nb_4);
        std::unique_ptr<float> xb_4_destructor(xb_4);
        size_t nb_5, d_5;
        float *xb_5 = fvecs_read(sift1m_base_split_5, &d_5, &nb_5);
        std::unique_ptr<float> xb_5_destructor(xb_5);
        assert(d == d_1 && d == d_2 && d == d_3 && d == d_4 && d == d_5 || !"dataset does not have same dimension as train set");
        assert(nb == nb_1 + nb_2 + nb_3 + nb_4 + nb_5 || !"split dataset error");
        std::cout << "d: " << d << std::endl;
        std::cout << "nb: " << nb << std::endl;
        std::cout << "nb_1: " << nb_1 << std::endl;
        std::cout << "nb_2: " << nb_2 << std::endl;
        std::cout << "nb_3: " << nb_3 << std::endl;
        std::cout << "nb_4: " << nb_4 << std::endl;
        std::cout << "nb_5: " << nb_5 << std::endl;
        // check whether xb_1, xb_2, xb_3, xb_4, xb_5 are the same as xb
        for (size_t i = 0; i < nb_1; ++i) {
            for (size_t j = 0; j < d; ++j) {
                assert(xb_1[i * d + j] == xb[i * d + j] || !"split dataset error");
            }
        }
        for (size_t i = 0; i < nb_2; ++i) {
            for (size_t j = 0; j < d; ++j) {
                assert(xb_2[i * d + j] == xb[(i + nb_1) * d + j] || !"split dataset error");
            }
        }
        for (size_t i = 0; i < nb_3; ++i) {
            for (size_t j = 0; j < d; ++j) {
                assert(xb_3[i * d + j] == xb[(i + nb_1 + nb_2) * d + j] || !"split dataset error");
            }
        }
        for (size_t i = 0; i < nb_4; ++i) {
            for (size_t j = 0; j < d; ++j) {
                assert(xb_4[i * d + j] == xb[(i + nb_1 + nb_2 + nb_3) * d + j] || !"split dataset error");
            }
        }
        for (size_t i = 0; i < nb_5; ++i) {
            for (size_t j = 0; j < d; ++j) {
                assert(xb_5[i * d + j] == xb[(i + nb_1 + nb_2 + nb_3 + nb_4) * d + j] || !"split dataset error");
            }
        }
        std::cout << "split dataset success" << std::endl;
    }
    if constexpr (create_split_file) {
        size_t pos = 0;
        std::ofstream base_split_1(sift1m_base_split_1);
        std::ofstream base_split_2(sift1m_base_split_2);
        std::ofstream base_split_3(sift1m_base_split_3);
        std::ofstream base_split_4(sift1m_base_split_4);
        std::ofstream base_split_5(sift1m_base_split_5);
        int dim = d;
        for (; pos < split_size; ++pos) {
            base_split_1.write((const char *)(&dim), sizeof(int));
            base_split_1.write((const char *)(xb + pos * d), d * sizeof(float));
        }
        for (; pos < split_size * 2; ++pos) {
            base_split_2.write((const char *)(&dim), sizeof(int));
            base_split_2.write((const char *)(xb + pos * d), d * sizeof(float));
        }
        for (; pos < split_size * 3; ++pos) {
            base_split_3.write((const char *)(&dim), sizeof(int));
            base_split_3.write((const char *)(xb + pos * d), d * sizeof(float));
        }
        for (; pos < split_size * 4; ++pos) {
            base_split_4.write((const char *)(&dim), sizeof(int));
            base_split_4.write((const char *)(xb + pos * d), d * sizeof(float));
        }
        for (; pos < nb; ++pos) {
            base_split_5.write((const char *)(&dim), sizeof(int));
            base_split_5.write((const char *)(xb + pos * d), d * sizeof(float));
        }
    }
}
