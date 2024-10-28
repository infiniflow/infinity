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

#include <vector>
#include <filesystem>

module persist_result_handler;

import infinity_exception;
import third_party;
import virtual_store;
import infinity_context;
import peer_task;
import logger;
import admin_statement;

namespace fs = std::filesystem;

namespace infinity {

void PersistResultHandler::HandleWriteResult(const PersistWriteResult &result) {
    for (const String &persist_key : result.persist_keys_) {
        String persist_path = pm_->GetObjPath(persist_key);
        if(InfinityContext::instance().GetServerRole() == NodeRole::kLeader or
           InfinityContext::instance().GetServerRole() == NodeRole::kStandalone){
            VirtualStore::UploadObject(persist_path, persist_key);
        }
    }
    for (const String &drop_key : result.drop_keys_) {
        String drop_path = pm_->GetObjPath(drop_key);
        fs::remove(drop_path);
    }
    for (const String &drop_key : result.drop_from_remote_keys_) {
        String drop_path = pm_->GetObjPath(drop_key);
        fs::remove(drop_path);
        if(InfinityContext::instance().GetServerRole() == NodeRole::kLeader or
           InfinityContext::instance().GetServerRole() == NodeRole::kStandalone){
            VirtualStore::RemoveObject(drop_key);
        }
    }
}

ObjAddr PersistResultHandler::HandleReadResult(const PersistReadResult &result) {
    if (!result.cached_) {
        String read_path = InfinityContext::instance().persistence_manager()->GetObjPath(result.obj_addr_.obj_key_);
        VirtualStore::DownloadObject(read_path, result.obj_addr_.obj_key_);
        LOG_TRACE(fmt::format("GetObjCache download object {}", read_path));
        result.obj_stat_->cached_ = true;
    }
    return result.obj_addr_;
}

}
