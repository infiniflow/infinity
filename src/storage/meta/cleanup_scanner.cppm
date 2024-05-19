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
import buffer_manager;

namespace infinity {

class Catalog;
class EntryInterface;
class MetaInterface;

export class CleanupScanner {
public:
    CleanupScanner(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr);

    void Scan();

    void Cleanup() &&;

    void AddEntry(SharedPtr<EntryInterface> entry);

    TxnTimeStamp visible_ts() const { return visible_ts_; }

    static void CleanupDir(const String &dir);

private:
    Catalog *const catalog_;

    const TxnTimeStamp visible_ts_;

    BufferManager *buffer_mgr_;
    
    Vector<SharedPtr<EntryInterface>> entries_;
};

} // namespace infinity
