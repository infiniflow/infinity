// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module catalog_meta;

import stl;
import status;

namespace infinity {

class KVInstance;

export class CatalogMeta {
public:
    CatalogMeta(KVInstance &kv_instance);

    Status GetDBID(const String &db_name, String &db_key, String &db_id);

    Status GetDBIDs(Vector<String> *&db_id_strs, Vector<String> **db_names = nullptr) {
        if (!db_id_strs_ || !db_names_) {
            Status status = LoadDBIDs();
            if (!status.ok()) {
                return status;
            }
        }
        db_id_strs = &*db_id_strs_;
        if (db_names) {
            *db_names = &*db_names_;
        }
        return Status::OK();
    }

private:
    Status LoadDBIDs();

private:
    KVInstance &kv_instance_;

    Optional<Vector<String>> db_id_strs_;
    Optional<Vector<String>> db_names_;
};

} // namespace infinity
