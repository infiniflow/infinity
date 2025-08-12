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

module infinity_core:persist_result_handler.impl;

import :persist_result_handler;
import :infinity_exception;
import :virtual_store;
import :infinity_context;
import :peer_task;
import :logger;
import :obj_status;

import std;
import third_party;

import admin_statement;

namespace infinity {

void PersistResultHandler::HandleWriteResult(const PersistWriteResult &result) {
    for (const String &persist_key : result.persist_keys_) {
        String persist_path = pm_->GetObjPath(persist_key);
        if (InfinityContext::instance().GetServerRole() == NodeRole::kLeader or
            InfinityContext::instance().GetServerRole() == NodeRole::kStandalone) {
            VirtualStore::UploadObject(persist_path, persist_key);
        }
    }
    for (const String &drop_key : result.drop_keys_) {
        String drop_path = pm_->GetObjPath(drop_key);
        VirtualStore::DeleteFileBG(drop_path);
    }
    for (const String &drop_key : result.drop_from_remote_keys_) {
        String drop_path = pm_->GetObjPath(drop_key);
        VirtualStore::DeleteFileBG(drop_path);
        if (InfinityContext::instance().GetServerRole() == NodeRole::kLeader or
            InfinityContext::instance().GetServerRole() == NodeRole::kStandalone) {
            VirtualStore::RemoveObject(drop_key);
        }
    }
}

ObjAddr PersistResultHandler::HandleReadResult(const PersistReadResult &result) {
    if (result.obj_stat_ != nullptr) {
        ObjCached expect = ObjCached::kNotCached;
        Atomic<ObjCached> &cached = result.obj_stat_->cached_;
        if (cached.compare_exchange_strong(expect, ObjCached::kDownloading)) {
            VirtualStore::AddRequestCount();
            String read_path = InfinityContext::instance().persistence_manager()->GetObjPath(result.obj_addr_.obj_key_);
            LOG_TRACE(fmt::format("GetObjCache download object {}.", read_path));
            VirtualStore::DownloadObject(read_path, result.obj_addr_.obj_key_);
            LOG_TRACE(fmt::format("GetObjCache download object {} done.", read_path));
            cached.store(ObjCached::kCached);
            cached.notify_all();
            VirtualStore::AddCacheMissCount();
        } else if (expect == ObjCached::kDownloading) {
            LOG_TRACE(fmt::format("GetObjCache waiting downloading object {}", result.obj_addr_.obj_key_));
            cached.wait(ObjCached::kDownloading);
            LOG_TRACE(fmt::format("GetObjCache finish waiting object {}", result.obj_addr_.obj_key_));
        } else {
            VirtualStore::AddRequestCount();
        }
    }
    return result.obj_addr_;
}

} // namespace infinity
