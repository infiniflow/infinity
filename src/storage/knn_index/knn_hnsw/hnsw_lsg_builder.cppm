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

export module hnsw_lsg_builder;

import stl;

namespace infinity {

struct HnswIndexInMem;
struct SegmentEntry;
class ColumnDef;
class IndexHnsw;
class BufferManager;
struct IVF_Search_Params;
class KnnDistanceBase1;
class IVFIndexInChunk;
struct RowID;

export struct LSGConfig {
    float sample_raito_ = 0.01;
    SizeT ls_k_ = 10;
    float alpha_ = 1.0;
};

export class HnswLSGBuilder {
public:
    HnswLSGBuilder(const IndexHnsw *index_hnsw, SharedPtr<ColumnDef> column_def, LSGConfig lsg_config);

    ~HnswLSGBuilder();

    UniquePtr<HnswIndexInMem> Make(SegmentEntry *segment_entry, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts, BufferManager *buffer_mgr);

    template <typename Iter>
    UniquePtr<HnswIndexInMem> Make(Iter iter, SizeT row_count, const RowID &base_row_id);

private:
    template <typename Iter, typename DataType, typename DistanceDataType>
    UniquePtr<HnswIndexInMem> MakeImpl(Iter iter, SizeT row_count, const RowID &base_row_id);

    template <typename Iter, typename DataType, typename DistanceDataType>
    UniquePtr<DataType[]> GetLSAvg(Iter iter, SizeT row_count, const RowID &base_row_id);

    UniquePtr<IVFIndexInChunk> MakeIVFIndex();

    IVF_Search_Params MakeIVFSearchParams() const;

    template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
    UniquePtr<DataType[]> GetAvgByIVF(Iter iter, SizeT row_count, const IVFIndexInChunk *ivf_index);

private:
    const IndexHnsw *index_hnsw_ = nullptr;
    const SharedPtr<ColumnDef> column_def_ = nullptr;
    const LSGConfig lsg_config_;

    UniquePtr<KnnDistanceBase1> knn_distance_;
};

} // namespace infinity
