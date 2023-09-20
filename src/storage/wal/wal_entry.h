#pragma once

#include "common/types/internal_types.h"
#include <roaring/roaring.hh>
#include <cstdint>
namespace infinity {

class TableDef;
class DataBlock;

enum class WalCommandType : uint8_t {
	INVALID = 0,
	// -----------------------------
	// Catalog
	// -----------------------------
	CREATE_DATABASE = 1,
	DROP_DATABASE = 2,
	CREATE_TABLE = 3,
	DROP_TABLE = 4,
	ALTER_INFO = 5,

	// -----------------------------
	// Data
	// -----------------------------
	INSERT_DATABLOCK = 21,
	DELETE_ROWS = 22,
	// -----------------------------
	// Flush
	// -----------------------------
	CHECKPOINT = 99,
};


struct WalEntryHeader {
    int64_t lsn;  // each entry's lsn(Log Sequence Number) is strictly
                  // increasing by one.
    int32_t size; // size of payload, excluding the header, round to multi
                  // of 4. There's 4 bytes pad just after the payload storing
                  // the same value to assist backward iterating.
    uint32_t checksum; // crc32 of the entry, including the header and the
                       // payload. User shall populate it before writing to wal.
    int64_t txn_id;    // txn id of the entry, 0 if it contains CHECKPOINT command.
};


struct WalEntry : WalEntryHeader {
    Vector<String> dropped_databases_{};
    Vector<std::pair<String, String>> dropped_tables_{};
    Vector<String> created_databases_{};
    Vector<std::pair<String, SharedPtr<TableDef>>> created_tables_{};
    //TODO: create table
    //TODO: alter database, table
    HashMap<u64, SharedPtr<roaring::Roaring>> deleted_rows_{};
    Vector<SharedPtr<DataBlock>> blocks_{};
    int64_t max_lsn_{};
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    int32_t GetSizeInBytes() const ;
    // Write WalEntry to a char buffer, return the actual size in bytes.
    int32_t Write(char* buf, int32_t maxbytes) const;
    // Read a WalEntry from a serialized version, reading no more than maxbytes bytes, return the actual read bytes, -1 on failure.
    static int32_t Read(char* buf, int32_t maxbytes, WalEntry& entry);
};


} // namespace infinity
