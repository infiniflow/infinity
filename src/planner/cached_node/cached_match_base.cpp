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

#include <vector>

module cached_match_base;

namespace infinity {

u64 CachedMatchBase::Hash() const {
    u64 h = CachedNodeBase::Hash();
    h = h ^ std::hash<String>{}(*schema_name_);
    h = h ^ std::hash<String>{}(*table_name_);
    h = h ^ std::hash<TxnTimeStamp>{}(query_ts_);
    return h;
}

bool CachedMatchBase::Equal(const CachedMatchBase &other) const {
    return *schema_name_ == *other.schema_name_ && *table_name_ == *other.table_name_ && query_ts_ == other.query_ts_;
}

} // namespace infinity