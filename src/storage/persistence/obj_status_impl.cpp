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

module infinity_core:obj_status.impl;

import :obj_status;
import :status;
import :logger;

import std;
import third_party;

import serialize;

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

std::string ObjStat::ToString() const {
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
    return obj.dump();
}

void ObjStat::Deserialize(std::string_view str) {
    simdjson::padded_string obj_json(str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(obj_json);
    ref_count_ = 0;
    obj_size_ = doc["obj_size"];
    parts_ = doc["parts"];
    if (simdjson::array array; doc["deleted_ranges"].get(array) == simdjson::SUCCESS) {
        size_t start = 0;
        size_t end = 0;
        for (auto range_obj : array) {
            if (auto item = range_obj["start"]; item.error() == simdjson::SUCCESS) {
                start = item.get<size_t>();
            }
            if (auto item = range_obj["end"]; item.error() == simdjson::SUCCESS) {
                end = item.get<size_t>();
            }
            deleted_ranges_.emplace(Range{.start_ = start, .end_ = end});
        }
    }
}

size_t ObjStat::GetSizeInBytes() const {
    size_t size = sizeof(size_t) + sizeof(size_t) + sizeof(size_t);
    size += (sizeof(size_t) + sizeof(size_t)) * deleted_ranges_.size();
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
    ret.obj_size_ = ::infinity::ReadBufAdv<size_t>(buf);
    ret.parts_ = ::infinity::ReadBufAdv<size_t>(buf);
    ret.ref_count_ = 0;

    size_t start, end;
    size_t len = ::infinity::ReadBufAdv<size_t>(buf);
    for (size_t i = 0; i < len; ++i) {
        start = ::infinity::ReadBufAdv<size_t>(buf);
        end = ::infinity::ReadBufAdv<size_t>(buf);
        ret.deleted_ranges_.emplace(Range{.start_ = start, .end_ = end});
    }
    return ret;
}

void ObjStat::CheckValid(const std::string &obj_key, size_t current_object_size) const {
    const std::set<Range> &deleted_ranges = deleted_ranges_;
    if (deleted_ranges.size() >= 2) {
        auto it1 = deleted_ranges.begin();
        auto it2 = std::next(it1);
        while (it2 != deleted_ranges.end()) {
            if (it1->end_ >= it2->start_) {
                std::string error_message = fmt::format("CurrentObjFinalize Object {} deleted ranges intersect: [{}, {}), [{}, {})",
                                                   obj_key,
                                                   it1->start_,
                                                   it1->end_,
                                                   it2->start_,
                                                   it2->end_);
                LOG_ERROR(error_message);
            }
            it1 = it2;
            it2 = std::next(it2);
        }
    } else if (deleted_ranges.size() == 1) {
        auto it1 = deleted_ranges.begin();
        if (it1->start_ == 0 && it1->end_ == current_object_size) {
            std::string error_message = fmt::format("CurrentObjFinalize Object {} is fully deleted", obj_key);
            LOG_ERROR(error_message);
        }
    }
}

} // namespace infinity
