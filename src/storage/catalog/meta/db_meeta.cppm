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

export module db_meeta;

import stl;
import status;
import new_catalog;

namespace infinity {

class KVInstance;
class DatabaseInfo;

export class DBMeeta {
public:
    DBMeeta(String db_id_str, KVInstance &kv_instance);

    const String &db_id_str() const { return db_id_str_; }

    KVInstance &kv_instance() const { return kv_instance_; }

    Status InitSet(const String *comment = nullptr);

    Status UninitSet(UsageFlag usage_flag);

    Status GetComment(String *&comment) {
        if (!comment_) {
            Status status = LoadComment();
            if (!status.ok()) {
                return status;
            }
        }
        comment = &*comment_;
        return Status::OK();
    }

    Status GetTableIDs(Vector<String> *&table_id_strs, Vector<String> **table_names = nullptr) {
        if (!table_id_strs_ || !table_names_) {
            Status status = LoadTableIDs();
            if (!status.ok()) {
                return status;
            }
        }
        table_id_strs = &*table_id_strs_;
        if (table_names) {
            *table_names = &*table_names_;
        }
        return Status::OK();
    }

    Status GetTableID(const String &table_name, String &table_key, String &table_id_str);

    Status GetDatabaseInfo(DatabaseInfo &db_info);

    Tuple<String, Status> GetNextTableID();

    Status SetNextTableID(const String &table_id_str);

private:
    Status LoadComment();

    Status LoadTableIDs();

    String GetDBTag(const String &tag) const;

private:
    String db_id_str_;
    KVInstance &kv_instance_;

    Optional<String> comment_;
    Optional<Vector<String>> table_id_strs_;
    Optional<Vector<String>> table_names_;
};

} // namespace infinity
