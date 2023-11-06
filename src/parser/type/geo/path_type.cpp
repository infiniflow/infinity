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

#include "path_type.h"
#include <cstring>

namespace infinity {
PathType::PathType(const PathType &other) {
    this->point_count = other.point_count;
    this->closed = other.closed;

    ptr = new char[point_count * sizeof(PointType)]{0};
    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
}

PathType::PathType(PathType &&other) noexcept {
    this->point_count = other.point_count;
    this->closed = other.closed;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
}

PathType &PathType::operator=(const PathType &other) {
    if (this == &other)
        return *this;
    if (point_count != other.point_count) {
        this->Reset();
        point_count = other.point_count;

        ptr = new char[point_count * sizeof(PointType)]{0};
    }
    this->closed = other.closed;
    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
    return *this;
}

PathType &PathType::operator=(PathType &&other) noexcept {
    if (this == &other)
        return *this;
    if (this->point_count != 0) {
        Reset();
    }
    this->point_count = other.point_count;
    this->closed = other.closed;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
    return *this;
}

} // namespace infinity
