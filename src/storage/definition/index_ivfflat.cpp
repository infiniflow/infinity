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
import index_def;
import parser;
import third_party;
import serialize;
import index_base;

import infinity_exception;

module index_ivfflat;

namespace infinity {

SharedPtr<IndexBase> IndexIVFFlat::Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    SizeT centroids_count = 0;
    MetricType metric_type = MetricType::kInvalid;
    for (auto para : index_param_list) {
        if (para->param_name_ == "centroids_count") {
            centroids_count = std::stoi(para->param_value_);
        } else if (para->param_name_ == "metric") {
            metric_type = StringToMetricType(para->param_value_);
        }
    }
    if (metric_type == MetricType::kInvalid) {
        Error<StorageException>("Lack index parameter metric_type");
    }
    return MakeShared<IndexIVFFlat>(Move(file_name), Move(column_names), centroids_count, metric_type);
}

bool IndexIVFFlat::operator==(const IndexIVFFlat &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return centroids_count_ == other.centroids_count_ && metric_type_ == other.metric_type_;
}

bool IndexIVFFlat::operator!=(const IndexIVFFlat &other) const { return !(*this == other); }

i32 IndexIVFFlat::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(centroids_count_);
    size += sizeof(metric_type_);
    return size;
}

void IndexIVFFlat::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, centroids_count_);
    WriteBufAdv(ptr, metric_type_);
}

SharedPtr<IndexBase> IndexIVFFlat::ReadAdv(char *&, int32_t) {
    Error<StorageException>("Not implemented");
    return nullptr;
}

String IndexIVFFlat::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << centroids_count_ << ", " << MetricTypeToString(metric_type_);
    return ss.str();
}

Json IndexIVFFlat::Serialize() const {
    Json res = IndexBase::Serialize();
    res["centroids_count"] = centroids_count_;
    res["metric_type"] = MetricTypeToString(metric_type_);
    return res;
}

SharedPtr<IndexIVFFlat> IndexIVFFlat::Deserialize(const Json &) {
    Error<StorageException>("Not implemented");
    return nullptr;
}

} // namespace infinity