//
// Created by xwg on 23-10-16.
//

#pragma once

#include "storage/meta/catalog.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "wal_entry.h"
namespace infinity {

class TableCollectionEntry;
class Catalog;
class WalReplayState {
public:
    WalReplayState(Catalog *catalog, TableCollectionEntry *tableEntry, bool deserializeOnly);
    Catalog *catalog_{};
    i64 txn_id_{};
    i64 begin_ts_{};

    TableCollectionEntry *table_entry_{};
    bool deserialize_only_{};

public:
    static void ReplayCommand(WalCommandType cmd);

protected:
    void ReplayCreateDatabase();
    void ReplayCreateTable();

    void ReplayDropDatabase();
    void ReplayDropTable();
};

} // namespace infinity
