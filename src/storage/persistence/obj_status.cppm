// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module obj_status;

import stl;
import third_party;

namespace infinity {

class Status;

export struct Range {
    SizeT start_{}; // inclusive
    SizeT end_{};   // exclusive
    bool operator<(const Range &rhs) const { return start_ < rhs.start_; }
    bool operator==(const Range &rhs) const { return start_ == rhs.start_ && end_ == rhs.end_; }
    bool Cover(const Range &rhs) const { return start_ <= rhs.start_ && rhs.end_ <= end_; }
    bool Intersect(const Range &rhs) const { return start_ < rhs.end_ && rhs.start_ < end_; }
};

export struct ObjStat {
    SizeT obj_size_{}; // footer (if present) is excluded
    SizeT parts_{};    // an object attribute
    SizeT ref_count_{}; // the number of user (R and W) of some part of this object
    Set<Range> deleted_ranges_{};

    bool cached_{}; // whether the object is in localdisk cache

    ObjStat() = default;

    ObjStat(SizeT obj_size, SizeT parts, SizeT ref_count) : obj_size_(obj_size), parts_(parts), ref_count_(ref_count) {}

    nlohmann::json Serialize() const;

    void Deserialize(const nlohmann::json &obj);

    SizeT GetSizeInBytes() const;

    void WriteBufAdv(char *&buf) const;

    static ObjStat ReadBufAdv(const char *&buf);

    void CheckValid(const String &obj_key, SizeT current_object_size) const;
};

}