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

#include <ostream>

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

namespace infinity {

export template <typename DataType, typename LabelType>
class AbstractHnsw {
    using Hnsw1 = KnnHnsw<PlainIPVecStoreType<DataType>, LabelType>;
    using Hnsw2 = KnnHnsw<PlainL2VecStoreType<DataType>, LabelType>;
    using Hnsw3 = KnnHnsw<LVQIPVecStoreType<DataType, i8>, LabelType>;
    using Hnsw4 = KnnHnsw<LVQL2VecStoreType<DataType, i8>, LabelType>;

public:
    AbstractHnsw(void *ptr, const IndexHnsw *index_hnsw) {
        switch (index_hnsw->encode_type_) {
            case HnswEncodeType::kPlain: {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricInnerProduct: {
                        knn_hnsw_ptr_ = reinterpret_cast<Hnsw1 *>(ptr);
                        break;
                    }
                    case MetricType::kMetricL2: {
                        knn_hnsw_ptr_ = reinterpret_cast<Hnsw2 *>(ptr);
                        break;
                    }
                    default: {
                        UnrecoverableError("HNSW supports inner product and L2 distance.");
                    }
                }
                break;
            }
            case HnswEncodeType::kLVQ: {
                switch (index_hnsw->metric_type_) {
                    case MetricType::kMetricInnerProduct: {
                        knn_hnsw_ptr_ = reinterpret_cast<Hnsw3 *>(ptr);
                        break;
                    }
                    case MetricType::kMetricL2: {
                        knn_hnsw_ptr_ = reinterpret_cast<Hnsw4 *>(ptr);
                        break;
                    }
                    default: {
                        UnrecoverableError("HNSW supports inner product and L2 distance.");
                    }
                }
                break;
            }
            default: {
                UnrecoverableError("Invalid metric type");
            }
        }
    }

public:
    void Make(SizeT chunk_sizes, SizeT max_chunk_n, SizeT dimension, SizeT M, SizeT ef_c) {
        std::visit(
            [chunk_sizes, max_chunk_n, dimension, M, ef_c, this](auto &&arg) {
                using T = std::decay_t<decltype(*arg)>;
                knn_hnsw_ptr_ = new T(T::Make(chunk_sizes, max_chunk_n, dimension, M, ef_c));
            },
            knn_hnsw_ptr_);
    }

    void Load(FileHandler &file_handler) {
        std::visit(
            [&file_handler, this](auto &&arg) {
                using T = std::decay_t<decltype(*arg)>;
                knn_hnsw_ptr_ = new T(T::Load(file_handler));
            },
            knn_hnsw_ptr_);
    }

    void Save(FileHandler &file_handler) {
        std::visit([&file_handler](auto &&arg) { arg->Save(file_handler); }, knn_hnsw_ptr_);
    }

    void Free() {
        std::visit([](auto &&arg) { delete arg; }, knn_hnsw_ptr_);
    }

    SizeT GetVertexNum() const {
        return std::visit([](auto &&arg) { return arg->GetVertexNum(); }, knn_hnsw_ptr_);
    }

    void Build(SizeT idx) {
        std::visit([idx](auto &&arg) { arg->Build(idx); }, knn_hnsw_ptr_);
    }

    void *RawPtr() const {
        return std::visit([](auto &&arg) { return reinterpret_cast<void *>(arg); }, knn_hnsw_ptr_);
    }

    template <DataIteratorConcept<const DataType *, LabelType> Iterator>
    Pair<SizeT, SizeT> InsertVecs(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        return std::visit([&iter, &config](auto &&arg) { return arg->InsertVecs(std::move(iter), config); }, knn_hnsw_ptr_);
    }

    template <DataIteratorConcept<const DataType *, LabelType> Iterator>
    Pair<SizeT, SizeT> StoreData(Iterator &&iter, const HnswInsertConfig &config = kDefaultHnswInsertConfig) {
        return std::visit([&iter, &config](auto &&arg) { return arg->StoreData(std::move(iter), config); }, knn_hnsw_ptr_);
    }

    void Dump(std::ostream &os) const {
        std::visit([&os](auto &&arg) { arg->Dump(os); }, knn_hnsw_ptr_);
    }

    void SetEf(SizeT ef) {
        std::visit([ef](auto &&arg) { arg->SetEf(ef); }, knn_hnsw_ptr_);
    }

    template <FilterConcept<LabelType> Filter>
    Tuple<SizeT, UniquePtr<DataType[]>, UniquePtr<LabelType[]>>
    KnnSearch(const DataType *q, SizeT k, const Filter &filter, bool with_lock = true) const {
        return std::visit(
            [q, k, &filter, with_lock](auto &&arg) {
                if (with_lock) {
                    return arg->template KnnSearch<Filter, true>(q, k, filter);
                } else {
                    return arg->template KnnSearch<Filter, false>(q, k, filter);
                }
            },
            knn_hnsw_ptr_);
    }

    Tuple<SizeT, UniquePtr<DataType[]>, UniquePtr<LabelType[]>> KnnSearch(const DataType *q, SizeT k, bool with_lock = true) const {
        return std::visit(
            [q, k, with_lock](auto &&arg) {
                if (with_lock) {
                    return arg->template KnnSearch<true>(q, k);
                } else {
                    return arg->template KnnSearch<false>(q, k);
                }
            },
            knn_hnsw_ptr_);
    }

private:
    std::variant<Hnsw1 *, Hnsw2 *, Hnsw3 *, Hnsw4 *> knn_hnsw_ptr_;
};

} // namespace infinity