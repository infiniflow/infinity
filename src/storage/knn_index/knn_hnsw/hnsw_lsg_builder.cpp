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

#include <future>
#include <vector>

module hnsw_lsg_builder;

import stl;
#ifdef INDEX_HANDLER
import hnsw_handler;
#else
import abstract_hnsw;
#endif
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
import knn_scan_data;
import knn_expr;
import ivf_index_search;
import data_type;
import roaring_bitmap;
import knn_result_handler;
import logical_type;
import embedding_info;
import hnsw_common;
import status;
import merge_knn;
import infinity_context;
import logger;
import kv_store;

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
    void InitEmbedding(Iter iter) {
        while (true) {
            if (auto ret = iter.Next(); ret) {
                auto &[embedding, offset] = *ret;
                embeddings_.emplace_back(reinterpret_cast<const_ptr_t>(embedding), offset);
            } else {
                break;
            }
        }
    }

    const_ptr_t GetEmbedding(SizeT offset, KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) override {
        return embeddings_[offset].first;
    }

    Pair<Span<const char>, SizeT> GetMultiVector(SizeT offset, KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) override {
        UnrecoverableError("Not implemented");
        return {};
    }

    SizeT size() const { return embeddings_.size(); }

private:
    Vector<Pair<const_ptr_t, SizeT>> embeddings_;
};

} // namespace

HnswLSGBuilder::HnswLSGBuilder(const IndexHnsw *index_hnsw, SharedPtr<ColumnDef> column_def) : index_hnsw_(index_hnsw), column_def_(column_def) {
    if (index_hnsw_->build_type_ != HnswBuildType::kLSG) {
        RecoverableError(Status::NotSupport("Only support LSG build type"));
    }
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
        case MetricType::kMetricCosine:
            distance_type = KnnDistanceType::kCosine;
            break;
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
    knn_distance_ = KnnDistanceBase1::Make(embedding_info->Type(), distance_type);
}

HnswLSGBuilder::~HnswLSGBuilder() = default;

template <typename Iter, typename DataType, typename DistanceDataType>
UniquePtr<HnswIndexInMem> HnswLSGBuilder::MakeImplIter(Iter iter, SizeT row_count, const RowID &base_row_id, bool trace) {
    Iter iter_copy = iter;
    auto avg = GetLSAvg<Iter, DataType, DistanceDataType>(std::move(iter_copy), row_count, base_row_id);
    auto hnsw_index = HnswIndexInMem::Make(index_hnsw_, column_def_.get(), trace);
    const LSGConfig &lsg_config = *index_hnsw_->lsg_config_;
    float alpha = lsg_config.alpha_;
    hnsw_index->SetLSGParam(alpha, std::move(avg));
    hnsw_index->InsertVecs(std::move(iter), kDefaultHnswInsertConfig, false);
    return hnsw_index;
}

template <typename Iter>
class SampleIter {
public:
    using ValueType = typename Iter::ValueType;

    SampleIter(Iter iter, float ratio) : iter_(std::move(iter)), ratio_(ratio) {}

    auto Next() {
        using RtnType = decltype(iter_.Next());
        if (ratio_ >= 1.0) {
            return iter_.Next();
        }
        if (ratio_ <= 0.0) {
            return RtnType{};
        }
        while (true) {
            auto next_opt = iter_.Next();
            if (!next_opt.has_value()) {
                return RtnType{};
            }
            float r = rand() % 1000 / 1000.0;
            if (r <= ratio_) {
                return next_opt;
            }
        }
    }

private:
    Iter iter_;
    float ratio_;
};

// #define use_ivf

