// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include <vector>

namespace infinity {

/** Operation completed successfully */
#define BTREE_SUCCESS                     (  0)
/** Invalid record size */
#define BTREE_INV_RECORD_SIZE             ( -2)
/** Invalid key size */
#define BTREE_INV_KEY_SIZE                ( -3)
/* deprecated */
#define BTREE_INV_KEYSIZE                 BTREE_INV_KEY_SIZE
/** Invalid page size (must be 1024 or a multiple of 2048) */
#define BTREE_INV_PAGE_SIZE               ( -4)
/* deprecated */
#define BTREE_INV_PAGESIZE                BTREE_INV_PAGE_SIZE
/** Memory allocation failed - out of memory */
#define BTREE_OUT_OF_MEMORY               ( -6)
/** Invalid function parameter */
#define BTREE_INV_PARAMETER               ( -8)
/** Invalid file header */
#define BTREE_INV_FILE_HEADER             ( -9)
/** Invalid file version */
#define BTREE_INV_FILE_VERSION            (-10)
/** Key was not found */
#define BTREE_KEY_NOT_FOUND               (-11)
/** Tried to insert a key which already exists */
#define BTREE_DUPLICATE_KEY               (-12)
/** Internal Database integrity violated */
#define BTREE_INTEGRITY_VIOLATED          (-13)
/** Internal upscaledb error */
#define BTREE_INTERNAL_ERROR              (-14)
/** Tried to modify the Database, but the file was opened as read-only */
#define BTREE_WRITE_PROTECTED             (-15)
/** Database record not found */
#define BTREE_BLOB_NOT_FOUND              (-16)
/** Generic file I/O error */
#define BTREE_IO_ERROR                    (-18)
/** Function is not yet implemented */
#define BTREE_NOT_IMPLEMENTED             (-20)
/** File not found */
#define BTREE_FILE_NOT_FOUND              (-21)
/** Operation would block */
#define BTREE_WOULD_BLOCK                 (-22)
/** Object was not initialized correctly */
#define BTREE_NOT_READY                   (-23)
/** Database limits reached */
#define BTREE_LIMITS_REACHED              (-24)
/** Object was already initialized */
#define BTREE_ALREADY_INITIALIZED         (-27)
/** Database needs recovery */
#define BTREE_NEED_RECOVERY               (-28)
/** Cursor must be closed prior to Txn abort/commit */
#define BTREE_CURSOR_STILL_OPEN           (-29)
/** Record filter or file filter not found */
#define BTREE_FILTER_NOT_FOUND            (-30)
/** Operation conflicts with another Txn */
#define BTREE_TXN_CONFLICT                (-31)
/* internal use: key was erased in a Txn */
#define BTREE_KEY_ERASED_IN_TXN           (-32)
/** Database cannot be closed because it is modified in a Txn */
#define BTREE_TXN_STILL_OPEN              (-33)
/** Cursor does not point to a valid item */
#define BTREE_CURSOR_IS_NIL               (-100)
/** Database not found */
#define BTREE_DATABASE_NOT_FOUND          (-200)
/** Database name already exists */
#define BTREE_DATABASE_ALREADY_EXISTS     (-201)
/** Database already open, or: Database handle is already initialized */
#define BTREE_DATABASE_ALREADY_OPEN       (-202)
/** Environment already open, or: Environment handle is already initialized */
#define BTREE_ENVIRONMENT_ALREADY_OPEN    (-203)
/** Invalid log file header */
#define BTREE_LOG_INV_FILE_HEADER         (-300)
/** Remote I/O error/Network error */
#define BTREE_NETWORK_ERROR               (-400)
/** UQI: plugin not found or unable to load */
#define BTREE_PLUGIN_NOT_FOUND            (-500)
/** UQI: failed to parse a query command */
#define BTREE_PARSER_ERROR                (-501)
/** UQI: a plugin with the given name is already registered */
#define BTREE_PLUGIN_ALREADY_EXISTS       (-502)

#define ISSET(f, b)       (((f) & (b)) == (b))
#define ISSETANY(f, b)    (((f) & (b)) != 0)
#define NOTSET(f, b)      (((f) & (b)) == 0)


//
// A helper class wrapping key-related constants into a common namespace.
// This class does not contain any logic.
//
struct BtreeKey {
    // persisted btree key flags; also used in combination with ups_key_t._flags
    enum {
        // key is extended with overflow area
        kExtendedKey          = 0x01,

        // key is compressed; the original size is stored in the payload
        kCompressed           = 0x08
    };

    // flags used with the ups_key_t::_flags (note the underscore - this
    // field is for INTERNAL USE!)
    //
    // Note: these flags should NOT overlap with the persisted flags above!
    //
    // As these flags NEVER will be persisted, they should be located outside
    // the range of a uint16_t, i.e. outside the mask 0x0000ffff.
    enum {
        // Actual key is lower than the requested key
        kLower               = 0x00010000,

        // Actual key is greater than the requested key
        kGreater             = 0x00020000,

        // Actual key is an "approximate match"
        kApproximate         = (kLower | kGreater)
    };
};

//
// A helper class wrapping record-related constants into a common namespace.
// This class does not contain any logic.
//
struct BtreeRecord {
    enum {
        // record size < 8; length is encoded at byte[7] of key->ptr
        kBlobSizeTiny         = 0x01,

        // record size == 8; record is stored in key->ptr
        kBlobSizeSmall        = 0x02,

        // record size == 0; key->ptr == 0
        kBlobSizeEmpty        = 0x04,

        // key has duplicates in an overflow area; this is the msb of 1 byte;
        // the lower bits are the counter for the inline duplicate list
        kExtendedDuplicates   = 0x80
    };
};

typedef struct {
    /** The size of the key, in bytes */
    uint16_t size_;

    /** The data of the key */
    void *data_;

    /** The key flags; */
    uint32_t flags_;

    /** For internal use */
    uint32_t _flags;

} btree_key_t;

typedef struct {
    /** The size of the record data, in bytes */
    uint32_t size_;

    /** Pointer to the record data */
    void *data_;

    /** The record flags; */
    uint32_t flags_;

} btree_record_t;

#define btree_make_record(PTR, SIZE) { SIZE, PTR, 0 }

typedef std::vector<uint8_t> ByteArray;

template<typename T>
struct ArrayView {
    ArrayView()
        : data_(0), size_(0) {
    }

    ArrayView(T *data, size_t size)
        : data_(data), size_(size) {
    }

    ArrayView(const ArrayView &other)
        : data_(other.data_), size_(other.size_) {
    }

    T &operator[](size_t index) {
        return data_[index];
    }

    const T &operator[](size_t index) const {
        return data_[index];
    }

    // Pointer to the data
    T *data_;

    // The number of elements in the array
    size_t size_;
};

}