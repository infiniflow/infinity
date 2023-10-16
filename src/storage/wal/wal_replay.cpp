//
// Created by xwg on 23-10-16.
//

#include "wal_replay.h"

namespace infinity {
WalReplayState::WalReplayState(Catalog *catalog, TableCollectionEntry *tableEntry, bool deserializeOnly)
    : catalog_(catalog), table_entry_(tableEntry), deserialize_only_(deserializeOnly) {}

void WalReplayState::ReplayCommand(WalCommandType cmd_type) {
    switch (cmd_type) {
        case WalCommandType::CREATE_DATABASE:
            break;
        case WalCommandType::DROP_DATABASE:
            break;
        case WalCommandType::CREATE_TABLE:
            break;
        case WalCommandType::DROP_TABLE:
            break;
        case WalCommandType::ALTER_INFO:
            break;
        case WalCommandType::CREATE_INDEX:
            break;
        case WalCommandType::IMPORT:
            break;
        case WalCommandType::APPEND:
            break;
        case WalCommandType::DELETE:
            break;
        case WalCommandType::CHECKPOINT:
            break;
        default:
            StorageError("Not supported wal command type");
    }
}
void WalReplayState::ReplayCreateTable() {}
void WalReplayState::ReplayCreateDatabase() {}

} // namespace infinity