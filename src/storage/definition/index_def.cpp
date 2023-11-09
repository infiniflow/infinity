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

#include <memory>
#include <string>
#include <vector>

import stl;
import serialize;
import ivfflat_index_def;
import hnsw_index_def;
import third_party;

import infinity_exception;

module index_def;

namespace infinity {
String IndexMethodToString(IndexMethod method) {
    switch (method) {
        case IndexMethod::kIVFFlat: {
            return "IVFFlat";
        }
        case IndexMethod::kIVFSQ8: {
            return "IVFSQ8";
        }
        case IndexMethod::kHnsw: {
            return "HNSW";
        }
        case IndexMethod::kInvalid: {
            return "Invalid";
        }
    }
}

String MetricTypeToString(MetricType metric_type) {
    switch (metric_type) {
        case MetricType::kMerticInnerProduct: {
            return "ip";
        }
        case MetricType::kMerticL2: {
            return "l2";
        }
        case MetricType::kInvalid: {
            return "Invalid";
        }
    }
}

IndexMethod StringToIndexMethod(const String &str) {
    if (str == "IVFFlat") {
        return IndexMethod::kIVFFlat;
    } else if (str == "IVFSQ8") {
        return IndexMethod::kIVFSQ8;
    } else if (str == "HNSW") {
        return IndexMethod::kHnsw;
    } else {
        return IndexMethod::kInvalid;
    }
}

MetricType StringToMetricType(const String &str) {
    if (str == "ip") {
        return MetricType::kMerticInnerProduct;
    } else if (str == "l2") {
        return MetricType::kMerticL2;
    } else {
        return MetricType::kInvalid;
    }
}
} // namespace infinity

//--------------------------------------------------

namespace infinity {

bool IndexDef::operator==(const IndexDef &other) const {
    return *index_name_ == *other.index_name_ && method_type_ == other.method_type_ && column_names_ == other.column_names_;
}

bool IndexDef::operator!=(const IndexDef &other) const { return !(*this == other); }

int32_t IndexDef::GetSizeInBytes() const {
    int32_t size = 0;
    size += sizeof(int32_t) + index_name_->length();
    size += sizeof(method_type_);
    size += sizeof(int32_t);
    for (const String &column_name : column_names_) {
        size += sizeof(int32_t) + column_name.length();
    }
    return size;
}

void IndexDef::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, *index_name_);
    WriteBufAdv(ptr, method_type_);
    WriteBufAdv(ptr, static_cast<int32_t>(column_names_.size()));
    for (const String &column_name : column_names_) {
        WriteBufAdv(ptr, column_name);
    }
}

SharedPtr<IndexDef> IndexDef::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    Assert<StorageException>(maxbytes > 0, "ptr goes out of range when reading IndexDef");
    SharedPtr<String> index_name = MakeShared<String>(ReadBufAdv<String>(ptr));
    IndexMethod method_type = ReadBufAdv<IndexMethod>(ptr);
    Vector<String> column_names;
    int32_t column_names_size = ReadBufAdv<int32_t>(ptr);
    for (int32_t i = 0; i < column_names_size; ++i) {
        column_names.emplace_back(ReadBufAdv<String>(ptr));
    }
    SharedPtr<IndexDef> res = nullptr;
    switch (method_type) {
        case IndexMethod::kIVFFlat: {
            size_t centroids_count = ReadBufAdv<size_t>(ptr);
            MetricType metric_type = ReadBufAdv<MetricType>(ptr);
            auto res1 = MakeShared<IVFFlatIndexDef>(index_name, column_names, centroids_count, metric_type);
            res = std::static_pointer_cast<IndexDef>(res1);
            break;
        }
        case IndexMethod::kHnsw: {
            MetricType metric_type = ReadBufAdv<MetricType>(ptr);
            SizeT M = ReadBufAdv<SizeT>(ptr);
            SizeT ef_construction = ReadBufAdv<SizeT>(ptr);
            SizeT ef = ReadBufAdv<SizeT>(ptr);
            auto res1 = MakeShared<HnswIndexDef>(index_name, column_names, metric_type, M, ef_construction, ef);
            res = std::static_pointer_cast<IndexDef>(res1);
            break;
        }
        case IndexMethod::kInvalid: {
            Error<StorageException>("Error index method while reading");
        }
        default: {
            Error<StorageException>("Not implemented");
        }
    }
    Assert<StorageException>(maxbytes >= 0, "ptr goes out of range when reading IndexDef");
    return res;
}

String IndexDef::ToString() const {
    std::stringstream ss;
    ss << "IndexDef(" << index_name_ << ", " << IndexMethodToString(method_type_) << ", [";
    for (size_t i = 0; i < column_names_.size(); ++i) {
        ss << column_names_[i];
        if (i != column_names_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "])";
    return ss.str();
}

Json IndexDef::Serialize() const {
    Json res;
    res["index_name"] = *index_name_;
    res["method_type"] = IndexMethodToString(method_type_);
    res["column_names"] = column_names_;
    return res;
}

SharedPtr<IndexDef> IndexDef::Deserialize(const Json &index_def_json) {
    SharedPtr<IndexDef> res = nullptr;
    auto index_name = MakeShared<String>(index_def_json["index_name"]);
    IndexMethod method_type = StringToIndexMethod(index_def_json["method_type"]);
    Vector<String> column_names = index_def_json["column_names"];
    switch (method_type) {
        case IndexMethod::kIVFFlat: {
            size_t centroids_count = index_def_json["centroids_count"];
            MetricType metric_type = StringToMetricType(index_def_json["metric_type"]);
            auto ptr = MakeShared<IVFFlatIndexDef>(Move(index_name), Move(column_names), centroids_count, metric_type);
            res = std::static_pointer_cast<IndexDef>(ptr);
            break;
        }
        case IndexMethod::kHnsw: {
            SizeT M = index_def_json["M"];
            SizeT ef_construction = index_def_json["ef_construction"];
            SizeT ef = index_def_json["ef"];
            MetricType metric_type = StringToMetricType(index_def_json["metric_type"]);
            auto ptr = MakeShared<HnswIndexDef>(Move(index_name), Move(column_names), metric_type, M, ef_construction, ef);
            break;
        }
        case IndexMethod::kInvalid: {
            Error<StorageException>("Error index method while deserializing");
        }
        default: {
            Error<StorageException>("Not implemented");
        }
    }
    return res;
}

} // namespace infinity