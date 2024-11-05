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
import global_resource_usage;

namespace infinity {

class Catalog;
class EntryInterface;
class MetaInterface;
class BaseEntry;

export class CleanupInfoTracer {
public:
    CleanupInfoTracer() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("CleanupInfoTracer");
#endif
    }

    ~CleanupInfoTracer() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("CleanupInfoTracer");
#endif
    }

    void ResetInfo(TxnTimeStamp cleanup_ts) {
        cleanup_ts_ = cleanup_ts;
        cleanup_info_.clear();
    }

    void AddCleanupInfo(String path) { cleanup_info_.emplace_back(std::move(path)); }

    String GetCleanupInfo() const;

private:
    TxnTimeStamp cleanup_ts_ = 0;

    Vector<String> cleanup_info_;
};

export class CleanupScanner {
public:
    CleanupScanner(Catalog *catalog, TxnTimeStamp visible_ts, BufferManager *buffer_mgr);

    void Scan();

    void Cleanup(CleanupInfoTracer *info_tracer = nullptr) &&;

    void AddEntry(SharedPtr<BaseEntry> entry, bool dropped = true);

    TxnTimeStamp visible_ts() const { return visible_ts_; }

    static void CleanupDir(const String &abs_dir);

private:
    Catalog *const catalog_;

    const TxnTimeStamp visible_ts_;

    BufferManager *buffer_mgr_;

    Vector<Pair<SharedPtr<BaseEntry>, bool>> entries_;
};

} // namespace infinity
