// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core;

import :stl;
import :memindex_tracer;
import :infinity_context;
import :storage;

namespace infinity {

void BaseMemIndex::IncreaseMemoryUsageBase(SizeT mem) {
    Storage *storage = InfinityContext::instance().storage();
    if (!storage) {
        return;
    }
    auto *memindex_tracer = storage->memindex_tracer();
    if (!memindex_tracer) {
        return;
    }
    memindex_tracer->IncreaseMemoryUsage(mem);
}

void BaseMemIndex::DecreaseMemoryUsageBase(SizeT mem) {
    Storage *storage = InfinityContext::instance().storage();
    if (!storage) {
        return;
    }
    auto *memindex_tracer = storage->memindex_tracer();
    if (!memindex_tracer) {
        return;
    }
    memindex_tracer->DecreaseMemUsed(mem);
}

SizeT BaseMemIndex::GetMemUsed() const { return GetInfo().mem_used_; }

SizeT BaseMemIndex::GetRowCount() const { return GetInfo().row_count_; }

} // namespace infinity