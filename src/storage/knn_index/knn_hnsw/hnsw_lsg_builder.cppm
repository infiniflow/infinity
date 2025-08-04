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

export module infinity_core:hnsw_lsg_builder;

import :stl;
import column_def;
import :index_hnsw;
import :index_ivf;
import statement_common;
import :defer_op;
import :infinity_exception;
import :index_base;
import :ivf_index_data;
import :third_party;
import internal_types;
import :knn_scan_data;
import knn_expr;
import :ivf_index_search;
import data_type;
import :roaring_bitmap;
import :knn_result_handler;
import logical_type;
import embedding_info;
import :hnsw_common;
import :status;
import :merge_knn;
import :infinity_context;
import :logger;

namespace infinity {

// #define use_ivf

class IterIVFDataAccessor : public IVFDataAccessorBase {
public:
    IterIVFDataAccessor() {}

    template <typename Iter>
    SizeT InsertEmbedding(Iter iter, SizeT sample_num) {
        SizeT count = 0;
        while (count < sample_num) {
            if (auto ret = iter.Next(); ret) {
                auto &[embedding, offset] = *ret;
                embeddings_.emplace_back(reinterpret_cast<const_ptr_t>(embedding), offset);
                ++count;
            } else {
                break;
            }
        }
        return count;
    }

    const_ptr_t GetEmbedding(SizeT offset) override { return embeddings_[offset].first; }

    Pair<Span<const char>, SizeT> GetMultiVector(SizeT offset) override {
        UnrecoverableError("Not implemented");
        return {};
    }

    SizeT size() const { return embeddings_.size(); }

private:
    Vector<Pair<const_ptr_t, SizeT>> embeddings_;
};

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

export template <typename DataType, typename DistanceType>
class HnswLSGBuilder {
public:
    using This = HnswLSGBuilder<DataType, DistanceType>;

