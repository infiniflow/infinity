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

namespace infinity {
struct SegmentEntry;
class ColumnDef;
struct RowID;
} // namespace infinity

export module new_hnsw_lsg_builder;

import stl;

namespace infinity {

struct HnswIndexInMem;
class BufferManager;
struct IVF_Search_Params;
class KnnDistanceBase1;
class IVFIndexInChunk;
class IndexHnsw;

export class HnswLSGBuilder {
public:
    HnswLSGBuilder(const IndexHnsw *index_hnsw, SharedPtr<ColumnDef> column_def);

    ~HnswLSGBuilder();

    UniquePtr<HnswIndexInMem> Make(const SegmentEntry *segment_entry, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts, BufferManager *buffer_mgr, bool trace = true);

    template <typename Iter, typename DataType, typename DistanceDataType>
    UniquePtr<HnswIndexInMem> MakeImplIter(Iter iter, SizeT row_count, const RowID &base_row_id, bool trace);

private:
    template <typename DataType, typename DistanceDataType>
    UniquePtr<HnswIndexInMem> MakeImpl(const SegmentEntry *segment_entry, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts, BufferManager *buffer_mgr, bool trace);

public:
    template <typename Iter, typename DataType, typename DistanceDataType>
    UniquePtr<float[]> GetLSAvg(Iter iter, SizeT row_count, const RowID &base_row_id);

private:
    UniquePtr<IVFIndexInChunk> MakeIVFIndex();

    IVF_Search_Params MakeIVFSearchParams() const;

    template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
    UniquePtr<float[]> GetAvgByIVF(Iter iter, SizeT row_count);

    template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
    UniquePtr<float[]> GetAvgBF(Iter iter, SizeT row_count);

private:
    const IndexHnsw *index_hnsw_ = nullptr;
    const SharedPtr<ColumnDef> column_def_ = nullptr;

    UniquePtr<KnnDistanceBase1> knn_distance_;
};

} // namespace infinity
