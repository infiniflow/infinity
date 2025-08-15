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

export module infinity_core:index_diskann;

import :stl;
import :index_base;
import :index_base;
import :base_table_ref;

import third_party;

import create_index_info;
import statement_common;

namespace infinity {

export enum class DiskAnnEncodeType {
    kPlain,
    kInvalid,
};

export std::string DiskAnnEncodeTypeToString(DiskAnnEncodeType encode_type);

export DiskAnnEncodeType StringToDiskAnnEncodeType(const std::string &str);

export class IndexDiskAnn final : public IndexBase {
public:
    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                     std::shared_ptr<std::string> index_comment,
                                     const std::string &file_name,
                                     std::vector<std::string> column_names,
                                     const std::vector<InitParameter *> &index_param_list);

    IndexDiskAnn(std::shared_ptr<std::string> index_name,
                 std::shared_ptr<std::string> index_comment,
                 const std::string &file_name,
                 std::vector<std::string> column_names,
                 MetricType metric_type,
                 DiskAnnEncodeType encode_type,
                 size_t R,
                 size_t L,
                 size_t num_pq_chunks,
                 size_t num_parts)
        : IndexBase(IndexType::kDiskAnn, index_name, index_comment, file_name, std::move(column_names)), metric_type_(metric_type),
          encode_type_(encode_type), R_(R), L_(L), num_pq_chunks_(num_pq_chunks), num_parts_(num_parts) {}

    ~IndexDiskAnn() final = default;

    bool operator==(const IndexDiskAnn &other) const;

    bool operator!=(const IndexDiskAnn &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual std::string ToString() const override;

    virtual std::string BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

public:
    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name);

public:
    const MetricType metric_type_{MetricType::kInvalid};
    DiskAnnEncodeType encode_type_{DiskAnnEncodeType::kInvalid};
    const size_t R_{};             // degree of the graph
    const size_t L_{};             // length of the candidates list
    const size_t num_pq_chunks_{}; // dimension cut blocks by pq
    const size_t num_parts_{};     // Number of dataset slices when building the Vamana index
};

} // namespace infinity
