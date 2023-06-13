#pragma once

#include "common/utility/cache_base.h"

#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/write_batch.h>
#include <leveldb/filter_policy.h>
#include <roaring/roaring.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace infinity {

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


class PostingCache : public CacheBase<String, roaring_bitmap_t>{};

class IndexKV {
public:
    IndexKV(const std::string& path):db_(NULL),path_(path) {
    }

    ~IndexKV() {
        delete db_;
    }

    Status Open();
private:
    leveldb::DB* db_;
    std::string path_;
    leveldb::WriteBatch batch_;
};
}