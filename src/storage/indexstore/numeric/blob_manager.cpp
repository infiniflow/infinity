#include "blob_manager.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

static bool
CheckIntegrity(BlobManager *dbm, BlobPageHeader *header) {
    assert(header->num_pages_ > 0);

    if (header->free_bytes_ + BlobManager::kPageOverhead
            > Page::kSize * header->num_pages_) {
        return false;
    }

    // freelist is not used if this is a multi-page blob
    if (header->num_pages_ > 1)
        return true;

    uint32_t total_sizes = 0;
    typedef std::pair<uint32_t, uint32_t> Range;
    typedef std::vector<Range> RangeVec;
    RangeVec ranges;

    for (uint32_t i = 0; i < BlobPageHeader::kFreelistLength - 1; i++) {
        const auto& entry = header->freelist_[i];
        if (entry.size_ == 0) {
            assert(entry.offset_ == 0);
            continue;
        }
        total_sizes += entry.size_;
        ranges.emplace_back(entry.offset_, entry.size_);
    }

    // the sum of freelist chunks must not exceed total number of free bytes
    if (total_sizes > header->free_bytes_) {
        return false;
    }

    std::sort(ranges.begin(), ranges.end());

    if (!ranges.empty()) {
        for (uint32_t i = 0; i < ranges.size() - 1; i++) {
            if (ranges[i].first + ranges[i].second
                    > Page::kSize * header->num_pages_) {
                return false;
            }
            if (ranges[i].first + ranges[i].second > ranges[i + 1].first) {
                return false;
            }
        }
    }

    return true;
}

static void
AddToFreelist(BlobManager *dbm, BlobPageHeader *header,
              uint32_t offset, uint32_t size) {
    assert(CheckIntegrity(dbm, header));

    // freelist is not used if this is a multi-page blob
    if (header->num_pages_ > 1)
        return;

    // first try to collapse the blobs
    for (uint32_t i = 0; i < BlobPageHeader::kFreelistLength; i++) {
        if (offset + size == header->freelist_[i].offset_) {
            header->freelist_[i].offset_ = offset;
            header->freelist_[i].size_ += size;
            assert(CheckIntegrity(dbm, header));
            return;
        }
        if (header->freelist_[i].offset_ + header->freelist_[i].size_ == offset) {
            header->freelist_[i].size_ += size;
            assert(CheckIntegrity(dbm, header));
            return;
        }
    }

    // otherwise store the blob in a new slot, if available
    uint32_t smallest = 0;
    for (uint32_t i = 0; i < BlobPageHeader::kFreelistLength; i++) {
        // slot is empty
        if (header->freelist_[i].size_ == 0) {
            header->freelist_[i].offset_ = offset;
            header->freelist_[i].size_ = size;
            assert(CheckIntegrity(dbm, header));
            return;
        }
        // otherwise look for the smallest entry
        if (header->freelist_[i].size_ < header->freelist_[smallest].size_) {
            smallest = i;
            continue;
        }
    }

    // overwrite the smallest entry?
    if (size > header->freelist_[smallest].size_) {
        header->freelist_[smallest].offset_ = offset;
        header->freelist_[smallest].size_ = size;
    }

    assert(CheckIntegrity(dbm, header));
}

static bool
AllocFromFreelist(BlobManager *dbm, BlobPageHeader *header,
                  uint32_t size, uint64_t *poffset) {
    assert(CheckIntegrity(dbm, header));

    // freelist is not used if this is a multi-page blob
    if (header->num_pages_ > 1)
        return false;

    for (uint32_t i = 0; i < BlobPageHeader::kFreelistLength; i++) {
        // exact match
        if (header->freelist_[i].size_ == size) {
            *poffset = header->freelist_[i].offset_;
            header->freelist_[i].offset_ = 0;
            header->freelist_[i].size_ = 0;
            assert(CheckIntegrity(dbm, header));
            return true;
        }
        // space in freelist is larger than what we need? return this space,
        // make sure the remaining gap stays in the freelist
        if (header->freelist_[i].size_ > size) {
            *poffset = header->freelist_[i].offset_;
            header->freelist_[i].offset_ = (uint32_t)(*poffset + size);
            header->freelist_[i].size_ -= size;
            assert(CheckIntegrity(dbm, header));
            return true;
        }
    }

    // there was no gap large enough for the blob
    return false;
}

