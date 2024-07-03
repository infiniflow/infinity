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

module bmp_index_file_worker;

import index_bmp;
import logger;
import infinity_exception;
import third_party;
import internal_types;

namespace infinity {

BMPIndexFileWorker::~BMPIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

template <typename DataType, typename IndexType>
AbstractBMP GetAbstractIndex(const IndexBMP *index_bmp, const SparseInfo *sparse_info, void *data) {
    switch (index_bmp->compress_type_) {
        case BMPCompressType::kCompressed: {
            using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kCompressed>;
            return reinterpret_cast<BMPIndex *>(data);
        }
        case BMPCompressType::kRaw: {
            using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kRaw>;
            return reinterpret_cast<BMPIndex *>(data);
        }
        default: {
            return nullptr;
        }
    }
}

template <typename DataType>
AbstractBMP GetAbstractIndex(const IndexBMP *index_bmp, const SparseInfo *sparse_info, void *data) {
    switch (sparse_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            return GetAbstractIndex<DataType, i8>(index_bmp, sparse_info, data);
        }
        case EmbeddingDataType::kElemInt16: {
            return GetAbstractIndex<DataType, i16>(index_bmp, sparse_info, data);
        }
        case EmbeddingDataType::kElemInt32: {
            return GetAbstractIndex<DataType, i32>(index_bmp, sparse_info, data);
        }
        default: {
            return nullptr;
        }
    }
}

AbstractBMP BMPIndexFileWorker::GetAbstractIndex() {
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base_.get());
    const auto *sparse_info = GetSparseInfo();
    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemFloat: {
            return GetAbstractIndex<f32>(index_bmp, sparse_info, data_);
        }
        case EmbeddingDataType::kElemDouble: {
            return GetAbstractIndex<f64>(index_bmp, sparse_info, data_);
        }
        default: {
            return nullptr;
        }
    }
}

ConstAbstractBMP BMPIndexFileWorker::GetConstAbstractIndex() const {
    auto index = const_cast<BMPIndexFileWorker *>(this)->GetAbstractIndex();
    ConstAbstractBMP res = nullptr;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                res = index;
            }
        },
        index);
    return res;
}

void BMPIndexFileWorker::AllocateInMemory() {
    if (data_) {
        const auto error_message = "Data is already allocated.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    auto index = GetAbstractIndex();

    const auto *sparse_info = GetSparseInfo();
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base_.get());

    SizeT term_num = sparse_info->Dimension();
    SizeT block_size = index_bmp->block_size_;

    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                auto *index_ptr = new IndexT(term_num, block_size);
                data_ = reinterpret_cast<void *>(index_ptr);
            }
        },
        index);
}

void BMPIndexFileWorker::FreeInMemory() {
    if (!data_) {
        const auto error_message = "Data is not allocated.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    auto index = GetAbstractIndex();
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                delete index;
            }
        },
        index);
    data_ = nullptr;
}

void BMPIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (!data_) {
        UnrecoverableError("Data is not allocated.");
    }
    auto index = GetAbstractIndex();
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                index->Save(*file_handler_);
            }
        },
        index);
    prepare_success = true;
}

void BMPIndexFileWorker::ReadFromFileImpl() {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    auto index = GetAbstractIndex();
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                data_ = reinterpret_cast<void *>(new IndexT(IndexT::Load(*file_handler_)));
            }
        },
        index);
}

const SparseInfo *BMPIndexFileWorker::GetSparseInfo() const { return static_cast<SparseInfo *>(column_def_->type()->type_info().get()); }

} // namespace infinity
