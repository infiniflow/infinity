#pragma once

#include "storage/table_def.h"
#include "btree_types.h"

#include <leveldb/status.h>
#include <roaring/roaring.hh>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace infinity {

using Status = leveldb::Status;
using Roaring = roaring::Roaring;

class EnvHeader;
class File;
class PageManager;
class Journal;
class BtreeIndex;
class NumericDB {
public:
    NumericDB(std::filesystem::path db_path, uint64_t segment_id);

    ~NumericDB();

    void Create(SharedPtr<TableDef> table_def);

    void Open(SharedPtr<TableDef> table_def);

    Status Put(const uint32_t column_id, const std::string& key, const uint32_t row_id);

    Status GetEqual(const uint32_t column_id, const std::string& key, std::shared_ptr<Roaring>& filter);

    Status GetRange(const uint32_t column_id, const std::string& start_key, const std::string& end_key, std::shared_ptr<Roaring>& filter);

    Status Delete(const uint32_t column_id, const std::string& key);
private:
    Status DoGetRange(const uint32_t column_id, btree_key_t* start_key, btree_key_t* end_key, std::shared_ptr<Roaring>& filter);

    void Initialize();

    void NewEnv();

    void OpenEnv();

    void Close();

    const std::filesystem::path db_path_;
    uint64_t segment_id_;
    std::shared_ptr<EnvHeader> header_;
    std::shared_ptr<File> file_;
    std::shared_ptr<PageManager> page_manager_;
    std::shared_ptr<Journal> journal_;
    LsnManager lsn_manager_;

    struct FastHash {
        template <typename T>
        std::size_t operator()(T t) const {
            return static_cast<std::size_t>(t);
        }
    };    
    std::unordered_map<uint32_t,std::shared_ptr<BtreeIndex>,FastHash> btree_indices_;
};
}