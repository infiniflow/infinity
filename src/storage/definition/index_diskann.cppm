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

export module index_diskann;

import stl;
import index_base;

import third_party;
import index_base;
import base_table_ref;
import create_index_info;
import statement_common;

namespace infinity {

export enum class DiskAnnEncodeType {
    kPlain,
    kInvalid,
};

export String DiskAnnEncodeTypeToString(DiskAnnEncodeType encode_type);

export DiskAnnEncodeType StringToDiskAnnEncodeType(const String &str);

export class IndexDiskAnn final : public IndexBase {
public:
    static SharedPtr<IndexBase> Make(SharedPtr<String> index_name,
                                     SharedPtr<String> index_comment,
                                     const String &file_name,
                                     Vector<String> column_names,
                                     const Vector<InitParameter *> &index_param_list);

    IndexDiskAnn(SharedPtr<String> index_name,
                 SharedPtr<String> index_comment,
                 const String &file_name,
                 Vector<String> column_names,
                 MetricType metric_type,
                 DiskAnnEncodeType encode_type,
                 SizeT R,
                 SizeT L,
                 SizeT num_pq_chunks,
                 SizeT num_parts)
        : IndexBase(IndexType::kDiskAnn, index_name, index_comment, file_name, std::move(column_names)), metric_type_(metric_type),
          encode_type_(encode_type), R_(R), L_(L), num_pq_chunks_(num_pq_chunks), num_parts_(num_parts) {}

    ~IndexDiskAnn() final = default;

    bool operator==(const IndexDiskAnn &other) const;

    bool operator!=(const IndexDiskAnn &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual String ToString() const override;

    virtual String BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

public:
    static void ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name);

public:
    const MetricType metric_type_{MetricType::kInvalid};
    DiskAnnEncodeType encode_type_{DiskAnnEncodeType::kInvalid};
    const SizeT R_{};             // degree of the graph
    const SizeT L_{};             // length of the candidates list
    const SizeT num_pq_chunks_{}; // dimension cut blocks by pq
    const SizeT num_parts_{};     // Number of dataset slices when building the Vamana index
};

} // namespace infinity
