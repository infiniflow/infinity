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

import std;
import stl;
import index_def;
import parser;
import third_party;
import serialize;

import infinity_exception;

module ivfflat_index_def;

namespace infinity {

SharedPtr<IndexDef> IVFFlatIndexDef::Make(SharedPtr<String> index_name, Vector<String> column_names, const Vector<InitParameter *> &index_para_list) {
    SizeT centroids_count = 0;
    MetricType metric_type = MetricType::kInvalid;
    for (auto para : index_para_list) {
        if (para->para_name_ == "centroids_count") {
            centroids_count = std::stoi(para->para_value_);
        } else if (para->para_name_ == "metric") {
            metric_type = StringToMetricType(para->para_value_);
        }
    }
    if (centroids_count == 0 || metric_type == MetricType::kInvalid) {
        Error<StorageException>("Lack index parameters");
    }
    auto ptr = MakeShared<IVFFlatIndexDef>(Move(index_name), IndexMethod::kIVFFlat, Move(column_names), centroids_count, metric_type);
    return std::static_pointer_cast<IndexDef>(ptr);
}

bool IVFFlatIndexDef::operator==(const IndexDef &other) const {
    try {
        auto other1 = dynamic_cast<const IVFFlatIndexDef &>(other);
        return IndexDef::operator==(other) && centroids_count_ == other1.centroids_count_ && metric_type_ == other1.metric_type_;
    } catch (std::bad_cast exception) {
        return false;
    }
}

bool IVFFlatIndexDef::operator!=(const IndexDef &other) const { return !(*this == other); }

i32 IVFFlatIndexDef::GetSizeInBytes() const {
    SizeT size = IndexDef::GetSizeInBytes();
    size += sizeof(centroids_count_);
    size += sizeof(metric_type_);
    return size;
}

void IVFFlatIndexDef::WriteAdv(char *&ptr) const {
    IndexDef::WriteAdv(ptr);
    WriteBufAdv(ptr, centroids_count_);
    WriteBufAdv(ptr, metric_type_);
}

String IVFFlatIndexDef::ToString() const {
    std::stringstream ss;
    ss << IndexDef::ToString() << ", " << centroids_count_ << ", " << MetricTypeToString(metric_type_);
    return ss.str();
}

Json IVFFlatIndexDef::Serialize() const {
    Json res = IndexDef::Serialize();
    res["centroids_count"] = centroids_count_;
    res["metric_type"] = MetricTypeToString(metric_type_);
    return res;
}

} // namespace infinity