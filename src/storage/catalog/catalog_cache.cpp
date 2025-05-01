// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module catalog_cache;

import stl;
// import status;
// import meta_info;
// import extra_ddl_info;
// import default_values;
import internal_types;
// import buffer_handle;
// import column_def;
// import profiler;
// import third_party;
// import storage;
// import catalog_delta_entry;

namespace infinity {

void TableCache::UpdateCapacityPosition(SegmentID segment_id, SegmentOffset segment_offset) { return; }

void TableCache::UpdateDataPosition(SegmentID segment_id, SegmentOffset segment_offset) { return; }

Tuple<Vector<SegmentID>, RowID> TableCache::GetDataDistance() const { return {{}, {}}; }

} // namespace infinity