    HnswLSGBuilder(const IndexHnsw *index_hnsw, SharedPtr<ColumnDef> column_def) : index_hnsw_(index_hnsw), column_def_(column_def) {
        if (index_hnsw_->build_type_ != HnswBuildType::kLSG) {
            RecoverableError(Status::NotSupport("Only support LSG build type"));
        }
        const auto *column_type = column_def_->type().get();
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
    HnswLSGBuilder(This &&other)
        : index_hnsw_(other.index_hnsw_), column_def_(other.column_def_), knn_distance_(std::move(other.knn_distance_)),
          bf_sample_vecs_(std::move(other.bf_sample_vecs_)), bf_avg_(std::move(other.bf_avg_)), ivf_sample_iter_(std::move(other.ivf_sample_iter_)),
          ivf_avg_(std::move(other.ivf_avg_)) {}
    This &operator=(This &&other) {
        if (this != &other) {
            index_hnsw_ = other.index_hnsw_;
            column_def_ = other.column_def_;
            knn_distance_ = std::move(other.knn_distance_);
            bf_sample_vecs_ = std::move(other.bf_sample_vecs_);
            bf_avg_ = std::move(other.bf_avg_);
            ivf_sample_iter_ = std::move(other.ivf_sample_iter_);
            ivf_avg_ = std::move(other.ivf_avg_);
        }
        return *this;
    }

    ~HnswLSGBuilder() = default;

public:
    template <typename Iter>
    SizeT InsertSampleVec(Iter iter, SizeT sample_num = std::numeric_limits<SizeT>::max()) {
        float sample_ratio = index_hnsw_->lsg_config_->sample_ratio_;
        SizeT count = 0;
#ifdef use_ivf
        SampleIter<Iter> sample_iter(iter, sample_ratio);
        count = ivf_sample_iter_.InsertEmbedding(std::move(sample_iter), sample_num);
#else
        SizeT dim = static_cast<EmbeddingInfo *>(column_def_->type()->type_info().get())->Dimension();
        SampleIter<Iter> sample_iter(iter, sample_ratio);
        for (; count < sample_num; ++count) {
            auto next_opt = sample_iter.Next();
            if (!next_opt.has_value()) {
                break;
            }
            const auto &[embedding, offset] = next_opt.value();
            bf_sample_vecs_.insert(bf_sample_vecs_.end(), embedding, embedding + dim);
        }
#endif
        return count;
    }

    template <typename Iter>
    void InsertLSAvg(Iter iter, SizeT row_count) {
#ifdef use_ivf
        switch (index_hnsw_->metric_type_) {
            case MetricType::kMetricL2:
                return GetAvgByIVF<Iter, CompareMax>(std::move(iter), row_count);
            case MetricType::kMetricInnerProduct:
            case MetricType::kMetricCosine:
                return GetAvgByIVF<Iter, CompareMin>(std::move(iter), row_count);
            default:
                UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
        }
#else
        switch (index_hnsw_->metric_type_) {
            case MetricType::kMetricL2:
                return GetAvgBF<Iter, CompareMax>(std::move(iter), row_count);
            case MetricType::kMetricInnerProduct:
            case MetricType::kMetricCosine:
                return GetAvgBF<Iter, CompareMin>(std::move(iter), row_count);
            default:
                UnrecoverableError(fmt::format("Invalid metric type: {}", MetricTypeToString(index_hnsw_->metric_type_)));
        }
#endif
    }

    DistanceType *avg() {
#ifdef use_ivf
        return ivf_avg_.data();
#else
        return bf_avg_.data();
#endif
    }

    float alpha() { return index_hnsw_->lsg_config_->alpha_; }

private:
    DistanceType GetDistMean(const DistanceType *data, u32 size) {
        DistanceType sum = 0;
        for (u32 i = 0; i < size; ++i) {
            sum += data[i];
        }
        return sum / size;
    }

    UniquePtr<IVFIndexInChunk> MakeIVFIndex() {
        auto *embedding_info = static_cast<EmbeddingInfo *>(column_def_->type().get()->type_info().get());

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

    IVF_Search_Params MakeIVFSearchParams() const {
        auto *embedding_info = static_cast<EmbeddingInfo *>(column_def_->type().get()->type_info().get());
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

    template <typename Iter, template <typename, typename> typename Compare>
    void GetAvgByIVF(Iter iter, SizeT row_count) {
        auto ivf_index = MakeIVFIndex();
        ivf_index->BuildIVFIndex(RowID(0, 0), ivf_sample_iter_.size(), &ivf_sample_iter_, column_def_);

        ivf_avg_.resize(ivf_avg_.size() + row_count);

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
                GetIVFSearchHandler<LogicalType::kEmbedding, Compare, DistanceType>(ivf_search_params, use_bitmask, bitmask, row_count);
            ivf_result_handler->Begin();
            ivf_result_handler->Search(ivf_index.get());
            auto [result_n, d_ptr, offset_ptr] = ivf_result_handler->EndWithoutSort();

            ivf_avg_[offset] = GetDistMean(d_ptr.get(), result_n);
        }
    }

    template <typename Iter, template <typename, typename> typename Compare>
    void GetAvgBF(Iter iter, SizeT row_count) {
        const auto *embedding_info = static_cast<EmbeddingInfo *>(column_def_->type()->type_info().get());
        SizeT dim = embedding_info->Dimension();
        const LSGConfig &lsg_config = *index_hnsw_->lsg_config_;

        const SizeT sample_count = bf_sample_vecs_.size() / dim;
        const auto &sample_data = bf_sample_vecs_;

        bf_avg_.resize(bf_avg_.size() + row_count);
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
        KnnDistance1<DataType, DistanceType> dist_f(dist_type);
        SizeT ls_k = std::min(lsg_config.ls_k_, sample_count);
        if constexpr (SplitIter<Iter>) {
            Iter iter_copy = iter;
            auto iters = std::move(iter_copy).split();
            auto &thread_pool = InfinityContext::instance().GetLsgBuildThreadPool();
            Vector<std::future<void>> futs;
            for (auto &splited_iter : iters) {
                futs.emplace_back(thread_pool.push([&](int id) {
                    MergeKnn<DataType, Compare, DistanceType> merge_heap(1, ls_k, None);
                    while (true) {
                        auto next_opt = splited_iter.Next();
                        if (!next_opt.has_value()) {
                            break;
                        }
                        merge_heap.Begin();
                        const auto &[data, offset] = next_opt.value();
                        const auto *query = reinterpret_cast<const DataType *>(data);
                        merge_heap.Search(query, sample_data.data(), dim, dist_f.dist_func_, sample_count, 0 /*segment_id*/, 0 /*block_id*/);
                        merge_heap.End();

                        auto dist = merge_heap.GetDistances();
                        if (bf_avg_[offset] != 0) {
                            UnrecoverableError(fmt::format("Invalid avg value: {} at {}", bf_avg_[offset], offset));
                        }
                        bf_avg_[offset] = GetDistMean(dist, ls_k);
                    }
                }));
            }
            [[maybe_unused]] SizeT i = 0;
            for (auto &fut : futs) {
                fut.get();
                LOG_INFO(fmt::format("Future {} finished", i++));
            }

        } else {
            MergeKnn<DataType, Compare, DistanceType> merge_heap(1, ls_k, None);
            while (true) {
                auto next_opt = iter.Next();
                if (!next_opt.has_value()) {
                    break;
                }
                merge_heap.Begin();
                const auto &[data, offset] = next_opt.value();
                const auto *query = reinterpret_cast<const DataType *>(data);
                merge_heap.Search(query, sample_data.data(), dim, dist_f.dist_func_, sample_count, 0 /*segment_id*/, 0 /*block_id*/);
                merge_heap.End();

                auto dist = merge_heap.GetDistances();
                auto avg_v = GetDistMean(dist, ls_k);
                bf_avg_[offset] = avg_v;
            }
        }
    }

private:
    const IndexHnsw *index_hnsw_ = nullptr;
    SharedPtr<ColumnDef> column_def_ = nullptr;

    UniquePtr<KnnDistanceBase1> knn_distance_;

    Vector<DataType> bf_sample_vecs_;
    Vector<DistanceType> bf_avg_;
    IterIVFDataAccessor ivf_sample_iter_;
    Vector<DistanceType> ivf_avg_;
};

} // namespace infinity
