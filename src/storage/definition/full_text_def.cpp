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

#include <string>
#include <vector>

import stl;
import base_index;
import third_party;
import parser;
import serialize;
import infinity_exception;

module full_text_def;

namespace infinity {

SharedPtr<BaseIndex> FullTextDef::Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    String analyzer{};
    SizeT param_count = index_param_list.size();
    for (SizeT param_idx = 0; param_idx < param_count; ++param_idx) {
        InitParameter *parameter = index_param_list[param_idx];
        if (parameter->param_name_ == "analyzer") {
            analyzer = parameter->param_value_;
        }
    }

    return MakeShared<FullTextDef>(file_name, Move(column_names), analyzer);
}

bool FullTextDef::operator==(const FullTextDef &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return analyzer_ == other.analyzer_;
}

bool FullTextDef::operator!=(const FullTextDef &other) const { return !(*this == other); }

i32 FullTextDef::GetSizeInBytes() const {
    SizeT size = BaseIndex::GetSizeInBytes();
    size += sizeof(int32_t) + analyzer_.length();
    return size;
}

void FullTextDef::WriteAdv(char *&ptr) const {
    BaseIndex::WriteAdv(ptr);
    WriteBufAdv(ptr, analyzer_);
}

SharedPtr<BaseIndex> FullTextDef::ReadAdv(char *&ptr, int32_t maxbytes) {
    Error<StorageException>("Not implemented");
}

String FullTextDef::ToString() const {
    std::stringstream ss;
    ss << BaseIndex::ToString();
    if(!analyzer_.empty()) {
        ss << ", " << analyzer_;
    }
    return ss.str();
}

Json FullTextDef::Serialize() const {
    Json res = BaseIndex::Serialize();
    res["analyzer"] = analyzer_;
    return res;
}

SharedPtr<FullTextDef> FullTextDef::Deserialize(const Json &index_def_json) {
    Error<StorageException>("Not implemented");
}

} // namespace infinity
