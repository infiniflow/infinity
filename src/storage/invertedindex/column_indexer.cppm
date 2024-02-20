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

export module column_indexer;
import stl;
import memory_pool;
import index_defines;
import index_config;
import data_block;

import column_vector;
import third_party;
import memory_indexer;
import segment;
import internal_types;

namespace infinity {

class Indexer;
export class ColumnIndexer {
public:
    ColumnIndexer(Indexer *indexer,
                  u64 column_id,
                  const InvertedIndexConfig &index_config,
                  SharedPtr<MemoryPool> byte_slice_pool,
                  SharedPtr<RecyclePool> buffer_pool);

    ~ColumnIndexer();

    MemoryIndexer *GetMemoryIndexer() { return active_memory_indexer_.get(); }

    void Insert(const ColumnVector &column_vector, u32 row_offset, u32 row_count, RowID row_id_begin);

    void PreCommit();

    void Commit();

    void Dump();

private:
    UniquePtr<MemoryIndexer> active_memory_indexer_;
    String index_name_;
    u32 current_segment_id_{0};
};

} // namespace infinity
