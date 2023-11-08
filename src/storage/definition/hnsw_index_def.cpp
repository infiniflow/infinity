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

import stl;
import parser;
import index_def;
import third_party;
import infinity_exception;
import serialize;

module hnsw_index_def;

namespace infinity {

SharedPtr<IndexDef> HnswIndexDef::Make(SharedPtr<String> index_name, Vector<String> column_names, const Vector<InitParameter *> &index_para_list) {
    //
    SizeT M = 16;
    SizeT ef_construction = 200;
    SizeT ef = 200;
    MetricType metric_type = MetricType::kInvalid;
    for (auto para : index_para_list) {
        if (para->para_name_ == "M") {
            M = std::stoi(para->para_value_);
        } else if (para->para_name_ == "ef_construction") {
            ef_construction = std::stoi(para->para_value_);
        } else if (para->para_name_ == "ef") {
            ef = std::stoi(para->para_value_);
        } else if (para->para_name_ == "metric") {
            metric_type = StringToMetricType(para->para_value_);
        } else {
            Error<StorageException>("Invalid index parameter");
        }
    }
    if (metric_type == MetricType::kInvalid) {
        Error<StorageException>("Lack index parameters");
    }
    return MakeShared<HnswIndexDef>(Move(index_name), Move(column_names), metric_type, M, ef_construction, ef);
}

bool HnswIndexDef::operator==(const IndexDef &other) const {
    try {
        auto other1 = dynamic_cast<const HnswIndexDef &>(other);
        return IndexDef::operator==(other) && metric_type_ == other1.metric_type_ && M_ == other1.M_ && ef_construction_ == other1.ef_construction_ &&
               ef_ == other1.ef_;
    } catch (std::bad_cast exception) {
        return false;
    }
}

bool HnswIndexDef::operator!=(const IndexDef &other) const { return !(*this == other); }

i32 HnswIndexDef::GetSizeInBytes() const {
    SizeT size = IndexDef::GetSizeInBytes();
    size += sizeof(metric_type_);
    size += sizeof(M_);
    size += sizeof(ef_construction_);
    size += sizeof(ef_);
    return size;
}

void HnswIndexDef::WriteAdv(char *&ptr) const {
    IndexDef::WriteAdv(ptr);
    WriteBufAdv(ptr, metric_type_);
    WriteBufAdv(ptr, M_);
    WriteBufAdv(ptr, ef_construction_);
    WriteBufAdv(ptr, ef_);
}

String HnswIndexDef::ToString() const {
    std::stringstream ss;
    ss << IndexDef::ToString() << ", " << MetricTypeToString(metric_type_) << ", " << M_ << ", " << ef_construction_ << ", " << ef_;
    return ss.str();
}

Json HnswIndexDef::Serialize() const {
    Json res = IndexDef::Serialize();
    res["metric_type"] = MetricTypeToString(metric_type_);
    res["M"] = M_;
    res["ef_construction"] = ef_construction_;
    res["ef"] = ef_;
    return res;
}
} // namespace infinity