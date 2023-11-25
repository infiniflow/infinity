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
import parser;
import index_def;
import third_party;
import infinity_exception;
import serialize;
import default_values;
import base_index;

module hnsw_def;

namespace infinity {

SharedPtr<BaseIndex> HnswDef::Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    SizeT M = HNSW_M;
    SizeT ef_construction = HNSW_EF_CONSTRUCTION;
    SizeT ef = HNSW_EF;
    MetricType metric_type = MetricType::kInvalid;
    for (auto para : index_param_list) {
        if (para->param_name_ == "M") {
            M = std::stoi(para->param_value_);
        } else if (para->param_name_ == "ef_construction") {
            ef_construction = std::stoi(para->param_value_);
        } else if (para->param_name_ == "ef") {
            ef = std::stoi(para->param_value_);
        } else if (para->param_name_ == "metric") {
            metric_type = StringToMetricType(para->param_value_);
        } else {
            Error<StorageException>("Invalid index parameter");
        }
    }
    if (metric_type == MetricType::kInvalid) {
        Error<StorageException>("Lack index parameters");
    }
    return MakeShared<HnswDef>(file_name, Move(column_names), metric_type, M, ef_construction, ef);
}

bool HnswDef::operator==(const HnswDef &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return metric_type_ == other.metric_type_ && M_ == other.M_ && ef_construction_ == other.ef_construction_ && ef_ == other.ef_;
}

bool HnswDef::operator!=(const HnswDef &other) const { return !(*this == other); }

i32 HnswDef::GetSizeInBytes() const {
    SizeT size = BaseIndex::GetSizeInBytes();
    size += sizeof(metric_type_);
    size += sizeof(M_);
    size += sizeof(ef_construction_);
    size += sizeof(ef_);
    return size;
}

void HnswDef::WriteAdv(char *&ptr) const {
    BaseIndex::WriteAdv(ptr);
    WriteBufAdv(ptr, metric_type_);
    WriteBufAdv(ptr, M_);
    WriteBufAdv(ptr, ef_construction_);
    WriteBufAdv(ptr, ef_);
}

SharedPtr<BaseIndex> HnswDef::ReadAdv(char *&ptr, int32_t maxbytes) {
    Error<StorageException>("Not implemented");
}

String HnswDef::ToString() const {
    std::stringstream ss;
    ss << BaseIndex::ToString() << ", " << MetricTypeToString(metric_type_) << ", " << M_ << ", " << ef_construction_ << ", " << ef_;
    return ss.str();
}

Json HnswDef::Serialize() const {
    Json res = BaseIndex::Serialize();
    res["metric_type"] = MetricTypeToString(metric_type_);
    res["M"] = M_;
    res["ef_construction"] = ef_construction_;
    res["ef"] = ef_;
    return res;
}

SharedPtr<HnswDef> HnswDef::Deserialize(const Json &index_def_json) {
    Error<StorageException>("Not implemented");
}

} // namespace infinity