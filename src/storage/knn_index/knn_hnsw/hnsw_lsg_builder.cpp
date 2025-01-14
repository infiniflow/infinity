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

#include <vector>

module hnsw_lsg_builder;

import stl;
import abstract_hnsw;
import segment_entry;
import column_def;
import index_hnsw;
import index_ivf;
import statement_common;
import defer_op;
import infinity_exception;
import index_base;
import ivf_index_data;
import third_party;
import internal_types;
import segment_iter;
import knn_scan_data;
import knn_expr;
import ivf_index_search;
import data_type;
import roaring_bitmap;
import knn_result_handler;
import logical_type;
import embedding_info;
import hnsw_common;

namespace infinity {

namespace {

template <typename DataType>
DataType GetMean(const DataType *data, u32 size) {
    DataType sum = 0;
    for (u32 i = 0; i < size; ++i) {
        sum += data[i];
    }
    return sum / size;
}

class IterIVFDataAccessor : public IVFDataAccessorBase {
public:
    IterIVFDataAccessor() {}

    template <typename Iter>
    void InitEmbedding(Iter iter, SizeT row_count) {
        embeddings_.resize(row_count);
        while (true) {
            if (auto ret = iter.Next(); ret) {
                auto &[embedding, offset] = *ret;
                embeddings_[offset] = reinterpret_cast<const_ptr_t>(embedding);
            } else {
                break;
            }
        }
    }

    const_ptr_t GetEmbedding(SizeT offset) override { return embeddings_[offset]; }

    Pair<Span<const char>, SizeT> GetMultiVector(SizeT offset) override {
        UnrecoverableError("Not implemented");
        return {};
    }

private:
    Vector<const_ptr_t> embeddings_;
};

} // namespace

HnswLSGBuilder::HnswLSGBuilder(const IndexHnsw *index_hnsw, SharedPtr<ColumnDef> column_def, LSGConfig lsg_config)
    : index_hnsw_(index_hnsw), column_def_(column_def), lsg_config_(std::move(lsg_config)) {
    const DataType *column_type = column_def_->type().get();
    if (column_type->type() != LogicalType::kEmbedding) {
        UnrecoverableError("Invalid column type");
    }
    auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());

