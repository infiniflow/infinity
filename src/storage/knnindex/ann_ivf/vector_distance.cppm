//
// Created by yzq on 10/31/23.
//

module;

import stl;

export module vector_distance;

namespace infinity {
export template <typename DiffType, typename ElemType1, typename ElemType2>
DiffType L2Distance(const ElemType1 *vector1, const ElemType2 *vector2, u32 dimension) {
    DiffType distance{};
    for (u32 i = 0; i < dimension; ++i) {
        auto diff = (DiffType)(vector1[i] - vector2[i]);
        distance += diff * diff;
    }
    return distance;
}

export template <typename DiffType, typename ElemType1, typename ElemType2>
DiffType IPDistance(const ElemType1 *vector1, const ElemType2 *vector2, u32 dimension) {
    DiffType distance{};
    for (u32 i = 0; i < dimension; ++i) {
        distance += ((DiffType)vector1[i]) * ((DiffType)vector2[i]);
    }
    return distance;
}

} // namespace infinity
