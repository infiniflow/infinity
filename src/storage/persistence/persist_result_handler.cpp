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

namespace fs = std::filesystem;

namespace infinity {

void PersistResultHandler::HandleWriteResult(const PersistWriteResult &result) {
    for (const String &persist_key : result.persist_keys_) {
        String persist_path = pm_->GetObjPath(persist_key);
        VirtualStore::UploadObject(persist_path, persist_path);
    }
    for (const String &drop_key : result.drop_keys_) {
        String drop_path = pm_->GetObjPath(drop_key);
        fs::remove(drop_path);
    }
    for (const String &drop_key : result.drop_from_remote_keys_) {
        String drop_path = pm_->GetObjPath(drop_key);
        fs::remove(drop_path);
        VirtualStore::RemoveObject(drop_path);
    }
}

ObjAddr PersistResultHandler::HandleReadResult(const PersistReadResult &result) {
    if(!result.cached_ and result.obj_addr_.Valid()){
        String read_path = pm_->GetObjPath(result.obj_addr_.obj_key_);
        VirtualStore::DownloadObject(read_path, read_path);
    }
    if (!result.cached_) {
        UnrecoverableError(fmt::format("HandleReadResult: object {} is not cached", result.obj_addr_.obj_key_));
    }
    return result.obj_addr_;
}

}
