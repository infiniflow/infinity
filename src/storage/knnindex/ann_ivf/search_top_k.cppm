

module;

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mm_malloc.h>
#include <random>
#include <sys/time.h>

import stl;
import knn_heap;
import knn_result_handler;
import knn_distance;
import knn_partition;
import faiss;
import parser;
import third_party;
import infinity_assert;
import infinity_exception;
import index_data;
import vector_distance;

export module search_top_k;

namespace infinity {

export template <typename TypeX, typename TypeY, typename ID, typename DistType>
void search_top_1(i32 dimension, i32 nx, const TypeX *x, i32 ny, const TypeY *y, ID *labels, DistType *distances) {
    for (i32 i = 0; i < nx; ++i) {
        DistType min_dist = std::numeric_limits<DistType>::max();
        i32 min_idx = -1;
        for (i32 j = 0; j < ny; ++j) {
            DistType dist = L2Distance<DistType>(x + i * dimension, y + j * dimension, dimension);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        distances[i] = min_dist;
        labels[i] = min_idx;
    }
}

export template <typename DistType, typename TypeX, typename TypeY, typename ID>
void search_top_1(i32 dimension, i32 nx, const TypeX *x, i32 ny, const TypeY *y, ID *labels) {
    for (i32 i = 0; i < nx; ++i) {
        DistType min_dist = std::numeric_limits<DistType>::max();
        i32 min_idx = -1;
        for (i32 j = 0; j < ny; ++j) {
            DistType dist = L2Distance<DistType>(x + i * dimension, y + j * dimension, dimension);
            if (dist < min_dist) {
                min_dist = dist;
                min_idx = j;
            }
        }
        labels[i] = min_idx;
    }
}

export template <typename DistType>
void FakeSearch11(int d, int nq, float *x, int *result, IVFFlatIndexData<DistType> *base_ivf, float *dd) {
    Vector<int> list_id(nq);
    search_top_1<float>(d, nq, x, base_ivf->partition_num_, base_ivf->centroids_.data(), list_id.data());
    for (int i = 0; i < nq; ++i) {
        int offset;
        search_top_1<float>(d, 1, x + i * d, base_ivf->ids_[list_id[i]].size(), base_ivf->vectors_[list_id[i]].data(), &offset, dd + i);
        result[i] = base_ivf->ids_[list_id[i]][offset].block_offset_;
    }
}

} // namespace infinity