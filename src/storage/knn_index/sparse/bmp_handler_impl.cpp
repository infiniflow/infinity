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

module infinity_core:bmp_handler.impl;

import :bmp_handler;

import :block_column_iter;
import :sparse_util;
import :logger;
import :local_file_handle;
import :chunk_index_meta;
import :bmp_alg;

import sparse_info;
import embedding_type;
import column_def;

namespace infinity {

template <typename DataType, typename IndexType, BMPOwnMem OwnMem>
static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp) {
    switch (index_bmp->compress_type_) {
        case BMPCompressType::kCompressed: {
            using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kCompressed, OwnMem>;
            return std::unique_ptr<BMPIndex>();
        }
        case BMPCompressType::kRaw: {
            using BMPIndex = BMPAlg<DataType, IndexType, BMPCompressType::kRaw, OwnMem>;
            return std::unique_ptr<BMPIndex>();
        }
        default: {
            return nullptr;
        }
    }
}

template <typename DataType, BMPOwnMem OwnMem>
static AbstractBMP InitAbstractIndex(const IndexBMP *index_bmp, const SparseInfo *sparse_info) {
    switch (sparse_info->IndexType()) {
        case EmbeddingDataType::kElemInt8: {
            return InitAbstractIndex<DataType, i8, OwnMem>(index_bmp);
        }
        case EmbeddingDataType::kElemInt16: {
            return InitAbstractIndex<DataType, i16, OwnMem>(index_bmp);
        }
        case EmbeddingDataType::kElemInt32: {
            return InitAbstractIndex<DataType, i32, OwnMem>(index_bmp);
        }
        default: {
            return nullptr;
        }
    }
}

template <BMPOwnMem OwnMem>
static AbstractBMP InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def) {
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
    const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());

    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemFloat: {
            return InitAbstractIndex<f32, OwnMem>(index_bmp, sparse_info);
        }
        case EmbeddingDataType::kElemDouble: {
            return InitAbstractIndex<f64, OwnMem>(index_bmp, sparse_info);
        }
        default: {
            return nullptr;
        }
    }
}

AbstractBMP BMPHandler::InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem) {
    if (own_mem) {
        return InitAbstractIndex<BMPOwnMem::kTrue>(index_base, column_def);
    } else {
        return InitAbstractIndex<BMPOwnMem::kFalse>(index_base, column_def);
    }
}

BMPHandler::BMPHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem)
    : bmp_(InitAbstractIndex(index_base, column_def, own_mem)) {
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
    const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());
    if (!own_mem) {
        return;
    }
    size_t term_num = sparse_info->Dimension();
    size_t block_size = index_bmp->block_size_;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = std::make_unique<IndexT>(term_num, block_size);
                } else {
                    UnrecoverableError("BMPHandler::BMPHandler: index does not own memory");
                }
            }
        },
        bmp_);
}

std::unique_ptr<BMPHandler> BMPHandler::Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem) {
    return std::make_unique<BMPHandler>(index_base, column_def, own_mem);
}

size_t BMPHandler::AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count) {
    size_t mem_usage{};
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;
                    size_t mem_before = index->MemoryUsage();
                    MemIndexInserterIter1<SparseRefT> iter(block_offset, col, offset, row_count);
                    index->AddDocs(std::move(iter));
                    size_t mem_after = index->MemoryUsage();
                    mem_usage = mem_after - mem_before;
                    LOG_INFO(fmt::format("before : {} -> after : {}, add mem_used : {}", mem_before, mem_after, mem_after - mem_before));
                } else {
                    UnrecoverableError("BMPHandler::AddDocs: index does not own memory");
                }
            }
        },
        bmp_);
    return mem_usage;
}

size_t BMPHandler::MemUsage() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return size_t(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->MemoryUsage();
                } else {
                    UnrecoverableError("BMPHandler::MemUsage: index does not own memory");
                    return size_t(0);
                }
            }
        },
        bmp_);
}

size_t BMPHandler::DocNum() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return size_t(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->DocNum();
                } else {
                    UnrecoverableError("BMPHandler::DocNum: index does not own memory");
                    return size_t(0);
                }
            }
        },
        bmp_);
}

size_t BMPHandler::GetSizeInBytes() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return size_t(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->GetSizeInBytes();
                } else {
                    UnrecoverableError("BMPHandler::GetSizeInBytes: index does not own memory");
                    return size_t(0);
                }
            }
        },
        bmp_);
}

void BMPHandler::Save(LocalFileHandle &file_handle) const {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->Save(file_handle);
                } else {
                    UnrecoverableError("BMPHandler::Save: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::SaveToPtr(LocalFileHandle &file_handle) const {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->SaveToPtr(file_handle);
                } else {
                    UnrecoverableError("BMPHandler::SaveToPtr: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::Load(LocalFileHandle &file_handle) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = IndexT::Load(file_handle);
                }
            }
        },
        bmp_);
}

void BMPHandler::LoadFromPtr(LocalFileHandle &file_handle, size_t file_size) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = IndexT::LoadFromPtr(file_handle, file_size);
                } else {
                    UnrecoverableError("BMPHandler::LoadFromPtr: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::LoadFromPtr(const char *ptr, size_t file_size) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    UnrecoverableError("BMPHandler::LoadFromPtr: index own memory");
                } else {
                    index = IndexT::LoadFromPtr(ptr, file_size);
                }
            }
        },
        bmp_);
}

void BMPHandler::Optimize(const BMPOptimizeOptions &options) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->Optimize(options);
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        bmp_);
}

BMPIndexInMem::~BMPIndexInMem() {
    if (!own_memory_) {
        return;
    }
    size_t mem_used = bmp_handler_->MemUsage();
    if (bmp_handler_ != nullptr) {
        delete bmp_handler_;
    }
    DecreaseMemoryUsageBase(mem_used);
}

void BMPIndexInMem::AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count) {
    size_t mem_used = bmp_handler_->AddDocs(block_offset, col, offset, row_count);
    IncreaseMemoryUsageBase(mem_used);
}

void BMPIndexInMem::Dump(FileWorker *index_file_worker, size_t *dump_size_ptr) {
    if (!own_memory_) {
        UnrecoverableError("BMPIndexInMem::Dump() called with own_memory_ = false.");
    }
    if (dump_size_ptr) {
        *dump_size_ptr = bmp_handler_->MemUsage();
    }

    std::shared_ptr<BMPHandlerPtr> data_ptr;
    index_file_worker->Read(data_ptr);
    *data_ptr = bmp_handler_;
    own_memory_ = false;
    index_file_worker_ = std::move(index_file_worker);
    index_file_worker_->Write(std::span{data_ptr.get(), 1});
}

size_t BMPIndexInMem::GetRowCount() const { return bmp_handler_->DocNum(); }

size_t BMPIndexInMem::GetSizeInBytes() const { return bmp_handler_->GetSizeInBytes(); }

MemIndexTracerInfo BMPIndexInMem::GetInfo() const {
    size_t mem_used = bmp_handler_->MemUsage();
    size_t row_cnt = bmp_handler_->DocNum();
    return MemIndexTracerInfo(std::make_shared<std::string>(index_name_),
                              std::make_shared<std::string>(table_name_),
                              std::make_shared<std::string>(db_name_),
                              mem_used,
                              row_cnt);
}

const ChunkIndexMetaInfo BMPIndexInMem::GetChunkIndexMetaInfo() const {
    return ChunkIndexMetaInfo{"", begin_row_id_, GetRowCount(), 0, GetSizeInBytes()};
}

} // namespace infinity
