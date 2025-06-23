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

export module ivf_index_data;

import stl;
import index_ivf;
import ivf_index_storage;
import column_def;
import embedding_info;
import internal_types;
import logical_type;
import local_file_handle;
import infinity_exception;

namespace infinity {

class IndexBase;
class BufferManager;
class SegmentMeta;

export class IVFDataAccessorBase {
public:
    virtual const_ptr_t GetEmbedding(SizeT offset) = 0;

    virtual Pair<Span<const char>, SizeT> GetMultiVector(SizeT offset) = 0;
};

export class IVFIndexInChunk : protected IVF_Index_Storage {
    using IVF_Index_Storage::IVF_Index_Storage;

public:
    using IVF_Index_Storage::GetMemData;

    IVF_Index_Storage *GetIVFIndexStoragePtr() { return this; }

    const IVF_Index_Storage *GetIVFIndexStoragePtr() const { return this; }

    void BuildIVFIndex(SegmentMeta &segment_meta, u32 row_count, SharedPtr<ColumnDef> column_def);

    void BuildIVFIndex(RowID base_rowid, u32 row_count, IVFDataAccessorBase *data_accessor, const SharedPtr<ColumnDef> &column_def);

    void SaveIndexInner(LocalFileHandle &file_handle) const;

    void ReadIndexInner(LocalFileHandle &file_handle);

    static IVFIndexInChunk *GetNewIVFIndexInChunk(const IndexBase *ivf_index, const ColumnDef *column_def);

private:
    template <LogicalType column_t, EmbeddingDataType embedding_t>
    void BuildIVFIndexT(RowID base_rowid, u32 row_count, IVFDataAccessorBase *data_accessor, const SharedPtr<ColumnDef> &column_def);
};

} // namespace infinity
