#pragma once

#include "common/utility/cache_base.h"
#include "tx.h"
#include "codec.h"

#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/write_batch.h>
#include <leveldb/filter_policy.h>
#include <leveldb/status.h>
#include <roaring/roaring.hh>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>

namespace infinity {

using Roaring = roaring::Roaring;
using Status = leveldb::Status;

struct RoaringWeightFunction{
    size_t operator()(const Roaring & r) const {
        return r.getSizeInBytes();
    }
};

class PostingCache 
: public CacheBase<String, Roaring, std::hash<String>, RoaringWeightFunction>{
public:
    PostingCache(size_t max_size):CacheBase(max_size){}
};

class IndexDB {
public:
    IndexDB(const std::string& path);

    ~IndexDB();

    Status Open();

    Status Get(const std::string& key, std::shared_ptr<Roaring>& result);

    Status Put(const std::string& key, std::shared_ptr<Roaring>& result);

    Status ScanForBSI(u64 column_id, std::vector<std::shared_ptr<Roaring>>& results);

    Tx* Begin();

private:
    leveldb::DB* db_;
    leveldb::Options options_;
    std::string path_;
    std::unique_ptr<PostingCache> cache_;
    Codec codec_;
    leveldb::WriteBatch batch_;
};

}