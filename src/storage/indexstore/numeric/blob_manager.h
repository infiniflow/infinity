// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "page.h"
#include "page_manager.h"
#include "btree_types.h"
#include "context.h"

namespace infinity {
// A blob header structure
//
// This header is prepended to the blob's payload. It holds the blob size and
// the blob's address (which is not required but useful for error checking.)
struct BlobHeader {
    enum {
        // Blob is compressed
        kIsCompressed = 1
    };

    BlobHeader() {
        ::memset(this, 0, sizeof(BlobHeader));
    }

    // The blob id - which is the absolute address/offset of this
    // structure in the file
    uint64_t blob_id_;

    // Flags; store compression information
    uint32_t flags_;

    // The allocated size of the blob; this is the size, which is used
    // by the blob and it's header and maybe additional padding
    uint32_t allocated_size_;

    // The size of the blob from the user's point of view (excluding the header)
    uint32_t size_;

} __attribute__ ((packed));


struct BlobPageHeader {
    enum {
        // Freelist entries
        kFreelistLength = 32
    };

    void Initialize() {
        ::memset(this, 0, sizeof(BlobPageHeader));
    }

    // Returns a BlobPageHeader from a page
    static BlobPageHeader *FromPage(Page *page) {
        return (BlobPageHeader *)&page->Payload()[0];
    }

    // Number of "regular" pages for this blob; used for blobs exceeding
    // a page size
    uint32_t num_pages_;

    // Number of free bytes in this page
    uint32_t free_bytes_;

    // The freelist - offset/size pairs in this page
    struct FreelistEntry {
        uint32_t offset_;
        uint32_t size_;
    } freelist_[kFreelistLength];
} __attribute__ ((packed));


struct BlobManager {
    // A parameter for overwrite_regions()
    struct Region {
        Region() {
        }

        Region(uint32_t offset, uint32_t size)
            : offset_(offset), size_(size) {
        }

        uint32_t offset_;
        uint32_t size_;
    };

    // Flags for allocate(); make sure that they do not conflict with
    // the flags for ups_db_insert()
    enum {
        // Do not compress the blob, even if compression is enabled
        kDisableCompression = 0x10000000
    };

    enum {
        // Overhead per page
        kPageOverhead = Page::kSizeofPersistentHeader + sizeof(BlobPageHeader)
    };


    BlobManager(PageManager *page_manager, File *file)
        : page_manager_(page_manager), file_(file) {
    }


    // allocate/create a blob
    // returns the blob-id (the start address of the blob header)
    uint64_t Allocate(Context *context, btree_record_t *record, uint32_t flags);

    // reads a blob and stores the data in |record|. The pointer |record.data|
    // is backed by the |arena|, unless |UPS_RECORD_USER_ALLOC| is set.
    // flags: either 0 or UPS_DIRECT_ACCESS
    void Read(Context *context, uint64_t blobid, btree_record_t *record, uint32_t flags, ByteArray *arena);

    // retrieves the size of a blob
    uint32_t BlobSize(Context *context, uint64_t blobid);

    // overwrite an existing blob
    //
    // will return an error if the blob does not exist
    // returns the blob-id (the start address of the blob header) in |blobid|
    uint64_t Overwrite(Context *context, uint64_t old_blobid, btree_record_t *record, uint32_t flags);

    // Overwrites regions of an existing blob
    //
    // Will return an error if the blob does not exist. Returns the blob-id
    // (the start address of the blob header)
    uint64_t OverwriteRegions(Context *context, uint64_t old_blob_id,
                              btree_record_t *record, uint32_t flags,
                              Region *regions, size_t num_regions);

    // delete an existing blob
    void Erase(Context *context, uint64_t blobid, Page *page = 0, uint32_t flags = 0);


    // The active page manager - required to allocate and fetch pages
    PageManager *page_manager_;

    // The device - sometimes it's accessed directly
    File *file_;

};


}