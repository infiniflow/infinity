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

export module mem_index;

import stl;
import secondary_index_in_mem;
import ivf_index_data_in_mem;
import emvb_index_in_mem;
import memory_indexer;
import abstract_hnsw;
import abstract_bmp;

namespace infinity {

export struct MemIndex {
    void ClearMemIndex() {
        std::unique_lock<std::mutex> lock(mtx_);
        if (!to_clear_) {
            return;
        }
        to_clear_ = false;
        memory_hnsw_index_.reset();
        memory_ivf_index_.reset();
        memory_indexer_.reset();
        memory_secondary_index_.reset();
        memory_emvb_index_.reset();
        memory_bmp_index_.reset();
    }

    std::mutex mtx_; // Used by append / mem index dump / clear
    bool to_clear_ = false;

    SharedPtr<HnswIndexInMem> memory_hnsw_index_{};
    SharedPtr<IVFIndexInMem> memory_ivf_index_{};
    SharedPtr<MemoryIndexer> memory_indexer_{};
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_{};
    SharedPtr<EMVBIndexInMem> memory_emvb_index_{};
    SharedPtr<BMPIndexInMem> memory_bmp_index_{};
};

} // namespace infinity