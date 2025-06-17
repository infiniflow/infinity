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

export module knn_scan_data;

import stl;
import table_function;

import global_block_id;
import merge_knn;
import roaring_bitmap;
import data_block;
import column_vector;
import base_expression;
import expression_state;
import knn_expr;
import statement_common;
import base_table_ref;
import internal_types;

namespace infinity {

class BlockMeta;
class TableIndexMeeta;
class SegmentIndexMeta;

export class KnnScanSharedData {
public:
    KnnScanSharedData(SharedPtr<BaseTableRef> table_ref,
                      UniquePtr<Vector<BlockMeta *>> block_metas,
                      SharedPtr<TableIndexMeeta> table_index_meta,
                      UniquePtr<Vector<SegmentIndexMeta>> segment_index_metas,
                      Vector<InitParameter> opt_params,
                      i64 topk,
                      i64 dimension,
                      i64 query_embedding_count,
                      void *query_embedding,
                      EmbeddingDataType elem_type,
                      KnnDistanceType knn_distance_type);

    ~KnnScanSharedData();

public:
    const SharedPtr<BaseTableRef> table_ref_{};

    UniquePtr<Vector<BlockMeta *>> block_metas_{};
    SharedPtr<TableIndexMeeta> table_index_meta_{};
    UniquePtr<Vector<SegmentIndexMeta>> segment_index_metas_{};

    const Vector<InitParameter> opt_params_{};
    const i64 topk_;
    const i64 dimension_;
    const u64 query_count_;
    void *const query_embedding_;
    const EmbeddingDataType query_elem_type_{EmbeddingDataType::kElemInvalid};
    const KnnDistanceType knn_distance_type_{KnnDistanceType::kInvalid};

    atomic_u64 current_block_idx_{0};
    atomic_u64 current_index_idx_{0};
};

//-------------------------------------------------------------------

export class KnnDistanceBase1 {
public:
    static UniquePtr<KnnDistanceBase1> Make(EmbeddingDataType embedding_type, KnnDistanceType distance_type);

    const KnnDistanceType dist_type_{};
    explicit KnnDistanceBase1(const KnnDistanceType dist_type) : dist_type_(dist_type) {}
    virtual ~KnnDistanceBase1() = default;
};

export template <typename QueryDataType, typename DistType>
class KnnDistance1 : public KnnDistanceBase1 {
public:
    explicit KnnDistance1(const KnnDistanceType dist_type) : KnnDistanceBase1(dist_type) { InitKnnDistance1(dist_type); }

    void InitKnnDistance1(KnnDistanceType dist_type);

    Vector<DistType> Calculate(const QueryDataType *datas, SizeT data_count, const QueryDataType *query, SizeT dim) const {
        Vector<DistType> res(data_count);
        for (SizeT i = 0; i < data_count; ++i) {
            res[i] = dist_func_(query, datas + i * dim, dim);
        }
        return res;
    }

    Vector<DistType> Calculate(const QueryDataType *datas, SizeT data_count, const QueryDataType *query, SizeT dim, Bitmask &bitmask) const {
        Vector<DistType> res(data_count);
        for (SizeT i = 0; i < data_count; ++i) {
            if (bitmask.IsTrue(i)) {
                res[i] = dist_func_(query, datas + i * dim, dim);
            }
        }
        return res;
    }

public:
    using DistFunc = DistType (*)(const QueryDataType *, const QueryDataType *, SizeT);

    DistFunc dist_func_{};
};

template <>
void KnnDistance1<f32, f32>::InitKnnDistance1(KnnDistanceType dist_type);

template <>
void KnnDistance1<u8, i32>::InitKnnDistance1(KnnDistanceType dist_type);

template <>
void KnnDistance1<u8, f32>::InitKnnDistance1(KnnDistanceType dist_type);

template <>
void KnnDistance1<i8, i32>::InitKnnDistance1(KnnDistanceType dist_type);

template <>
void KnnDistance1<i8, f32>::InitKnnDistance1(KnnDistanceType dist_type);

//-------------------------------------------------------------------

export class KnnScanFunctionData final : public TableFunctionData {
public:
    KnnScanFunctionData(KnnScanSharedData *shared_data, u32 current_parallel_idx, bool execute_block_scan_job);

    ~KnnScanFunctionData() final = default;

public:
    KnnScanSharedData *knn_scan_shared_data_;
    const u32 task_id_;
    bool execute_block_scan_job_ = false;

    UniquePtr<MergeKnnBase> merge_knn_base_{};
    UniquePtr<KnnDistanceBase1> knn_distance_{};

    SharedPtr<ExpressionState> filter_state_{};
    UniquePtr<DataBlock> db_for_filter_{};
    SharedPtr<ColumnVector> bool_column_{};
};

} // namespace infinity
