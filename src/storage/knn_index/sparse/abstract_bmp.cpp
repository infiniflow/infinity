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

module abstract_bmp;

import segment_index_entry;
import chunk_index_entry;
import buffer_manager;
import buffer_handle;
import logger;

namespace infinity {

SharedPtr<ChunkIndexEntry> BMPIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) {
    SizeT row_count = 0;
    SizeT index_size = 0;
    std::visit(
        [&](auto &&index) {
            using IndexType = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                return;
            } else {
                row_count = index->DocNum();
                index_size = index->GetSizeInBytes();
            }
        },
        bmp_);
    auto new_chunk_index_entry = segment_index_entry->CreateBMPIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);

    BufferHandle handle = new_chunk_index_entry->GetIndex();
    auto *data_ptr = static_cast<AbstractBMP *>(handle.GetDataMut());
    *data_ptr = bmp_;
    bmp_ = nullptr;
    return new_chunk_index_entry;
}

} // namespace infinity