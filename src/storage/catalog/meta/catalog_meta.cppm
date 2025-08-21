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

export module infinity_core:catalog_meta;

import :status;

namespace infinity {

class KVInstance;
class NewTxn;
class MetaCache;

export class CatalogMeta {
public:
    CatalogMeta(NewTxn *txn);
    CatalogMeta(KVInstance *kv_instance, MetaCache *meta_cache);

    Status GetDBID(const std::string &db_name, std::string &db_key, std::string &db_id, TxnTimeStamp &create_ts);

    Status GetDBIDs(std::vector<std::string> *&db_id_strs, std::vector<std::string> **db_names = nullptr);

private:
    Status LoadDBIDs();

private:
    NewTxn *txn_{};
    TxnTimeStamp read_ts_{};
    KVInstance *kv_instance_{};
    MetaCache *meta_cache_{};

    std::optional<std::vector<std::string>> db_id_strs_;
    std::optional<std::vector<std::string>> db_names_;
};

} // namespace infinity
