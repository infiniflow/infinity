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

export module infinity_core:persist_result_handler;

import :stl;
import :persistence_manager;
import global_resource_usage;

namespace infinity {

export class PersistResultHandler {
public:
    PersistResultHandler() : pm_(nullptr) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PersistResultHandler");
#endif
    }

    PersistResultHandler(PersistenceManager *pm) : pm_(pm) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PersistResultHandler");
#endif
    }

    ~PersistResultHandler() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("PersistResultHandler");
#endif
    }

    void HandleWriteResult(const PersistWriteResult &result);

    ObjAddr HandleReadResult(const PersistReadResult &result);

private:
    PersistenceManager *pm_;
};

} // namespace infinity
