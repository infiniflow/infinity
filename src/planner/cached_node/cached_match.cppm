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

export module cached_match;

import stl;
import cached_node_base;
import cached_scan_base;
import logical_node_type;

namespace infinity {

class BaseTableRef;
class MatchExpression;
class BaseExpression;
class LogicalMatch;
class PhysicalMatch;

export class CachedMatch final : public CachedScanBase {
public:
    CachedMatch(TxnTimeStamp query_ts, LogicalMatch *logical_match);

    CachedMatch(TxnTimeStamp query_ts, PhysicalMatch *physical_match);

    u64 Hash() const override;

    bool Eq(const CachedNodeBase &other) const override;

private:
    SharedPtr<MatchExpression> match_expr_{};
    SharedPtr<BaseExpression> filter_expression_{};
    u32 topn_;
};

} // namespace infinity
