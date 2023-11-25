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
import parser;
import third_party;

export module index_base;

namespace infinity {

// TODO shenyushi: use definition in knn_exprs.h
export enum class MetricType {
    kMerticInnerProduct,
    kMerticL2,
    kInvalid,
};

export String MetricTypeToString(MetricType metric_type);

export MetricType StringToMetricType(const String &str);

export class IndexBase {
protected:
    explicit IndexBase(String file_name, IndexType index_type, Vector<String> column_names)
        : file_name_(Move(file_name)), index_type_(index_type), column_names_(Move(column_names)){};

public:
    virtual ~IndexBase() = default;

    bool operator==(const IndexBase &other) const;

    bool operator!=(const IndexBase &other) const;

public:
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    virtual i32 GetSizeInBytes() const;

    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const;

    // Read char from buffer
    static SharedPtr<IndexBase> ReadAdv(char *&ptr, i32 maxbytes);

    virtual String ToString() const;

    virtual Json Serialize() const;

    static SharedPtr<IndexBase> Deserialize(const Json &index_def_json);

    inline String column_name() { return column_names_[0]; }

public:
    const IndexType index_type_{IndexType::kInvalid};
    const String file_name_{};
    const Vector<String> column_names_{};
};
} // namespace infinity