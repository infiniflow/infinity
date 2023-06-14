#pragma once

#include "common/utility/cache_base.h"
#include "tx.h"
#include "codec.h"

#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/write_batch.h>
#include <leveldb/filter_policy.h>
#include <roaring/roaring.hh>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>

namespace infinity {

using Roaring = roaring::Roaring;

class Status {
    enum Code {
        kOk = 0,
        kNotFound = 1,
        kCorruption = 2,
        kNotSupported = 3,
        kInvalidArgument = 4,
        kIOError = 5
    };

    Code code_;

public:

    Code code() const {
        return code_;
    }

    Status(Code code) {
        code_ = code;
    }

    // Return a success status.
    static Status OK() {
        return Status(kOk);
    }

    // Return error status of an appropriate type.
    static Status NotFound() {
        return Status(kNotFound);
    }

    static Status Corruption() {
        return Status(kCorruption);
    }

    static Status NotSupported() {
        return Status(kNotSupported);
    }

    static Status InvalidArgument() {
        return Status(kInvalidArgument);
    }

    static Status IOError() {
        return Status(kIOError);
    }

    // Returns true iff the status indicates success.
    bool ok() const {
        return (code() == kOk);
    }

    // Returns true iff the status indicates a NotFound error.
    bool IsNotFound() const {
        return code() == kNotFound;
    }

    // Returns true iff the status indicates a Corruption error.
    bool IsCorruption() const {
        return code() == kCorruption;
    }

    // Returns true iff the status indicates an IOError.
    bool IsIOError() const {
        return code() == kIOError;
    }
};

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

    Tx* Begin();

private:
    leveldb::DB* db_;
    std::string path_;
    std::unique_ptr<PostingCache> cache_;
    Codec codec_;
    leveldb::WriteBatch batch_;
};

}