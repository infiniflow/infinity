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

module obj_status;

import serialize;
import status;

namespace infinity {

nlohmann::json ObjStat::Serialize() const {
    nlohmann::json obj;
    obj["obj_size"] = obj_size_;
    obj["parts"] = parts_;
    obj["deleted_ranges"] = nlohmann::json::array();
    for (auto &range : deleted_ranges_) {
        nlohmann::json range_obj;
        range_obj["start"] = range.start_;
        range_obj["end"] = range.end_;
        obj["deleted_ranges"].emplace_back(range_obj);
    }
    return obj;
}

void ObjStat::Deserialize(const nlohmann::json &obj) {
    ref_count_ = 0;
    obj_size_ = obj["obj_size"];
    parts_ = obj["parts"];
    if (obj.contains("deleted_ranges")) {
        SizeT start = 0;
        SizeT end = 0;
        for (auto &range_obj : obj["deleted_ranges"]) {
            if (range_obj.contains("start")) {
                start = range_obj["start"];
            }
            if (range_obj.contains("end")) {
                end = range_obj["end"];
            }
            deleted_ranges_.emplace(Range{.start_ = start, .end_ = end});
        }
    }
}

SizeT ObjStat::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT) + sizeof(SizeT) + sizeof(SizeT);
    size += (sizeof(SizeT) + sizeof(SizeT)) * deleted_ranges_.size();
    return size;
}

void ObjStat::WriteBufAdv(char *&buf) const {
    ::infinity::WriteBufAdv(buf, obj_size_);
    ::infinity::WriteBufAdv(buf, parts_);
    ::infinity::WriteBufAdv(buf, deleted_ranges_.size());
    for (auto &range : deleted_ranges_) {
        ::infinity::WriteBufAdv(buf, range.start_);
        ::infinity::WriteBufAdv(buf, range.end_);
    }
}

ObjStat ObjStat::ReadBufAdv(const char *&buf) {
    ObjStat ret;
    ret.obj_size_ = ::infinity::ReadBufAdv<SizeT>(buf);
    ret.parts_ = ::infinity::ReadBufAdv<SizeT>(buf);
    ret.ref_count_ = 0;

    SizeT start, end;
    SizeT len = ::infinity::ReadBufAdv<SizeT>(buf);
    for (SizeT i = 0; i < len; ++i) {
        start = ::infinity::ReadBufAdv<SizeT>(buf);
        end = ::infinity::ReadBufAdv<SizeT>(buf);
        ret.deleted_ranges_.emplace(Range{.start_ = start, .end_ = end});
    }
    return ret;
}

} // namespace infinity
