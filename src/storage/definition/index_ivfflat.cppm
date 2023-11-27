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
import index_base;
import third_party;

export module index_ivfflat;

namespace infinity {
export class IndexIVFFlat : public IndexBase {
public:
    static SharedPtr<IndexBase> Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list);

    IndexIVFFlat(String file_name, Vector<String> column_names, SizeT centroids_count, MetricType metric_type)
        : IndexBase(file_name, IndexType::kIVFFlat, Move(column_names)), centroids_count_(centroids_count), metric_type_(metric_type) {}

    ~IndexIVFFlat() = default;

    bool operator==(const IndexIVFFlat &other) const;

    bool operator!=(const IndexIVFFlat &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    static SharedPtr<IndexBase> ReadAdv(char *&ptr, i32 maxbytes);

    virtual String ToString() const override;

    virtual Json Serialize() const override;

    static SharedPtr<IndexIVFFlat> Deserialize(const Json &index_def_json);

public:
    const SizeT centroids_count_{};

    const MetricType metric_type_{MetricType::kInvalid};
};

} // namespace infinity