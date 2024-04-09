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
    void Make(SizeT max_element, SizeT dimension, SizeT M, SizeT ef_c) {
        std::visit(
            [max_element, dimension, M, ef_c, this](auto &&arg) {
                using T = std::decay_t<decltype(*arg)>;
                if constexpr (std::is_same_v<T, Hnsw1> || std::is_same_v<T, Hnsw2>) {
                    knn_hnsw_ptr_ = T::Make(max_element, dimension, M, ef_c).release();
                } else if constexpr (std::is_same_v<T, Hnsw3> || std::is_same_v<T, Hnsw4>) {
                    knn_hnsw_ptr_ = T::Make(max_element, dimension, M, ef_c).release();
                } else {
                    UnrecoverableError("Invalid type");
                }
            },
            knn_hnsw_ptr_);
    }

    void Load(FileHandler &file_handler) {
        std::visit(
            [&file_handler, this](auto &&arg) {
                using T = std::decay_t<decltype(*arg)>;
                if constexpr (std::is_same_v<T, Hnsw1> || std::is_same_v<T, Hnsw2>) {
                    knn_hnsw_ptr_ = T::Load(file_handler).release();
                } else if constexpr (std::is_same_v<T, Hnsw3> || std::is_same_v<T, Hnsw4>) {
                    knn_hnsw_ptr_ = T::Load(file_handler).release();
                } else {
                    UnrecoverableError("Invalid type");
                }
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
    void InsertVecs(Iterator &&iter) {
        std::visit([&iter](auto &&arg) { arg->InsertVecs(std::move(iter)); }, knn_hnsw_ptr_);
    }

    template <DataIteratorConcept<const DataType *, LabelType> Iterator>
    Pair<SizeT, SizeT> StoreData(Iterator &&iter) {
        return std::visit([&iter](auto &&arg) { return arg->StoreData(std::move(iter)); }, knn_hnsw_ptr_);
    }

    void SetEf(SizeT ef) {
        std::visit([ef](auto &&arg) { arg->SetEf(ef); }, knn_hnsw_ptr_);
    }

    template <FilterConcept<LabelType> Filter>
    Tuple<SizeT, UniquePtr<DataType[]>, UniquePtr<LabelType[]>> KnnSearch(const DataType *q, SizeT k, const Filter &filter) const {
        return std::visit([q, k, &filter](auto &&arg) { return arg->template KnnSearch<Filter>(q, k, filter); }, knn_hnsw_ptr_);
    }

    Tuple<SizeT, UniquePtr<DataType[]>, UniquePtr<LabelType[]>> KnnSearch(const DataType *q, SizeT k) const {
        return std::visit([q, k](auto &&arg) { return arg->KnnSearch(q, k); }, knn_hnsw_ptr_);
    }

private:
    std::variant<Hnsw1 *, Hnsw2 *, Hnsw3 *, Hnsw4 *> knn_hnsw_ptr_;
};

} // namespace infinity