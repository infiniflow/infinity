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

export module hnsw_index_def;

namespace infinity {
export class HnswIndexDef : public IndexDef {
public:
    static SharedPtr<IndexDef> Make(SharedPtr<String> index_name, Vector<String> column_names, const Vector<InitParameter *> &index_para_list);

    HnswIndexDef(SharedPtr<String> index_name, Vector<String> column_names, MetricType metric_type, SizeT M, SizeT ef_construction, SizeT ef)
        : IndexDef(Move(index_name), IndexMethod::kHnsw, Move(column_names)), metric_type_(metric_type), M_(M), ef_construction_(ef_construction),
          ef_(ef) {}

    ~HnswIndexDef() = default;

    virtual bool operator==(const IndexDef &other) const override;

    virtual bool operator!=(const IndexDef &other) const override;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual String ToString() const override;

    virtual Json Serialize() const override;

public:
    const MetricType metric_type_{MetricType::kInvalid};

    const SizeT M_{};
    const SizeT ef_construction_{};
    const SizeT ef_{};
};

} // namespace infinity