    KnnDistanceType distance_type = KnnDistanceType::kInvalid;
    switch (index_hnsw_->metric_type_) {
        case MetricType::kMetricL2:
            distance_type = KnnDistanceType::kL2;
            break;
        case MetricType::kMetricInnerProduct:
            distance_type = KnnDistanceType::kInnerProduct;
            break;
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
    knn_distance_ = KnnDistanceBase1::Make(embedding_info->Type(), distance_type);
}

HnswLSGBuilder::~HnswLSGBuilder() = default;

UniquePtr<HnswIndexInMem>
HnswLSGBuilder::Make(SegmentEntry *segment_entry, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts, BufferManager *buffer_mgr) {
    OneColumnIterator<DataType> iter(segment_entry, buffer_mgr, column_id, begin_ts);
    SizeT row_count = segment_entry->row_count();
    RowID base_row_id(segment_entry->segment_id(), 0);
    return Make(std::move(iter), row_count, base_row_id);
}

template <typename Iter>
UniquePtr<HnswIndexInMem> HnswLSGBuilder::Make(Iter iter, SizeT row_count, const RowID &base_row_id) {
    const DataType *column_type = column_def_->type().get();
    auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    switch (embedding_info->Type()) {
        case EmbeddingDataType::kElemFloat:
            return MakeImpl<Iter, float, float>(std::move(iter), row_count, base_row_id);
        case EmbeddingDataType::kElemDouble:
            return MakeImpl<Iter, double, float>(std::move(iter), row_count, base_row_id);
        default:
            UnrecoverableError("Invalid embedding data type");
            return nullptr;
    }
}

template <>
UniquePtr<HnswIndexInMem> HnswLSGBuilder::Make<DenseVectorIter<f32, i32>>(DenseVectorIter<f32, i32> iter, SizeT row_count, const RowID &base_row_id) {
    const DataType *column_type = column_def_->type().get();
    auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    switch (embedding_info->Type()) {
        case EmbeddingDataType::kElemFloat:
            return MakeImpl<DenseVectorIter<f32, i32>, float, float>(std::move(iter), row_count, base_row_id);
        case EmbeddingDataType::kElemDouble:
            return MakeImpl<DenseVectorIter<f32, i32>, double, float>(std::move(iter), row_count, base_row_id);
        default:
            UnrecoverableError("Invalid embedding data type");
            return nullptr;
    }
}

template <typename Iter, typename DataType, typename DistanceDataType>
UniquePtr<HnswIndexInMem> HnswLSGBuilder::MakeImpl(Iter iter, SizeT row_count, const RowID &base_row_id) {
    [[maybe_unused]] Iter iter_copy = iter;
    [[maybe_unused]] auto avg = GetLSAvg<Iter, DataType, DistanceDataType>(iter, row_count, base_row_id);
    return nullptr;
}

template <typename Iter, typename DataType, typename DistanceDataType>
UniquePtr<DataType[]> HnswLSGBuilder::GetLSAvg(Iter iter, SizeT row_count, const RowID &base_row_id) {
    auto ivf_index = MakeIVFIndex();
    {
        IterIVFDataAccessor iter_accessor;
        Iter iter_copy = iter;
        iter_accessor.InitEmbedding(std::move(iter_copy), row_count);
        ivf_index->BuildIVFIndex(base_row_id, row_count, &iter_accessor, column_def_);
    }

    UniquePtr<DataType[]> avg;
    switch (index_hnsw_->metric_type_) {
        case MetricType::kMetricL2:
            avg = GetAvgByIVF<Iter, DataType, CompareMax, DistanceDataType>(std::move(iter), row_count, ivf_index.get());
            break;
        case MetricType::kMetricInnerProduct:
            avg = GetAvgByIVF<Iter, DataType, CompareMin, DistanceDataType>(std::move(iter), row_count, ivf_index.get());
            break;
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
    return avg;
}

UniquePtr<IVFIndexInChunk> HnswLSGBuilder::MakeIVFIndex() {
    auto index_name = MakeShared<String>("tmp_index_name");
    String filename = "tmp_index_file";
    Vector<String> column_names = {column_def_->name()};

    Vector<InitParameter *> parameters;
    DeferFn defer_params([&] {
        for (auto parameter : parameters) {
            delete parameter;
        }
    });
    {
        String metric = "";
        switch (index_hnsw_->metric_type_) {
            case MetricType::kMetricL2:
                metric = "l2";
                break;
            case MetricType::kMetricInnerProduct:
                metric = "ip";
                break;
            default:
                UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
        }
        parameters.push_back(new InitParameter("metric", std::move(metric)));
    }
    {
        String sample_raito_ = std::to_string(lsg_config_.sample_raito_);
        parameters.push_back(new InitParameter("centroids_num_ratio", std::move(sample_raito_)));
    }
    parameters.push_back(new InitParameter("storage_type", "plain"));

    SharedPtr<IndexIVF> index_ivf = IndexIVF::Make(index_name, nullptr, filename, std::move(column_names), parameters);
    UniquePtr<IVFIndexInChunk> ivf_index(IVFIndexInChunk::GetNewIVFIndexInChunk(index_ivf.get(), column_def_.get()));
    return ivf_index;
}

IVF_Search_Params HnswLSGBuilder::MakeIVFSearchParams() const {
    const DataType *column_type = column_def_->type().get();
    auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());

    IVF_Search_Params ivf_search_params;
    ivf_search_params.knn_distance_ = knn_distance_.get();
    ivf_search_params.topk_ = lsg_config_.ls_k_;
    ivf_search_params.query_elem_type_ = embedding_info->Type();
    switch (index_hnsw_->metric_type_) {
        case MetricType::kMetricL2:
            ivf_search_params.knn_distance_type_ = KnnDistanceType::kL2;
            break;
        case MetricType::kMetricInnerProduct:
            ivf_search_params.knn_distance_type_ = KnnDistanceType::kInnerProduct;
            break;
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
    return ivf_search_params;
}

template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
UniquePtr<DataType[]> HnswLSGBuilder::GetAvgByIVF(Iter iter, SizeT row_count, const IVFIndexInChunk *ivf_index) {
    auto avg = MakeUnique<DataType[]>(row_count);

    IVF_Search_Params ivf_search_params = MakeIVFSearchParams();

    Bitmask bitmask;
    bool use_bitmask = false;
    while (true) {
        auto next_opt = iter.Next();
        if (!next_opt.has_value()) {
            break;
        }
        const auto &[data, offset] = next_opt.value();
        ivf_search_params.query_embedding_ = reinterpret_cast<const void *>(data);

        auto ivf_result_handler =
            GetIVFSearchHandler<LogicalType::kEmbedding, Compare, DistanceDataType>(ivf_search_params, use_bitmask, bitmask, row_count);
        ivf_result_handler->Begin();
        ivf_result_handler->Search(ivf_index);
        auto [result_n, d_ptr, offset_ptr] = ivf_result_handler->EndWithoutSort();

        avg[offset] = GetMean(d_ptr.get(), result_n);
    }
    return avg;
}

} // namespace infinity
