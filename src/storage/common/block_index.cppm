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

export module infinity_core:block_index;

import :global_block_id;

import std;

namespace infinity {

class DBMeta;
class TableMeta;
class SegmentMeta;
class BlockMeta;
class NewTxn;
class TableIndexMeta;
class SegmentIndexMeta;

export struct NewSegmentSnapshot {
public:
    SegmentOffset segment_offset() const;
    const std::vector<std::unique_ptr<BlockMeta>> &block_map() const;
    std::unique_ptr<SegmentMeta> segment_meta_;

private:
    mutable std::vector<std::unique_ptr<BlockMeta>> block_map_;
};

export struct BlockIndex {
public:
    BlockIndex();

    ~BlockIndex();

    void NewInit(const std::shared_ptr<TableMeta> &table_meta);

    size_t BlockCount() const;

    size_t SegmentCount() const;

    BlockMeta *GetBlockMeta(u32 segment_id, u16 block_id) const;

    SegmentOffset GetSegmentOffset(SegmentID segment_id) const;

    BlockOffset GetBlockOffset(SegmentID segment_id, BlockID block_id) const;

    bool IsEmpty() const { return new_segment_block_index_.empty(); }

public:
    std::vector<std::shared_ptr<TableIndexMeta>> table_index_meta_map_;
    std::shared_ptr<TableMeta> table_meta_;
    std::map<SegmentID, NewSegmentSnapshot> new_segment_block_index_;
};

export struct NewIndexSnapshot {
    std::shared_ptr<TableIndexMeta> table_index_meta_;

    std::map<SegmentID, std::unique_ptr<SegmentIndexMeta>> segment_index_metas_;
};

export struct IndexIndex {
public:
    std::shared_ptr<NewIndexSnapshot> Insert(const std::string &index_name, std::shared_ptr<TableIndexMeta> table_index_meta);

    void Insert(std::string index_name, std::shared_ptr<NewIndexSnapshot> new_index_snapshot);

    bool IsEmpty() const { return new_index_snapshots_vec_.empty(); }

public:
    std::unordered_map<std::string, std::shared_ptr<NewIndexSnapshot>> new_index_snapshots_;
    std::vector<NewIndexSnapshot *> new_index_snapshots_vec_;
};

} // namespace infinity