static uint8_t *
ReadChunk(BlobManager *dbm, Context *context, Page *page, Page **ppage,
          uint64_t address, bool fetch_read_only, bool mapped_pointer) {
    // get the page-id from this chunk
    uint32_t page_size = Page::kSize;
    uint64_t pageid = address - (address % page_size);

    // is this the current page? if yes then continue working with this page,
    // otherwise Fetch the page
    if (page && page->Address() != pageid)
        page = 0;

    uint8_t *data;

    if (unlikely(!page)) {
        uint32_t flags = 0;
        if (fetch_read_only)
            flags |= PageManager::kReadOnly;
        if (mapped_pointer)
            flags |= PageManager::kOnlyFromCache;
        page = dbm->page_manager_->Fetch(context, pageid, flags);
        if (ppage)
            *ppage = page;
        if (page)
            data = page->RawPayload();
    } else
        data = page->RawPayload();

    uint32_t read_start = (uint32_t)(address - page->Address());
    return &data[read_start];
}

static void
CopyChunk(BlobManager *dbm, Context *context, Page *page, Page **ppage,
          uint64_t address, uint8_t *data, uint32_t size,
          bool fetch_read_only) {
    uint32_t page_size = Page::kSize;
    bool first_page = true;

    while (size) {
        // get the page-id from this chunk
        uint64_t pageid = address - (address % page_size);

        // is this the current page? if yes then continue working with this page,
        // otherwise Fetch the page
        if (page && page->Address() != pageid)
            page = 0;

        if (!page) {
            uint32_t flags = 0;
            if (fetch_read_only)
                flags |= PageManager::kReadOnly;
            if (!first_page)
                flags |= PageManager::kNoHeader;
            page = dbm->page_manager_->Fetch(context, pageid, flags);
        }

        // now read the data from the page
        uint32_t read_start = (uint32_t)(address - page->Address());
        uint32_t read_size = (uint32_t)(page_size - read_start);
        if (read_size > size)
            read_size = size;
        ::memcpy(data, &page->RawPayload()[read_start], read_size);
        address += read_size;
        data += read_size;
        size -= read_size;

        first_page = false;
    }

    if (ppage)
        *ppage = page;
}

static void
WriteChunks(BlobManager *dbm, Context *context, Page *page,
            uint64_t address, uint8_t **chunk_data, uint32_t *chunk_size,
            uint32_t chunks) {
    uint32_t page_size = Page::kSize;

    // for each chunk...
    for (uint32_t i = 0; i < chunks; i++) {
        uint32_t size = chunk_size[i];
        uint8_t *data = chunk_data[i];

        while (size) {
            // get the page-id from this chunk
            uint64_t pageid = address - (address % page_size);

            // is this the current page? if yes then continue working with this page,
            // otherwise Fetch the page
            if (page && page->Address() != pageid)
                page = 0;
            if (!page)
                page = dbm->page_manager_->Fetch(context, pageid, PageManager::kNoHeader);

            uint32_t write_start = (uint32_t)(address - page->Address());
            uint32_t write_size = (uint32_t)(page_size - write_start);

            // now write the data
            if (write_size > size)
                write_size = size;
            ::memmove(&page->RawPayload()[write_start], data, write_size);
            page->SetDirty(true);
            address += write_size;
            data += write_size;
            size -= write_size;
        }
    }
}

