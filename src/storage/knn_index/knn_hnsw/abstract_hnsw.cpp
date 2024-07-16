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

module abstract_hnsw;

import segment_index_entry;
import chunk_index_entry;
import buffer_manager;
import buffer_handle;

namespace infinity {

SharedPtr<ChunkIndexEntry> HnswIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) {
    SizeT row_count = 0;
    SizeT index_size = 0;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                row_count = index->GetVecNum();
                index_size = index->GetSizeInBytes();
            }
        },
        hnsw_);
    auto new_chunk_indey_entry = segment_index_entry->CreateHnswIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);

    BufferHandle handle = new_chunk_indey_entry->GetIndex();
    auto *data_ptr = static_cast<AbstractHnsw *>(handle.GetDataMut());
    *data_ptr = hnsw_;
    hnsw_ = nullptr;
    return new_chunk_indey_entry;
}

} // namespace infinity