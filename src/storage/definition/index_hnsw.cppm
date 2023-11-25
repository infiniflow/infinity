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

import stl;
import index_def;
import parser;
import third_party;
import index_base;

export module index_hnsw;

namespace infinity {

export class IndexHnsw : public IndexBase {
public:
    static SharedPtr<IndexBase> Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list);

    IndexHnsw(String file_name, Vector<String> column_names, MetricType metric_type, SizeT M, SizeT ef_construction, SizeT ef)
        : IndexBase(file_name, IndexType::kHnsw, Move(column_names)), metric_type_(metric_type), M_(M), ef_construction_(ef_construction), ef_(ef) {}

    ~IndexHnsw() = default;

    bool operator==(const IndexHnsw &other) const;

    bool operator!=(const IndexHnsw &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    static SharedPtr<IndexBase> ReadAdv(char *&ptr, i32 maxbytes);

    virtual String ToString() const override;

    virtual Json Serialize() const override;

    static SharedPtr<IndexHnsw> Deserialize(const Json &index_def_json);

public:
    const MetricType metric_type_{MetricType::kInvalid};

    const SizeT M_{};
    const SizeT ef_construction_{};
    const SizeT ef_{};
};

} // namespace infinity
