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

export module ivf_index_storage;

import stl;

namespace infinity {

struct IndexIVFStorageOption;
class FileHandler;

// always use float for centroids
export class IVF_Centroids_Storage {
    u32 embedding_dimension_ = 0;
    u32 centroids_num_ = 0;
    UniquePtr<f32[]> centroids_data_ = {};

public:
    IVF_Centroids_Storage() = default;
    IVF_Centroids_Storage(u32 embedding_dimension, u32 centroids_num);
    f32 *data() { return centroids_data_.get(); }
    const f32 *data() const { return centroids_data_.get(); }
    u32 embedding_dimension() const { return embedding_dimension_; }
    u32 centroids_num() const { return centroids_num_; }
    void Save(FileHandler &file_handler) const;
    void Load(FileHandler &file_handler);
};

export class IVF_Part_Storage {
    u32 embedding_dimension_ = 0;
    u32 part_id_ = std::numeric_limits<u32>::max();
    u32 embedding_num_ = 0;
    Vector<SegmentOffset> embedding_segment_offsets_ = {};

public:
    IVF_Part_Storage(const u32 embedding_dimension, const u32 part_id) : embedding_dimension_(embedding_dimension), part_id_(part_id) {}
    virtual ~IVF_Part_Storage() = default;
    static UniquePtr<IVF_Part_Storage> Make(u32 embedding_dimension, u32 part_id, const IndexIVFStorageOption &ivf_storage_option);
    u32 embedding_dimension() const { return embedding_dimension_; }
    u32 part_id() const { return part_id_; }
    u32 embedding_num() const { return embedding_num_; }
    SegmentOffset embedding_segment_offset(const u32 embedding_index) const { return embedding_segment_offsets_[embedding_index]; }

    virtual void Save(FileHandler &file_handler) const;
    virtual void Load(FileHandler &file_handler);

    // only for unit-test, return f32 / i8 / u8 embedding data
    virtual Pair<const void *, SharedPtr<void>> GetDataForTest(u32 embedding_id) const = 0;
};

} // namespace infinity
