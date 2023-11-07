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

#include "polygon_type.h"
#include <cstring>

namespace infinity {

PolygonType::PolygonType(const PolygonType &other) {
    this->point_count = other.point_count;

    this->bounding_box = other.bounding_box;

    ptr = new char[point_count * sizeof(PointType)]{0};

    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
}

PolygonType::PolygonType(PolygonType &&other) noexcept {
    this->point_count = other.point_count;
    this->bounding_box = other.bounding_box;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
}

PolygonType &PolygonType::operator=(const PolygonType &other) {
    if (this == &other)
        return *this;
    if (point_count != other.point_count) {
        Reset();
        point_count = other.point_count;

        ptr = new char[point_count * sizeof(PointType)]{0};
    }
    this->bounding_box = other.bounding_box;
    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
    return *this;
}

PolygonType &PolygonType::operator=(PolygonType &&other) noexcept {
    if (this == &other)
        return *this;
    if (this->point_count != 0) {
        Reset();
    }
    this->point_count = other.point_count;
    this->bounding_box = other.bounding_box;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
    return *this;
}

} // namespace infinity