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

module memindex_tracer;

import stl;
import base_memindex;

namespace infinity {

MemIndexTracer::MemIndexTracer(SizeT index_memory_limit) : index_memory_limit_(index_memory_limit) {}

Vector<MemIndexTracerInfo> MemIndexTracer::GetMemIndexTracerInfo() {
    Vector<MemIndexTracerInfo> info_vec;
    std::lock_guard lck(mtx_);
    info_vec.reserve(memindexes_.size());
    for (auto *memindex : memindexes_) {
        info_vec.push_back(memindex->GetInfo());
    }
    return info_vec;
}

void MemIndexTracer::TriggerDump() {
    //
}

} // namespace infinity
