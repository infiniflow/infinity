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

export module cleanup_scanner;

import stl;
// import meta_entry_interface;

namespace infinity {

class NewCatalog;
class EntryInterface;
class MetaInterface;

export class CleanupScanner {
public:
    CleanupScanner(NewCatalog *catalog, TxnTimeStamp visible_ts);

    void Scan();

    void Cleanup() &&;

    void AddMeta(UniquePtr<MetaInterface> meta);

    void AddEntry(SharedPtr<EntryInterface> entry);

    TxnTimeStamp visible_ts() const { return visible_ts_; }

private:
    NewCatalog *const catalog_;
    const TxnTimeStamp visible_ts_;

    Vector<SharedPtr<EntryInterface>> entries_;

    Vector<UniquePtr<MetaInterface>> metas_;
};

} // namespace infinity
