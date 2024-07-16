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

export module abstract_hnsw;

import stl;
import hnsw_alg;
import data_store;
import vec_store_type;
import dist_func_l2;
import dist_func_ip;
import file_system;
import hnsw_common;
import column_def;
import index_hnsw;
import infinity_exception;
import index_base;
import logger;
import internal_types;
import embedding_info;

namespace infinity {

class BufferManager;
class ChunkIndexEntry;
class SegmentIndexEntry;

export using AbstractHnsw = std::variant<KnnHnsw<PlainCosVecStoreType<float>, SegmentOffset> *,
                                         KnnHnsw<PlainIPVecStoreType<float>, SegmentOffset> *,
                                         KnnHnsw<PlainL2VecStoreType<float>, SegmentOffset> *,
                                         KnnHnsw<LVQCosVecStoreType<float, i8>, SegmentOffset> *,
                                         KnnHnsw<LVQIPVecStoreType<float, i8>, SegmentOffset> *,
                                         KnnHnsw<LVQL2VecStoreType<float, i8>, SegmentOffset> *,
                                         std::nullptr_t>;

export struct HnswIndexInMem {
public:
    HnswIndexInMem() : hnsw_(nullptr) {}

    HnswIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def)
        : begin_row_id_(begin_row_id), hnsw_(InitAbstractIndex(index_base, column_def)) {}

private:
    template <typename DataType>
    static AbstractHnsw InitAbstractIndex(const IndexHnsw *index_hnsw, const EmbeddingInfo *embedding_info) {
        SizeT chunk_size = 8192;
        SizeT max_chunk_num = 1024;
        SizeT dim = embedding_info->Dimension();
        SizeT M = index_hnsw->M_;
        SizeT ef_construction = index_hnsw->ef_construction_;
        switch (index_hnsw->encode_type_) {
            case HnswEncodeType::kPlain: {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<PlainL2VecStoreType<DataType>, SegmentOffset>;
                        return HnswIndex::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<PlainIPVecStoreType<DataType>, SegmentOffset>;
                        return HnswIndex::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<PlainCosVecStoreType<DataType>, SegmentOffset>;
                        return HnswIndex::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    }
                    default: {
                        return nullptr;
                    }
                }
            }
            case HnswEncodeType::kLVQ: {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricL2: {
                        using HnswIndex = KnnHnsw<LVQL2VecStoreType<DataType, i8>, SegmentOffset>;
                        return HnswIndex::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    }
                    case MetricType::kMetricInnerProduct: {
                        using HnswIndex = KnnHnsw<LVQIPVecStoreType<DataType, i8>, SegmentOffset>;
                        return HnswIndex::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    }
                    case MetricType::kMetricCosine: {
                        using HnswIndex = KnnHnsw<LVQCosVecStoreType<DataType, i8>, SegmentOffset>;
                        return HnswIndex::Make(chunk_size, max_chunk_num, dim, M, ef_construction).release();
                    }
                    default: {
                        return nullptr;
                    }
                }
            }
            default: {
                return nullptr;
            }
        }
    }

public:
    static AbstractHnsw InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def) {
        const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
        const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get());

        switch (embedding_info->Type()) {
            case EmbeddingDataType::kElemFloat: {
                return InitAbstractIndex<float>(index_hnsw, embedding_info);
            }
            default: {
                return nullptr;
            }
        }
    }

    ~HnswIndexInMem() {
        std::visit(
            [](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                    if (arg != nullptr) {
                        delete arg;
                    }
                }
            },
            hnsw_);
    }

    SizeT GetRowCount() const {
        return std::visit(
            [](auto &&index) {
                using IndexType = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                    return SizeT(0);
                } else {
                    return index->GetVecNum();
                }
            },
            hnsw_);
    }

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr);

    const AbstractHnsw &get() const { return hnsw_; }

    AbstractHnsw &get_ref() { return hnsw_; }

private:
    RowID begin_row_id_ = {};
    AbstractHnsw hnsw_ = nullptr;
};

} // namespace infinity