uint64_t
BlobManager::Allocate(Context *context, btree_record_t *record, uint32_t flags) {
    uint8_t *chunk_data[2];
    uint32_t chunk_size[2];
    uint32_t page_size = Page::kSize;

    void *record_data = record->data_;
    uint32_t record_size = record->size_;
    uint32_t original_size = record->size_;

    BlobHeader blob_header;
    uint32_t alloc_size = sizeof(BlobHeader) + record_size;

    // first check if we can add another blob to the last used page
    Page *page = page_manager_->LastBlobPage(context);

    BlobPageHeader *header = 0;
    uint64_t address = 0;
    if (page) {
        header = BlobPageHeader::FromPage(page);
        // allocate space for the blob
        if (!AllocFromFreelist(this, header, alloc_size, &address))
            page = 0;
        else
            address += page->Address();
    }

    if (!address) {
        // Allocate a new page. If the blob exceeds a page then allocate multiple
        // pages that are directly next to each other.
        uint32_t required_size = alloc_size + kPageOverhead;
        uint32_t num_pages = required_size / page_size;
        if (num_pages * page_size < required_size)
            num_pages++;

        // |page| now points to the first page that was allocated, and
        // the only one which has a header and a freelist
        page = page_manager_->AllocMultipleBlobPages(context, num_pages);
        assert(page->IsWithoutHeader() == false);

        // Initialize the BlobPageHeader
        header = BlobPageHeader::FromPage(page);
        header->Initialize();
        header->num_pages_ = num_pages;
        header->free_bytes_ = (num_pages * page_size) - kPageOverhead;

        // and move the remaining space to the freelist, unless we span multiple
        // pages (then the rest will be discarded) - TODO can we reuse it somehow?
        if (num_pages == 1
                && kPageOverhead + alloc_size > 0
                && header->free_bytes_ - alloc_size > 0) {
            header->freelist_[0].offset_ = kPageOverhead + alloc_size;
            header->freelist_[0].size_ = header->free_bytes_ - alloc_size;
        }

        address = page->Address() + kPageOverhead;
        assert(CheckIntegrity(this, header));
    }

    // addjust "free bytes" counter
    assert(header->free_bytes_ >= alloc_size);
    header->free_bytes_ -= alloc_size;

    // store the page id if it still has space left
    if (header->free_bytes_)
        page_manager_->SetLastBlobPage(page);
    else
        page_manager_->SetLastBlobPage(0);

    // Initialize the blob header
    blob_header.allocated_size_ = alloc_size;
    blob_header.size_ = record->size_;
    blob_header.blob_id_ = address;
    blob_header.flags_ = original_size != record_size
                         ? BlobHeader::kIsCompressed
                         : 0;

    chunk_data[0] = (uint8_t *)&blob_header;
    chunk_size[0] = sizeof(blob_header);
    chunk_data[1] = (uint8_t *)record_data;
    chunk_size[1] = record_size;

    WriteChunks(this, context, page, address, chunk_data, chunk_size, 2);
    address += chunk_size[0] + chunk_size[1];

    // store the blob_id; it will be returned to the caller
    uint64_t blob_id = blob_header.blob_id_;
    assert(CheckIntegrity(this, header));
    return blob_id;
}

void
BlobManager::Read(Context *context, uint64_t blob_id,
                  btree_record_t *record, uint32_t flags, ByteArray *arena) {
    // first step: read the blob header
    Page *page;
    BlobHeader *blob_header = (BlobHeader *)ReadChunk(this, context, 0, &page,
                              blob_id, true, false);

    // sanity check
    if (unlikely(blob_header->blob_id_ != blob_id)) {
        throw StorageException(fmt::format("blob {} not found", blob_id));
    }

    uint32_t blobsize = (uint32_t)blob_header->size_;
    record->size_ = blobsize;

    // empty blob?
    if (unlikely(!blobsize)) {
        record->data_ = 0;
        record->size_ = 0;
        return;
    }

    //if (NOTSET(record->flags_, UPS_RECORD_USER_ALLOC))
    {
        arena->resize(blobsize);
        record->data_ = arena->data();
    }

    CopyChunk(this, context, page, 0,
              blob_id + sizeof(BlobHeader),
              (uint8_t *)record->data_, blobsize, true);

}

uint32_t
BlobManager::BlobSize(Context *context, uint64_t blob_id) {
    // read the blob header
    BlobHeader *blob_header = (BlobHeader *)ReadChunk(this, context,
                              0, 0, blob_id, true, true);

    if (unlikely(blob_header->blob_id_ != blob_id))
        throw StorageException("blob not found");

    return blob_header->size_;
}

uint64_t
BlobManager::Overwrite(Context *context, uint64_t old_blobid,
                       btree_record_t *record, uint32_t flags) {
    BlobHeader *old_blob_header, new_blob_header;

    // This routine basically ignores compression. The likelyhood that a
    // compressed buffer has an identical size as the record that's overwritten,
    // is very small. In most cases this check will be false, and then
    // the record would be compressed again in do_allocate().
    //
    // As a consequence, the existing record is only overwritten if the
    // uncompressed record would fit in. Otherwise a new record is allocated,
    // and this one then is compressed.
    uint32_t alloc_size = sizeof(BlobHeader) + record->size_;

    // first, read the blob header; if the new blob fits into the
    // old blob, we overwrite the old blob (and add the remaining
    // space to the freelist, if there is any)
    Page *page;
    old_blob_header = (BlobHeader *)ReadChunk(this, context, 0, &page,
                      old_blobid, false, false);

    // sanity check
    if (unlikely(old_blob_header->blob_id_ != old_blobid))
        throw StorageException("blob not found");

    // now compare the sizes; does the new data fit in the old allocated
    // space?
    if (alloc_size <= old_blob_header->allocated_size_) {
        uint8_t *chunk_data[2];
        uint32_t chunk_size[2];

        // setup the new blob header
        new_blob_header.blob_id_ = old_blob_header->blob_id_;
        new_blob_header.size_ = record->size_;
        new_blob_header.allocated_size_ = alloc_size;
        new_blob_header.flags_ = 0; // disable compression, just in case...

        chunk_data[0] = (uint8_t *)&new_blob_header;
        chunk_size[0] = sizeof(new_blob_header);
        chunk_data[1] = (uint8_t *)record->data_;
        chunk_size[1] = record->size_;

        WriteChunks(this, context, page, new_blob_header.blob_id_,
                    chunk_data, chunk_size, 2);

        BlobPageHeader *header = BlobPageHeader::FromPage(page);

        // move remaining data to the freelist
        if (alloc_size < old_blob_header->allocated_size_) {
            BlobPageHeader *header = BlobPageHeader::FromPage(page);
            header->free_bytes_ += old_blob_header->allocated_size_ - alloc_size;
            AddToFreelist(this, header,
                          (uint32_t)(old_blobid + alloc_size) - page->Address(),
                          (uint32_t)old_blob_header->allocated_size_ - alloc_size);
        }

        // the old rid is the new rid
        return new_blob_header.blob_id_;
    }

    // if the new data is larger: allocate a fresh space for it
    // and discard the old; 'overwrite' has become (delete + insert) now.
    uint64_t new_blobid = Allocate(context, record, flags);
    Erase(context, old_blobid, 0, 0);

    return new_blobid;
}

