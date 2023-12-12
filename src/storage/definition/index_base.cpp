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
import index_ivfflat;
import index_hnsw;
import index_full_text;
import third_party;
import parser;
import infinity_exception;

module index_base;

namespace infinity {

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

bool IndexBase::operator==(const IndexBase &other) const { return index_type_ == other.index_type_ && column_names_ == other.column_names_; }

bool IndexBase::operator!=(const IndexBase &other) const { return !(*this == other); }

int32_t IndexBase::GetSizeInBytes() const {
    int32_t size = 0;
    size += sizeof(index_type_);
    size += sizeof(int32_t);
    size += file_name_.length();
    size += sizeof(int32_t);
    for (const String &column_name : column_names_) {
        size += sizeof(int32_t) + column_name.length();
    }
    return size;
}

void IndexBase::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, index_type_);
    WriteBufAdv(ptr, file_name_);
    WriteBufAdv(ptr, static_cast<int32_t>(column_names_.size()));
    for (const String &column_name : column_names_) {
        WriteBufAdv(ptr, column_name);
    }
}

SharedPtr<IndexBase> IndexBase::ReadAdv(char *&ptr, int32_t maxbytes) {
//    char *const ptr_end = ptr + maxbytes;
    if (maxbytes <= 0) {
        Error<StorageException>("ptr goes out of range when reading IndexBase");
    }
    IndexType index_type = ReadBufAdv<IndexType>(ptr);
    Vector<String> column_names;
    String file_name = ReadBufAdv<String>(ptr);
    int32_t column_names_size = ReadBufAdv<int32_t>(ptr);
    for (int32_t i = 0; i < column_names_size; ++i) {
        column_names.emplace_back(ReadBufAdv<String>(ptr));
    }
    SharedPtr<IndexBase> res = nullptr;
    switch (index_type) {
        case IndexType::kIVFFlat: {
            size_t centroids_count = ReadBufAdv<size_t>(ptr);
            MetricType metric_type = ReadBufAdv<MetricType>(ptr);
            res = MakeShared<IndexIVFFlat>(file_name, column_names, centroids_count, metric_type);
            break;
        }
        case IndexType::kHnsw: {
            MetricType metric_type = ReadBufAdv<MetricType>(ptr);
            HnswEncodeType encode_type = ReadBufAdv<HnswEncodeType>(ptr);
            SizeT M = ReadBufAdv<SizeT>(ptr);
            SizeT ef_construction = ReadBufAdv<SizeT>(ptr);
            SizeT ef = ReadBufAdv<SizeT>(ptr);
            res = MakeShared<IndexHnsw>(file_name, column_names, metric_type, encode_type, M, ef_construction, ef);
            break;
        }
        case IndexType::kIRSFullText: {
            String analyzer = ReadBufAdv<String>(ptr);
            res = MakeShared<IndexFullText>(file_name, column_names, analyzer);
            break;
        }
        case IndexType::kInvalid: {
            Error<StorageException>("Error index method while reading");
        }
        default: {
            Error<StorageException>("Not implemented");
        }
    }
    if (maxbytes < 0) {
        Error<StorageException>("ptr goes out of range when reading IndexBase");
    }
    return res;
}

String IndexBase::ToString() const {
    std::stringstream ss;
    ss << "IndexBase: " << IndexInfo::IndexTypeToString(index_type_) << ", [";
    for (size_t i = 0; i < column_names_.size(); ++i) {
        ss << column_names_[i];
        if (i != column_names_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

Json IndexBase::Serialize() const {
    Json res;
    res["file_name"] = file_name_;
    res["index_type"] = IndexInfo::IndexTypeToString(index_type_);
    res["column_names"] = column_names_;
    return res;
}

SharedPtr<IndexBase> IndexBase::Deserialize(const Json &index_def_json) {
    SharedPtr<IndexBase> res = nullptr;
    String index_type_name = index_def_json["index_type"];
    IndexType index_type = IndexInfo::StringToIndexType(index_type_name);
    String file_name = index_def_json["file_name"];
    Vector<String> column_names = index_def_json["column_names"];
    switch (index_type) {
        case IndexType::kIVFFlat: {
            size_t centroids_count = index_def_json["centroids_count"];
            MetricType metric_type = StringToMetricType(index_def_json["metric_type"]);
            auto ptr = MakeShared<IndexIVFFlat>(file_name, Move(column_names), centroids_count, metric_type);
            res = std::static_pointer_cast<IndexBase>(ptr);
            break;
        }
        case IndexType::kHnsw: {
            SizeT M = index_def_json["M"];
            SizeT ef_construction = index_def_json["ef_construction"];
            SizeT ef = index_def_json["ef"];
            MetricType metric_type = StringToMetricType(index_def_json["metric_type"]);
            HnswEncodeType encode_type = StringToHnswEncodeType(index_def_json["encode_type"]);
            auto ptr = MakeShared<IndexHnsw>(file_name, Move(column_names), metric_type, encode_type, M, ef_construction, ef);
            res = std::static_pointer_cast<IndexBase>(ptr);
            break;
        }
        case IndexType::kIRSFullText: {
            String analyzer = index_def_json["analyzer"];
            auto ptr = MakeShared<IndexFullText>(file_name, Move(column_names), analyzer);
            res = std::static_pointer_cast<IndexBase>(ptr);
            break;
        }
        case IndexType::kInvalid: {
            Error<StorageException>("Error index method while deserializing");
        }
        default: {
            Error<StorageException>("Not implemented");
        }
    }
    return res;
}

} // namespace infinity