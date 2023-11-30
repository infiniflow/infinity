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
#include <sstream>
#include <vector>

import stl;
import serialize;
import index_ivfflat;
import index_hnsw;
import index_base;
import third_party;

import infinity_exception;

module index_def;
//--------------------------------------------------

namespace infinity {

bool IndexDef::operator==(const IndexDef &other) const {
    if (*index_name_ != *other.index_name_) {
        return false;
    }

    if (index_array_.size() != index_array_.size()) {
        return false;
    }

    SizeT index_count = index_array_.size();
    for (SizeT idx = 0; idx < index_count; ++idx) {
        if (index_array_[idx] != index_array_[idx]) {
            return false;
        }
    }

    return true;
}

bool IndexDef::operator!=(const IndexDef &other) const { return !(*this == other); }

int32_t IndexDef::GetSizeInBytes() const {
    int32_t size = 0;
    size += sizeof(int32_t) + index_name_->length();
    size += sizeof(int32_t); // for the length of index_array_
    for (const SharedPtr<IndexBase> &index_base : index_array_) {
        size += sizeof(int32_t); // each base index size
        size += index_base->GetSizeInBytes();
    }
    return size;
}

void IndexDef::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, *index_name_);
    WriteBufAdv(ptr, static_cast<int32_t>(index_array_.size()));
    for (const SharedPtr<IndexBase> &index_base : index_array_) {
        WriteBufAdv(ptr, index_base->GetSizeInBytes());
        index_base->WriteAdv(ptr);
    }
}

SharedPtr<IndexDef> IndexDef::ReadAdv(char *&ptr, int32_t maxbytes) {
//    char *const ptr_end = ptr + maxbytes;
    Assert<StorageException>(maxbytes > 0, "ptr goes out of range when reading IndexDef");
    SharedPtr<String> index_name = MakeShared<String>(ReadBufAdv<String>(ptr));
    SharedPtr<IndexDef> index_def = MakeShared<IndexDef>(index_name);

    int32_t index_array_count = ReadBufAdv<int32_t>(ptr);
    index_def->index_array_.reserve(index_array_count);

    for (int32_t i = 0; i < index_array_count; ++i) {
        int32_t index_size = ReadBufAdv<int32_t>(ptr);
        index_def->index_array_.emplace_back(IndexBase::ReadAdv(ptr, index_size));
    }
    return index_def;
}

String IndexDef::ToString() const {
    std::stringstream ss;
    ss << "IndexDef(" << *index_name_ << ", [";
    for (size_t i = 0; i < index_array_.size(); ++i) {
        ss << index_array_[i]->ToString();
        if (i != index_array_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "])";
    return ss.str();
}

Json IndexDef::Serialize() const {
    Json res;
    res["index_name"] = *index_name_;
    for (const auto &index : index_array_) {
        res["indexes"].emplace_back(index->Serialize());
    }
    return res;
}

SharedPtr<IndexDef> IndexDef::Deserialize(const Json &index_def_json) {
    SharedPtr<String> index_name = MakeShared<String>(index_def_json["index_name"]);
    SharedPtr<IndexDef> res = MakeShared<IndexDef>(index_name);
    for (const auto &index : index_def_json["indexes"]) {
        SharedPtr<IndexBase> index_base = IndexBase::Deserialize(index);
        res->index_array_.emplace_back(Move(index_base));
    }
    return res;
}

} // namespace infinity