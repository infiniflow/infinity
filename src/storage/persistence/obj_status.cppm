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

export module infinity_core:obj_status;

import :status;

import std.compat;
import third_party;

namespace infinity {

export struct Range {
    size_t start_{}; // inclusive
    size_t end_{};   // exclusive
    bool operator<(const Range &rhs) const { return start_ < rhs.start_; }
    bool operator==(const Range &rhs) const { return start_ == rhs.start_ && end_ == rhs.end_; }
    bool Cover(const Range &rhs) const { return start_ <= rhs.start_ && rhs.end_ <= end_; }
    bool Intersect(const Range &rhs) const { return start_ < rhs.end_ && rhs.start_ < end_; }
};

export enum class ObjCached {
    kNotCached,
    kDownloading,
    kCached,
};

export struct ObjStat {
    size_t obj_size_{};  // footer (if present) is excluded
    size_t parts_{};     // an object attribute
    size_t ref_count_{}; // the number of user (R and W) of some part of this object
    std::set<Range> deleted_ranges_;

    std::atomic<ObjCached> cached_ = ObjCached::kCached; // whether the object is in localdisk cache

    ObjStat() = default;

    ObjStat(size_t obj_size, size_t parts, size_t ref_count, ObjCached cached = ObjCached::kCached)
        : obj_size_(obj_size), parts_(parts), ref_count_(ref_count), cached_(cached) {}

    ObjStat(const ObjStat &other)
        : obj_size_(other.obj_size_), parts_(other.parts_), ref_count_(other.ref_count_), deleted_ranges_(other.deleted_ranges_),
          cached_(other.cached_.load()) {}

    ObjStat &operator=(const ObjStat &other) {
        if (this != &other) {
            obj_size_ = other.obj_size_;
            parts_ = other.parts_;
            ref_count_ = other.ref_count_;
            deleted_ranges_ = other.deleted_ranges_;
            cached_.store(other.cached_.load());
        }
        return *this;
    }

    ObjStat(ObjStat &&other)
        : obj_size_(other.obj_size_), parts_(other.parts_), ref_count_(other.ref_count_), deleted_ranges_(std::move(other.deleted_ranges_)),
          cached_(other.cached_.load()) {}

    ObjStat &operator=(ObjStat &&other) noexcept {
        if (this != &other) {
            obj_size_ = other.obj_size_;
            parts_ = other.parts_;
            ref_count_ = other.ref_count_;
            deleted_ranges_ = std::move(other.deleted_ranges_);
            cached_.store(other.cached_.load());
        }
        return *this;
    }

    // nlohmann::json Serialize() const;

    std::string ToString() const;

    void Deserialize(std::string_view str);

    // size_t GetSizeInBytes() const;
    // void WriteBufAdv(char *&buf) const;
    // static ObjStat ReadBufAdv(const char *&buf);

    void CheckValid(const std::string &obj_key, size_t current_object_size) const;
};

} // namespace infinity