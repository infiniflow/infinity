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

#include <concepts>

export module meta_entry_interface;

import stl;

namespace infinity {

class CleanupScanner;
class CleanupInfoTracer;

export class MetaInterface {
public:
    virtual ~MetaInterface() = default;

    virtual bool PickCleanup(CleanupScanner *scanner) = 0;

    virtual void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true) = 0;

    virtual bool Empty() = 0;
};

export template <typename Meta>
concept MetaConcept = std::derived_from<Meta, MetaInterface>;

export class EntryInterface {
public:
    virtual ~EntryInterface() = default;

    virtual void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true) = 0;

    virtual void PickCleanup(CleanupScanner *scanner) = 0;
};

export template <typename Entry>
concept EntryConcept = std::derived_from<Entry, EntryInterface>;

} // namespace infinity