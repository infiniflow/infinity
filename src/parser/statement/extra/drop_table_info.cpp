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

#include "drop_table_info.h"
#include <sstream>

namespace infinity {

std::string DropTableInfo::ToString() const {
    std::stringstream ss;
    ss << "DROP TABLE: " << schema_name_ << "." << table_name_;
    if (this->conflict_type_ == ConflictType::kIgnore) {
        ss << " IgnoreIfExists";
    } else {
        ss << " ThrowErrorIfExists";
    }
    return ss.str();
}

} // namespace infinity
