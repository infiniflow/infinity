//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module cached_index_scan;

import stl;
import cached_scan_base;
import base_expression;

namespace infinity {

class LogicalIndexScan;
class PhysicalIndexScan;

export class CachedIndexScan final : public CachedScanBase {
public:
    CachedIndexScan(TxnTimeStamp query_ts, const LogicalIndexScan *logical_index_scan);

    CachedIndexScan(TxnTimeStamp query_ts, const PhysicalIndexScan *physical_index_scan);

    u64 Hash() const override;

    bool Eq(const CachedNodeBase &other) const override;

private:
    SharedPtr<BaseExpression> filter_expression_;
};

}
