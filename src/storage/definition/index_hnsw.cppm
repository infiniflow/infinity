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

export module index_hnsw;

import stl;
import index_base;

import third_party;
import index_base;
import base_table_ref;
import create_index_info;
import statement_common;

namespace infinity {

export enum class HnswEncodeType {
    kPlain,
    kLVQ,
    kInvalid,
};

export String HnswEncodeTypeToString(HnswEncodeType encode_type);

export HnswEncodeType StringToHnswEncodeType(const String &str);

export class IndexHnsw final : public IndexBase {
public:
    static SharedPtr<IndexBase> Make(SharedPtr<String> index_name,
                                     SharedPtr<String> index_comment,
                                     const String &file_name,
                                     Vector<String> column_names,
                                     const Vector<InitParameter *> &index_param_list);

    IndexHnsw(SharedPtr<String> index_name,
              SharedPtr<String> index_comment,
              const String &file_name,
              Vector<String> column_names,
              MetricType metric_type,
              HnswEncodeType encode_type,
              SizeT M,
              SizeT ef_construction,
              SizeT block_size)
        : IndexBase(IndexType::kHnsw, index_name, index_comment, file_name, std::move(column_names)), metric_type_(metric_type),
          encode_type_(encode_type), M_(M), ef_construction_(ef_construction), block_size_(block_size) {}

    ~IndexHnsw() final = default;

    bool operator==(const IndexHnsw &other) const;

    bool operator!=(const IndexHnsw &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual String ToString() const override;

    virtual String BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

public:
    static void
    ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name, const Vector<InitParameter *> &index_param_list);

public:
    const MetricType metric_type_{MetricType::kInvalid};
    HnswEncodeType encode_type_{HnswEncodeType::kInvalid};
    const SizeT M_{};
    const SizeT ef_construction_{};
    const SizeT block_size_{};
};

} // namespace infinity