uint64_t
BlobManager::OverwriteRegions(Context *context, uint64_t old_blob_id,
                              btree_record_t *record, uint32_t flags,
                              Region *regions, size_t num_regions) {
    assert(num_regions > 0);

    uint32_t page_size = Page::kSize;
    uint32_t alloc_size = sizeof(BlobHeader) + record->size_;

    // only one page is written? then don't bother updating the regions
    if (alloc_size < page_size)
        return Overwrite(context, old_blob_id, record, flags);

    // read the blob header
    Page *page;
    BlobHeader *blob_header = (BlobHeader *)ReadChunk(this, context, 0, &page,
                              old_blob_id, false, false);

    // sanity check
    if (unlikely(blob_header->blob_id_ != old_blob_id)) {
        throw StorageException(fmt::format("blob {} not found", old_blob_id));
    }

    BlobPageHeader *header = BlobPageHeader::FromPage(page);

    uint8_t *chunk_data[2];
    uint32_t chunk_size[2];

    uint64_t address = old_blob_id;

    // setup the new blob header
    int c = 0;
    if (alloc_size != blob_header->allocated_size_) {
        BlobHeader new_blob_header;
        new_blob_header.blob_id_ = blob_header->blob_id_;
        new_blob_header.size_ = record->size_;
        new_blob_header.allocated_size_ = alloc_size;
        new_blob_header.flags_ = 0; // disable compression, just in case...

        chunk_data[c] = (uint8_t *)&new_blob_header;
        chunk_size[c] = sizeof(new_blob_header);
        c++;
    }

    address += sizeof(BlobHeader);

    for (size_t i = 0; i < num_regions; i++) {
        chunk_data[c] = (uint8_t *)record->data_ + regions[i].offset_;
        chunk_size[c] = regions[i].size_;
        c++;

        WriteChunks(this, context, page, address + regions[i].offset_,
                    chunk_data, chunk_size, c);

        c = 0;
    }

    // move remaining data to the freelist
    if (alloc_size < blob_header->allocated_size_) {
        header->free_bytes_ += blob_header->allocated_size_ - alloc_size;
        AddToFreelist(this, header,
                      (uint32_t)(old_blob_id + alloc_size) - page->Address(),
                      (uint32_t)blob_header->allocated_size_ - alloc_size);
        page->SetDirty(true);
    }
    // the old rid is the new rid
    return old_blob_id;
}

void
BlobManager::Erase(Context *context, uint64_t blob_id, Page *page, uint32_t flags) {
    // Fetch the blob header
    BlobHeader *blob_header = (BlobHeader *)ReadChunk(this, context, 0, &page,
                              blob_id, false, false);

    if (unlikely(blob_header->blob_id_ != blob_id))
        throw StorageException("blob not found");

    // update the "free bytes" counter in the blob page header
    BlobPageHeader *header = BlobPageHeader::FromPage(page);
    header->free_bytes_ += blob_header->allocated_size_;

    // if the page is now completely empty (all blobs were erased) then move
    // it to the freelist
    if (header->free_bytes_ ==
            (header->num_pages_ * Page::kSize) - kPageOverhead) {
        page_manager_->SetLastBlobPage(0);
        page_manager_->Del(context, page, header->num_pages_);
        header->Initialize();
        return;
    }

    // otherwise move the blob to the freelist
    AddToFreelist(this, header, (uint32_t)(blob_id - page->Address()),
                  (uint32_t)blob_header->allocated_size_);
}


}