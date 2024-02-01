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

module hnsw_file_worker;

import infinity_exception;
import stl;
import index_file_worker;
import hnsw_alg;
import index_hnsw;
import parser;
import index_base;
import dist_func_l2;
import dist_func_ip;
import lvq_store;
import plain_store;
import third_party;
import logger;
import logical_type;

namespace infinity {
HnswFileWorker::~HnswFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void HnswFileWorker::AllocateInMemory() {
    if (data_) {
        UnrecoverableError("Data is already allocated.");
    }
    if (index_base_->index_type_ != IndexType::kHnsw) {
        UnrecoverableError("Index type isn't HNSW");
    }

    auto data_type = column_def_->type();
    if (data_type->type() != LogicalType::kEmbedding) {
        UnrecoverableError("Index should be created on embedding column now.");
    }

    SizeT dimension = GetDimension();
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_);
    SizeT M = index_hnsw->M_;
    SizeT ef_c = index_hnsw->ef_construction_;
    auto AllocateData = [&](auto *hnsw_index) { data_ = static_cast<void *>(hnsw_index); };

    switch (GetType()) {
        case kElemFloat: {
            switch (index_hnsw->encode_type_) {
                case HnswEncodeType::kPlain: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainIPDist<f32>>;
                            AllocateData(Hnsw::Make(max_element_, dimension, M, ef_c, {}).release());
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainL2Dist<f32>>;
                            AllocateData(Hnsw::Make(max_element_, dimension, M, ef_c, {}).release());
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type");
                        }
                    }
                    break;
                }
                case HnswEncodeType::kLVQ: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQIPCache<f32, i8>>, LVQIPDist<f32, i8>>;
                            AllocateData(Hnsw::Make(max_element_, dimension, M, ef_c, {}).release());
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
                            AllocateData(Hnsw::Make(max_element_, dimension, M, ef_c, {}).release());
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type.");
                        }
                    }
                    break;
                }
                default: {
                    UnrecoverableError("Invalid metric type");
                }
            }
            break;
        }
        default: {
            UnrecoverableError("Index should be created on float embedding column now.");
        }
    }
}

void HnswFileWorker::FreeInMemory() {
    if (!data_) {
        UnrecoverableError("FreeInMemory: Data is not allocated.");
    }
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_);
    auto FreeData = [&](auto *hnsw_index) { delete hnsw_index; };
    EmbeddingDataType embedding_type = GetType();
    switch (embedding_type) {
        case kElemFloat: {
            switch (index_hnsw->encode_type_) {
                case HnswEncodeType::kPlain: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainIPDist<f32>>;
                            FreeData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainL2Dist<f32>>;
                            FreeData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type");
                        }
                    }
                    break;
                }
                case HnswEncodeType::kLVQ: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQIPCache<f32, i8>>, LVQIPDist<f32, i8>>;
                            FreeData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
                            FreeData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type");
                        }
                    }
                    break;
                }
                default: {
                    UnrecoverableError("Invalid metric type");
                }
            }
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Index should be created on float embedding column now, type: {}",
                                                EmbeddingType::EmbeddingDataType2String(embedding_type)));
        }
    }
    data_ = nullptr;
}

void HnswFileWorker::WriteToFileImpl(bool &prepare_success) {
    if (!data_) {
        UnrecoverableError("WriteToFileImpl: Data is not allocated.");
    }
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_);
    auto SaveData = [&](auto *hnsw_index) { hnsw_index->Save(*file_handler_); };
    switch (GetType()) {
        case kElemFloat: {
            switch (index_hnsw->encode_type_) {
                case HnswEncodeType::kPlain: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainIPDist<f32>>;
                            SaveData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainL2Dist<f32>>;
                            SaveData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type");
                        }
                    }
                    break;
                }
                case HnswEncodeType::kLVQ: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQIPCache<f32, i8>>, LVQIPDist<f32, i8>>;
                            SaveData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
                            SaveData(static_cast<Hnsw *>(data_));
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type");
                        }
                    }
                    break;
                }
                default: {
                    UnrecoverableError("Invalid metric type");
                }
            }
            break;
        }
        default: {
            UnrecoverableError("Index should be created on float embedding column now.");
        }
    }
    prepare_success = true;
}

void HnswFileWorker::ReadFromFileImpl() {
    // TODO!! not save index parameter in index file.
    const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base_);
    auto LoadData = [&](auto *hnsw_index) { data_ = static_cast<void *>(hnsw_index); };
    switch (GetType()) {
        case kElemFloat: {
            switch (index_hnsw->encode_type_) {
                case HnswEncodeType::kPlain: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainIPDist<f32>>;
                            LoadData(Hnsw::Load(*file_handler_, {}).release());
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, PlainStore<f32>, PlainL2Dist<f32>>;
                            LoadData(Hnsw::Load(*file_handler_, {}).release());
                            break;
                        }
                        default: {
                            UnrecoverableError("Metric type");
                        }
                    }
                    break;
                }
                case HnswEncodeType::kLVQ: {
                    switch (index_hnsw->metric_type_) {
                        case MetricType::kMerticInnerProduct: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQIPCache<f32, i8>>, LVQIPDist<f32, i8>>;
                            LoadData(Hnsw::Load(*file_handler_, {}).release());
                            break;
                        }
                        case MetricType::kMerticL2: {
                            using Hnsw = KnnHnsw<f32, SegmentOffset, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
                            LoadData(Hnsw::Load(*file_handler_, {}).release());
                            break;
                        }
                        default: {
                            UnrecoverableError("Invalid metric type");
                        }
                    }
                    break;
                }
                default: {
                    UnrecoverableError("Invalid metric type");
                }
            }
            break;
        }
        default: {
            UnrecoverableError("Index should be created on float embedding column now.");
        }
    }
}

EmbeddingDataType HnswFileWorker::GetType() const {
    auto data_type = column_def_->type();
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    return embedding_info->Type();
}

SizeT HnswFileWorker::GetDimension() const {
    auto data_type = column_def_->type();
    auto type_info = data_type->type_info().get();
    auto embedding_info = (EmbeddingInfo *)type_info;
    return embedding_info->Dimension();
}
} // namespace infinity