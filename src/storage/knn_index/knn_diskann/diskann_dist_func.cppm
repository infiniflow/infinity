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

module;
#include <type_traits>
import stl;
import simd_functions;
import mlas_matrix_multiply;
import search_top_1;
import diskann_simd_func;
import infinity_exception;

export module diskann_dist_func;

namespace infinity {
export enum class DiskAnnMetricType{
    L2,
    Cosine,
    IP,
    Invalid
};

export template <typename DiffType, typename ElemType1, typename ElemType2, typename DimType = u32>
DiffType L2Distance(const ElemType1 *vector1, const ElemType2 *vector2, const DimType dimension) {
    if constexpr (std::is_same_v<ElemType1, f32> && std::is_same_v<ElemType2, f32>) {
        return GetSIMD_FUNCTIONS().L2Distance_func_ptr_(vector1, vector2, static_cast<SizeT>(dimension));
    } else {
        DiffType distance{};
        for (u32 i = 0; i < dimension; ++i) {
            auto diff = (DiffType)(vector1[i] - vector2[i]);
            distance += diff * diff;
        }
        return distance;
    }
}

export template <typename DiffType, typename ElemType1, typename ElemType2, typename DimType = u32>
DiffType CosineDistance(const ElemType1 *vector1, const ElemType2 *vector2, const DimType dimension) {
    if constexpr (std::is_same_v<ElemType1, f32> && std::is_same_v<ElemType2, f32>) {
        return GetSIMD_FUNCTIONS().CosineDistance_func_ptr_(vector1, vector2, static_cast<SizeT>(dimension));
    } else {
        DiffType dot_product{};
        DiffType norm1{};
        DiffType norm2{};
        for (u32 i = 0; i < dimension; ++i) {
            dot_product += ((DiffType)vector1[i]) * ((DiffType)vector2[i]);
            norm1 += ((DiffType)vector1[i]) * ((DiffType)vector1[i]);
            norm2 += ((DiffType)vector2[i]) * ((DiffType)vector2[i]);
        }
        return dot_product != 0 ? dot_product / sqrt(norm1 * norm2) : 0;
    }
}

export template <typename DiffType, typename ElemType1, typename ElemType2, typename DimType = u32>
DiffType IPDistance(const ElemType1 *vector1, const ElemType2 *vector2, const DimType dimension) {
    if constexpr (std::is_same_v<ElemType1, f32> && std::is_same_v<ElemType2, f32>) {
        return GetSIMD_FUNCTIONS().IPDistance_func_ptr_(vector1, vector2, static_cast<SizeT>(dimension));
    } else {
        DiffType distance{};
        for (u32 i = 0; i < dimension; ++i) {
            distance += ((DiffType)vector1[i]) * ((DiffType)vector2[i]);
        }
        return distance;
    }
}

// Compute the L2 squared norm of vector
export template <typename ElemType = f32, typename DimType = u32>
void ComputL2sq(ElemType *vecs_l2sq, ElemType *data, const DimType dim){
    matrixA_multiply_transpose_matrixB_output_to_C(data, data, 1, 1, dim, vecs_l2sq);
    *vecs_l2sq = sqrt(*vecs_l2sq);
}

export void ComputeClosestCenters(f32 *data, SizeT num_points, SizeT dim,
                             f32 *pivot_data, SizeT num_centers,
                             u32 *closest_centers_ivf,
                             Vector<SizeT> *inverted_index = nullptr){
    search_top_1_without_dis<f32, f32, f32, u32>(dim,
                            num_points,
                            data,
                            num_centers,
                            pivot_data,
                            closest_centers_ivf);

    if (inverted_index != nullptr){
        for (SizeT i = 0; i < num_points; ++i) {
            inverted_index[closest_centers_ivf[i]].push_back(i);
        }
    }
}

export inline u64 RoundUp(u64 X, u64 Y){
    return ((X + Y - 1) / Y) * Y;
}

export inline u64 DivRoundUp(u64 X, u64 Y){
    return (X / Y) + (X % Y != 0);
}


export template <typename DataType>
class DiskAnnDistance{
    using This = DiskAnnDistance<DataType>;
public:
    DiskAnnDistance() : distance_metric_(DiskAnnMetricType::Invalid) {};
    DiskAnnDistance(DiskAnnMetricType metric): distance_metric_(metric){}

    ~DiskAnnDistance(){}

    static This Make(DiskAnnMetricType metric) {
        return This(metric);
    }

    f32 Compare(DataType *x, DataType *y, SizeT dim) const {
        if (distance_metric_ == DiskAnnMetricType::Cosine) {
            return CosineDistance<f32, DataType, DataType>(x, y, dim);
        } else if (distance_metric_ == DiskAnnMetricType::L2) {
            return L2Distance<f32, DataType, DataType>(x, y, dim);
        } else if (distance_metric_ == DiskAnnMetricType::IP) {
            return IPDistance<f32, DataType, DataType>(x, y, dim);
        } else {
            String error_message = "Metric type is invalid";
            UnrecoverableError(error_message);
            return 0;
        }
    }

    DiskAnnMetricType GetMetric() const {
        return distance_metric_;
    }

    bool PreprocessingRequired() const {
        return false;
    }

    void PreprocessBasePoints(DataType *original_data, const SizeT orig_dim, const SizeT num_points){
        // preprocessing not required now
        return;
    }

    void PreprocessQuery(const DataType *query_vec, const SizeT query_dim, DataType *scratch_query){
        // preprocessing not required now, just copy the query to scratch_query
        memcpy(scratch_query, query_vec, query_dim * sizeof(DataType));

        // If preprocessing is required, normalise and copy the query
        // normalizeAndCopy(query_vec, (uint32_t)query_dim, query_scratch);
    }

    SizeT GetRequiredAlignment() const {
        return alignment_factor_;
    }

protected:
    DiskAnnMetricType distance_metric_;
    SizeT alignment_factor_ = 8;
};

} // namespace infinity