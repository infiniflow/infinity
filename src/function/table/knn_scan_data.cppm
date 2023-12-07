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

import stl;
import table_function;
import parser;
import global_block_id;
import knn_distance; // delete this
import block_index;
import block_column_entry;
import merge_knn;
import segment_column_index_entry;
import bitmask;

import base_table_ref;

export module knn_scan_data;

namespace infinity {

export class KnnScanSharedData {
public:
    KnnScanSharedData(SharedPtr<BaseTableRef> table_ref,
                      Vector<BlockColumnEntry *> block_column_entries,
                      Vector<SegmentColumnIndexEntry *> index_entries,
                      i64 topk,
                      i64 dimension,
                      i64 query_embedding_count,
                      void *query_embedding,
                      EmbeddingDataType elem_type,
                      KnnDistanceType knn_distance_type)
        : table_ref_(table_ref), block_column_entries_(Move(block_column_entries)), index_entries_(Move(index_entries)), topk_(topk),
          dimension_(dimension), query_count_(query_embedding_count), query_embedding_(query_embedding), elem_type_(elem_type),
          knn_distance_type_(knn_distance_type) {}

public:
    const SharedPtr<BaseTableRef> table_ref_{};

    const Vector<BlockColumnEntry *> block_column_entries_{};
    const Vector<SegmentColumnIndexEntry *> index_entries_{};

    const i64 topk_;
    const i64 dimension_;
    const u64 query_count_;
    void *const query_embedding_;
    const EmbeddingDataType elem_type_{EmbeddingDataType::kElemInvalid};
    const KnnDistanceType knn_distance_type_{KnnDistanceType::kInvalid};

    atomic_u64 current_block_idx_{0};
    atomic_u64 current_index_idx_{0};
};

//-------------------------------------------------------------------

export class KnnDistanceBase1 {};

export template <typename DataType>
class KnnDistance1 : public KnnDistanceBase1 {
public:
    KnnDistance1(KnnDistanceType dist_type);

    Vector<DataType> Calculate(const DataType *datas, SizeT data_count, const DataType *query, SizeT dim) {
        Vector<DataType> res(data_count);
        for (SizeT i = 0; i < data_count; ++i) {
            res[i] = dist_func_(query, datas + i * dim, dim);
        }
        return res;
    }

    Vector <DataType>
    Calculate(const DataType *datas, SizeT data_count, const DataType *query, SizeT dim, Bitmask &bitmask) {
        Vector <DataType> res(data_count);
        for (SizeT i = 0; i < data_count; ++i) {
            if (bitmask.IsTrue(i)) {
                res[i] = dist_func_(query, datas + i * dim, dim);
            }
        }
        return res;
    }

public:
    using DistFunc = DataType (*)(const DataType *, const DataType *, SizeT);

    DistFunc dist_func_{};
};

template <>
KnnDistance1<f32>::KnnDistance1(KnnDistanceType dist_type);

//-------------------------------------------------------------------

export class KnnScanFunctionData1 : public TableFunctionData {
public:
    KnnScanFunctionData1(SharedPtr<KnnScanSharedData> shared_data, u32 current_parallel_idx);

private:
    template <typename DataType>
    void Init();

public:
    const SharedPtr<KnnScanSharedData> shared_data_;
    const u32 task_id_;

    UniquePtr<MergeKnnBase> merge_knn_base_{};
    UniquePtr<KnnDistanceBase1> knn_distance_{};
};

} // namespace infinity
