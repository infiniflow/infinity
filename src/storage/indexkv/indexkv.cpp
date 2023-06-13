#include "indexkv.h"
#include "common/utility/infinity_assert.h"

namespace infinity {
Status IndexKV::Open() {
    leveldb::Options options;

    //options.block_cache = leveldb::NewLRUCache(500 * 1048576);
    options.write_buffer_size = 500 * 1048576;
    options.filter_policy = leveldb::NewBloomFilterPolicy(16);
    options.create_if_missing = true;
    options.paranoid_checks = false;
    options.compression = leveldb::kSnappyCompression;

    leveldb::Status status = leveldb::DB::Open(options, path_, &db_);
    StorageAssert(status.ok(), "Failed to open leveldb database")

    return Status::OK();
}



}