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

module infinity_core;

import :buffer_manager;
import :buffer_handle;
import :block_column_iter;
import :sparse_util;
import :infinity_exception;
import :third_party;
import :logger;
import :buffer_obj;
import sparse_info;
import :bmp_handler;
import :base_memindex;

namespace infinity {

// MemIndexTracerInfo BMPIndexInMem::GetInfo() const {
//     auto [mem_used, row_cnt] = std::visit(
//         [](auto &&index) -> Pair<SizeT, SizeT> {
//             using T = std::decay_t<decltype(index)>;
//             if constexpr (std::is_same_v<T, std::nullptr_t>) {
//                 return {};
//             } else {
//                 using IndexT = typename std::remove_pointer_t<T>;
//                 if constexpr (IndexT::kOwnMem) {
//                     return {index->MemoryUsage(), index->DocNum()};
//                 } else {
//                     UnrecoverableError("BMPIndexInMem::GetInfo: index does not own memory");
//                     return {};
//                 }
//             }
//         },
//         bmp_);
//
//     return MemIndexTracerInfo(MakeShared<String>(index_name_), MakeShared<String>(table_name_), MakeShared<String>(db_name_), mem_used, row_cnt);
// }
//
// const ChunkIndexMetaInfo BMPIndexInMem::GetChunkIndexMetaInfo() const {
//     return ChunkIndexMetaInfo{"", begin_row_id_, GetRowCount(), GetSizeInBytes()};
// }
//
// BMPIndexInMem::BMPIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def)
//     : begin_row_id_(begin_row_id), bmp_(InitAbstractIndex(index_base, column_def)) {
//     const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
//     const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());
//     SizeT term_num = sparse_info->Dimension();
//     SizeT block_size = index_bmp->block_size_;
//     std::visit(
//         [&](auto &&index) {
//             using T = std::decay_t<decltype(index)>;
//             if constexpr (!std::is_same_v<T, std::nullptr_t>) {
//                 using IndexT = std::decay_t<decltype(*index)>;
//                 if constexpr (IndexT::kOwnMem) {
//                     bmp_ = new IndexT(term_num, block_size);
//                 } else {
//                     UnrecoverableError("BMPIndexInMem::BMPIndexInMem: index does not own memory");
//                 }
//             }
//         },
//         bmp_);
//     own_memory_ = true;
// }
//
// BMPIndexInMem::~BMPIndexInMem() {
//     if (!own_memory_) {
//         return;
//     }
//     std::visit(
//         [&](auto &&index) {
//             using T = std::decay_t<decltype(index)>;
//             if constexpr (std::is_same_v<T, std::nullptr_t>) {
//                 return;
//             } else {
//                 using IndexT = std::decay_t<decltype(*index)>;
//                 if constexpr (IndexT::kOwnMem) {
//                     SizeT mem_used = index->MemoryUsage();
//                     if (index != nullptr) {
//                         delete index;
//                     }
//                     DecreaseMemoryUsageBase(mem_used);
//                 } else {
//                     UnrecoverableError("BMPIndexInMem::~BMPIndexInMem: index does not own memory");
//                 }
//             }
//         },
//         bmp_);
// }
//
// SizeT BMPIndexInMem::GetRowCount() const {
//     return std::visit(
//         [](auto &&index) {
//             using T = std::decay_t<decltype(index)>;
//             if constexpr (std::is_same_v<T, std::nullptr_t>) {
//                 return SizeT(0);
//             } else {
//                 using IndexT = std::decay_t<decltype(*index)>;
//                 if constexpr (IndexT::kOwnMem) {
//                     return index->DocNum();
//                 } else {
//                     UnrecoverableError("BMPIndexInMem::GetRowCount: index does not own memory");
//                     return SizeT(0);
//                 }
//             }
//         },
//         bmp_);
// }
//
// SizeT BMPIndexInMem::GetSizeInBytes() const {
//     return std::visit(
//         [](auto &&index) {
//             using T = std::decay_t<decltype(index)>;
//             if constexpr (std::is_same_v<T, std::nullptr_t>) {
//                 return SizeT(0);
//             } else {
//                 using IndexT = std::decay_t<decltype(*index)>;
//                 if constexpr (IndexT::kOwnMem) {
//                     return index->GetSizeInBytes();
//                 } else {
//                     UnrecoverableError("BMPIndexInMem::GetSizeInBytes: index does not own memory");
//                     return SizeT(0);
//                 }
//             }
//         },
//         bmp_);
// }
//
// void BMPIndexInMem::AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count) {
//     std::visit(
//         [&](auto &&index) {
//             using T = std::decay_t<decltype(index)>;
//             if constexpr (std::is_same_v<T, std::nullptr_t>) {
//                 return;
//             } else {
//                 using IndexT = std::decay_t<decltype(*index)>;
//                 if constexpr (IndexT::kOwnMem) {
//                     using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;
//                     SizeT mem_before = index->MemoryUsage();
//                     MemIndexInserterIter1<SparseRefT> iter(block_offset, col, offset, row_count);
//                     index->AddDocs(std::move(iter));
//                     SizeT mem_after = index->MemoryUsage();
//                     IncreaseMemoryUsageBase(mem_after - mem_before);
//                     LOG_INFO(fmt::format("before : {} -> after : {}, add mem_used : {}", mem_before, mem_after, mem_after - mem_before));
//                 } else {
//                     UnrecoverableError("BMPIndexInMem::AddDocs: index does not own memory");
//                 }
//             }
//         },
//         bmp_);
// }
//
// void BMPIndexInMem::Dump(BufferObj *buffer_obj, SizeT *dump_size_ptr) {
//     if (!own_memory_) {
//         UnrecoverableError("BMPIndexInMem::Dump() called with own_memory_ = false.");
//     }
//     if (dump_size_ptr) {
//         SizeT dump_size = 0;
//         std::visit(
//             [&](auto &&index) {
//                 using IndexType = std::decay_t<decltype(index)>;
//                 if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
//                     return;
//                 } else {
//                     using IndexT = std::decay_t<decltype(*index)>;
//                     if constexpr (IndexT::kOwnMem) {
//                         dump_size = index->MemoryUsage();
//                     } else {
//                         UnrecoverableError("BMPIndexInMem::Dump: index does not own memory");
//                     }
//                 }
//             },
//             bmp_);
//         *dump_size_ptr = dump_size;
//     }
//
//     BufferHandle handle = buffer_obj->Load();
//     auto *data_ptr = static_cast<AbstractBMP *>(handle.GetDataMut());
//     *data_ptr = bmp_;
//     own_memory_ = false;
//     chunk_handle_ = std::move(handle);
// }

} // namespace infinity