template <typename Iter, typename DataType, typename DistanceDataType>
UniquePtr<float[]> HnswLSGBuilder::GetLSAvg(Iter iter, SizeT row_count, const RowID &base_row_id) {

#ifdef use_ivf
    switch (index_hnsw_->metric_type_) {
        case MetricType::kMetricL2:
            return GetAvgByIVF<Iter, DataType, CompareMax, DistanceDataType>(std::move(iter), row_count);
        case MetricType::kMetricInnerProduct:
        case MetricType::kMetricCosine:
            return GetAvgByIVF<Iter, DataType, CompareMin, DistanceDataType>(std::move(iter), row_count);
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
#else
    switch (index_hnsw_->metric_type_) {
        case MetricType::kMetricL2:
            return GetAvgBF<Iter, DataType, CompareMax, DistanceDataType>(std::move(iter), row_count);
        case MetricType::kMetricInnerProduct:
        case MetricType::kMetricCosine:
            return GetAvgBF<Iter, DataType, CompareMin, DistanceDataType>(std::move(iter), row_count);
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
#endif
    return nullptr;
}

UniquePtr<IVFIndexInChunk> HnswLSGBuilder::MakeIVFIndex() {
    const DataType *column_type = column_def_->type().get();
    auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());

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
    // {
    //     String sample_ratio_ = std::to_string(lsg_config_.sample_ratio_);
    //     parameters.push_back(new InitParameter("centroids_num_ratio", std::move(sample_ratio_)));
    // }
    parameters.push_back(new InitParameter("storage_type", "plain"));
    {
        String data_type = EmbeddingT::EmbeddingDataType2String(embedding_info->Type());
        parameters.push_back(new InitParameter("plain_storage_data_type", std::move(data_type)));
    }

    SharedPtr<IndexIVF> index_ivf = IndexIVF::Make(index_name, nullptr, filename, std::move(column_names), parameters);
    UniquePtr<IVFIndexInChunk> ivf_index(IVFIndexInChunk::GetNewIVFIndexInChunk(index_ivf.get(), column_def_.get()));
    return ivf_index;
}

IVF_Search_Params HnswLSGBuilder::MakeIVFSearchParams() const {
    const DataType *column_type = column_def_->type().get();
    auto *embedding_info = static_cast<EmbeddingInfo *>(column_type->type_info().get());
    const LSGConfig &lsg_config = *index_hnsw_->lsg_config_;

    IVF_Search_Params ivf_search_params;
    ivf_search_params.knn_distance_ = knn_distance_.get();
    ivf_search_params.topk_ = lsg_config.ls_k_;
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
UniquePtr<float[]> HnswLSGBuilder::GetAvgByIVF(Iter iter, SizeT row_count, KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    auto ivf_index = MakeIVFIndex();
    const LSGConfig &lsg_config = *index_hnsw_->lsg_config_;

    IterIVFDataAccessor iter_accessor;
    SampleIter<Iter> sample_iter(iter, lsg_config.sample_ratio_);
    iter_accessor.InitEmbedding(std::move(sample_iter));
    SizeT sample_count = iter_accessor.size();
    RowID base_row_id(0, 0);
    ivf_index->BuildIVFIndex(base_row_id, sample_count, &iter_accessor, column_def_, kv_instance, begin_ts, commit_ts);

    auto avg = MakeUnique<float[]>(row_count);

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

template <typename Iter, typename DataType, template <typename, typename> typename Compare, typename DistanceDataType>
UniquePtr<float[]> HnswLSGBuilder::GetAvgBF(Iter iter, SizeT row_count) {
    const auto *embedding_info = static_cast<EmbeddingInfo *>(column_def_->type()->type_info().get());
    SizeT dim = embedding_info->Dimension();
    const LSGConfig &lsg_config = *index_hnsw_->lsg_config_;

    SampleIter<Iter> sample_iter(iter, lsg_config.sample_ratio_);
    SizeT sample_count = row_count * lsg_config.sample_ratio_;
    auto sample_data = MakeUnique<DataType[]>(sample_count * dim);
    {
        SizeT i;
        for (i = 0; i < sample_count;) {
            auto next_opt = sample_iter.Next();
            if (!next_opt.has_value()) {
                break;
            }
            const auto &[embedding, offset] = next_opt.value();
            std::memcpy(sample_data.get() + i * dim, embedding, dim * sizeof(DataType));
            ++i;
        }
        sample_count = i;
    }

    auto avg = MakeUnique<float[]>(row_count);
    KnnDistanceType dist_type = KnnDistanceType::kInvalid;
    switch (index_hnsw_->metric_type_) {
        case MetricType::kMetricL2:
            dist_type = KnnDistanceType::kL2;
            break;
        case MetricType::kMetricInnerProduct:
            dist_type = KnnDistanceType::kInnerProduct;
            break;
        case MetricType::kMetricCosine:
            dist_type = KnnDistanceType::kCosine;
            break;
        default:
            UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
    }
    KnnDistance1<DataType, DistanceDataType> dist_f(dist_type);
    SizeT ls_k = std::min(lsg_config.ls_k_, sample_count);
    if constexpr (SplitIter<Iter>) {
        Iter iter_copy = iter;
        auto iters = std::move(iter_copy).split();
        auto &thread_pool = InfinityContext::instance().GetHnswBuildThreadPool();
        Vector<std::future<void>> futs;
        for (auto &splited_iter : iters) {
            futs.emplace_back(thread_pool.push([&](int id) {
                MergeKnn<DataType, Compare, DistanceDataType> merge_heap(1, ls_k, None);
                while (true) {
                    auto next_opt = splited_iter.Next();
                    if (!next_opt.has_value()) {
                        break;
                    }
                    merge_heap.Begin();
                    const auto &[data, offset] = next_opt.value();
                    const auto *query = reinterpret_cast<const DataType *>(data);
                    merge_heap.Search(query, sample_data.get(), dim, dist_f.dist_func_, sample_count, 0 /*segment_id*/, 0 /*block_id*/);
                    merge_heap.End();

                    auto dist = merge_heap.GetDistances();
                    if (avg[offset] != 0) {
                        UnrecoverableError(fmt::format("Invalid avg value: {} at {}", avg[offset], offset));
                    }
                    avg[offset] = GetMean(dist, ls_k);
                }
            }));
        }
        [[maybe_unused]] SizeT i = 0;
        for (auto &fut : futs) {
            fut.get();
            LOG_INFO(fmt::format("Future {} finished", i++));
        }

    } else {
        MergeKnn<DataType, Compare, DistanceDataType> merge_heap(1, ls_k, None);
        while (true) {
            auto next_opt = iter.Next();
            if (!next_opt.has_value()) {
                break;
            }
            merge_heap.Begin();
            const auto &[data, offset] = next_opt.value();
            const auto *query = reinterpret_cast<const DataType *>(data);
            merge_heap.Search(query, sample_data.get(), dim, dist_f.dist_func_, sample_count, 0 /*segment_id*/, 0 /*block_id*/);
            merge_heap.End();

            auto dist = merge_heap.GetDistances();
            auto avg_v = GetMean(dist, ls_k);
            avg[offset] = avg_v;
        }
    }

    return avg;
}

template UniquePtr<HnswIndexInMem> HnswLSGBuilder::MakeImplIter<DenseVectorIter<f32, u32>, f32, f32>(DenseVectorIter<f32, u32> iter,
                                                                                                     SizeT row_count,
                                                                                                     const RowID &base_row_id,
                                                                                                     bool trace);

} // namespace infinity
