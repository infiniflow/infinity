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

export module infinity_core:data_access_state;

import :stl;
import :data_block;
import :column_vector;

import internal_types;

namespace infinity {

export struct DeleteState {
    //    HashMap<u64, Vector<RowID>> rows_; // key is pair<segment_id, block_id>
    // HashMap<<segment, block_id>, block_offset>
    HashMap<SegmentID, HashMap<BlockID, Vector<BlockOffset>>> rows_; // use segment id, as the first level key, block id as the second level key
};

export struct AccessState {
    HashMap<SegmentID, HashMap<BlockID, Vector<BlockOffset>>> rows_;
};

export enum class ScanStateType {
    kTableScan,
    kIndexScan,
    kAnnScan,
};

export enum class ScanLocation {
    kLocal,
    kGlobal,
};

} // namespace infinity
