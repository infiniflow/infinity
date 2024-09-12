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

export module cast_table;

import singleton;
import stl;

import logical_type;

namespace infinity {

export class CastTable : public Singleton<CastTable> {
public:
    CastTable();

public:
    [[nodiscard]] i64 GetCastCost(LogicalType from, LogicalType to) const;

private:
    Array<Array<i64, to_underlying_val(LogicalType::kInvalid)>, to_underlying_val(LogicalType::kInvalid)> matrix_{};
};

} // namespace infinity
