// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module column_meta;

import stl;
import status;

namespace infinity {

class BlockMeta;
class KVInstance;
class BufferObj;
class ColumnDef;

export class ColumnMeta {
public:
    ColumnMeta(SizeT column_idx, BlockMeta &block_meta);

    KVInstance &kv_instance() const { return kv_instance_; }

    BlockMeta &block_meta() const { return block_meta_; }

    SizeT column_idx() const { return column_idx_; }

    Status GetChunkOffset(SizeT &chunk_offset) {
        if (!chunk_offset_) {
            Status status = LoadChunkOffset();
            if (!status.ok()) {
                return status;
            }
        }
        chunk_offset = *chunk_offset_;
        return Status::OK();
    }

    Status SetChunkOffset(SizeT chunk_offset);

    Status InitSet();

    Status LoadSet();

    Status UninitSet(const ColumnDef *column_def);

    Status GetColumnBuffer(BufferObj *&column_buffer, BufferObj *&outline_buffer);

    Status FilePaths(Vector<String> &paths);

private:
    Status GetColumnBuffer(BufferObj *&column_buffer, BufferObj *&outline_buffer, const ColumnDef *column_def);

    Status LoadChunkOffset();

    Status LoadColumnBuffer(const ColumnDef *col_def);

    String GetColumnTag(const String &tag) const;

private:
    KVInstance &kv_instance_;
    BlockMeta &block_meta_;
    SizeT column_idx_;

    Optional<SizeT> chunk_offset_;

    BufferObj *column_buffer_ = nullptr;
    BufferObj *outline_buffer_ = nullptr;
};

} // namespace infinity
