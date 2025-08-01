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

export module infinity_core:index_base;

import :stl;
import :third_party;
import create_index_info;
import global_resource_usage;

namespace infinity {

// TODO shenyushi: use definition in knn_exprs.h
export enum class MetricType : i8 {
    kMetricCosine,
    kMetricInnerProduct,
    kMetricL2,
    kInvalid,
};

export String MetricTypeToString(MetricType metric_type);

export MetricType StringToMetricType(const String &str);

export class IndexBase {
protected:
    explicit IndexBase(IndexType index_type,
                       SharedPtr<String> index_name,
                       SharedPtr<String> index_comment,
                       const String &file_name,
                       Vector<String> column_names)
        : index_type_(index_type), index_name_(std::move(index_name)), index_comment_(std::move(index_comment)), file_name_(file_name),
          column_names_(std::move(column_names)) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("IndexBase");
#endif
    }

public:
    explicit IndexBase(SharedPtr<String> index_name) : index_name_(std::move(index_name)) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("IndexBase");
#endif
    }

    IndexBase(const IndexBase &other)
        : index_type_(other.index_type_), index_name_(other.index_name_), index_comment_(other.index_comment_), file_name_(other.file_name_),
          column_names_(other.column_names_) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("IndexBase");
#endif
    }

    virtual ~IndexBase() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("IndexBase");
#endif
    }

    bool operator==(const IndexBase &other) const;

    bool operator!=(const IndexBase &other) const;

public:
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    virtual i32 GetSizeInBytes() const;

    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const;

    // Read char from buffer
    static SharedPtr<IndexBase> ReadAdv(const char *&ptr, i32 maxbytes);

    virtual String ToString() const;
    virtual String BuildOtherParamsString() const { return ""; }
    virtual nlohmann::json Serialize() const;

    static SharedPtr<IndexBase> Deserialize(std::string_view index_def_str);

    inline String column_name() const { return column_names_[0]; }

    bool ContainsColumn(const String &column_name) const;

public:
    IndexType index_type_{IndexType::kInvalid};
    SharedPtr<String> index_name_{};
    SharedPtr<String> index_comment_{};
    const String file_name_{};
    const Vector<String> column_names_{};
};
} // namespace infinity
