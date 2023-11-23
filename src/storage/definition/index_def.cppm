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
import third_party;

export module index_def;

namespace infinity {

export enum class IndexMethod {
    kIVFFlat,
    kIVFSQ8,
    kHnsw,
    kIRSFullText,
    kInvalid,
};

// TODO shenyushi: use definition in knn_exprs.h
export enum class MetricType {
    kMerticInnerProduct,
    kMerticL2,
    kInvalid,
};

export String IndexMethodToString(IndexMethod method);

export String MetricTypeToString(MetricType metric_type);

IndexMethod StringToIndexMethod(const String &str);

export MetricType StringToMetricType(const String &str);

export class IndexDef {
protected:
    explicit IndexDef(SharedPtr<String> index_name, IndexMethod method_type, Vector<String> column_names)
        : index_name_(Move(index_name)), method_type_(method_type), column_names_(Move(column_names)){};

public:
    virtual ~IndexDef() = default;

    virtual bool operator==(const IndexDef &other) const;

    virtual bool operator!=(const IndexDef &other) const;

public:
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    virtual i32 GetSizeInBytes() const;

    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const;

    // Read char from buffer
    static SharedPtr<IndexDef> ReadAdv(char *&ptr, i32 maxbytes);

    virtual String ToString() const;

    virtual Json Serialize() const;

    static SharedPtr<IndexDef> Deserialize(const Json &index_def_json);

public:
    SharedPtr<String> index_name_{};
    const IndexMethod method_type_{IndexMethod::kInvalid};
    const Vector<String> column_names_{};
};
} // namespace infinity