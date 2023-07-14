#pragma once

#include "page_manager.h"
#include "key_page_map.h"
#include <leveldb/status.h>
#include <leveldb/slice.h>
#include <roaring/roaring.hh>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <string>

namespace infinity{

using Status = leveldb::Status;
using Slice = leveldb::Slice;
using Roaring = roaring::Roaring;

class NumericDB{
public:
    NumericDB(std::filesystem::path db_path);
    ~NumericDB();

    Status Put(const Slice& key, const Slice& value);

    Status Get(const Slice& key, std::string& value);

    Status GetRange(const Slice& start_key, const Slice& end_key, std::unique_ptr<Roaring>& filter);

    Status Delete(const Slice& key);
private:
    void Initialize();

    const std::filesystem::path db_path_;

    std::shared_ptr<PageManager> page_mgr_;

    std::shared_ptr<KeyPageMap> key_page_map_;
};
}