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

export module infinity_core:hnsw_lsg_builder;

import std;
import std.compat;

import column_def;
import row_id;

namespace infinity {

struct HnswIndexInMem;
class BufferManager;
struct IVF_Search_Params;
class KnnDistanceBase1;
class IVFIndexInChunk;
class IndexHnsw;

export class HnswLSGBuilder {
public:
    HnswLSGBuilder(const IndexHnsw *index_hnsw, std::shared_ptr<ColumnDef> column_def);

    ~HnswLSGBuilder();

    template <typename Iter, typename DataType, typename DistanceDataType>
    std::unique_ptr<HnswIndexInMem> MakeImplIter(Iter iter, size_t row_count, const RowID &base_row_id, bool trace);

public:
    template <typename Iter, typename DataType, typename DistanceDataType>
    std::unique_ptr<float[]> GetLSAvg(Iter iter, size_t row_count, const RowID &base_row_id);

private:
    std::unique_ptr<IVFIndexInChunk> MakeIVFIndex();

    IVF_Search_Params MakeIVFSearchParams() const;

    template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
    std::unique_ptr<float[]> GetAvgByIVF(Iter iter, size_t row_count);

    template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
    std::unique_ptr<float[]> GetAvgBF(Iter iter, size_t row_count);

private:
    const IndexHnsw *index_hnsw_ = nullptr;
    const std::shared_ptr<ColumnDef> column_def_ = nullptr;

    std::unique_ptr<KnnDistanceBase1> knn_distance_;
};

} // namespace infinity
