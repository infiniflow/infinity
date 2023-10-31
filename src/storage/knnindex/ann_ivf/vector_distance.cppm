//
// Created by yzq on 10/31/23.
//

module;

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

export module vector_distance;

namespace infinity {
// export template <typename ElemType1, typename ElemType2, typename DiffType = ElemType1>
export template <typename DiffType, typename ElemType1, typename ElemType2>
DiffType L2Distance(const ElemType1 *vector1, const ElemType2 *vector2, i32 dimension) {
    DiffType distance{};
    for (i32 i = 0; i < dimension; ++i) {
        DiffType diff = vector1[i] - vector2[i];
        distance += diff * diff;
    }
    return distance;
}

export template <typename ElemType1, typename ElemType2, typename DiffType = ElemType1>
DiffType IPDistance(const ElemType1 *vector1, const ElemType2 *vector2, i32 dimension) {
    DiffType distance{};
    for (i32 i = 0; i < dimension; ++i) {
        distance += static_cast<DiffType>(vector1[i]) * static_cast<DiffType>(vector2[i]);
    }
    return distance;
}

} // namespace infinity
