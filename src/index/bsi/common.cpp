#include "common.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

namespace infinity {

void RoaringBitmap::SetCRoaringBitmap(roaring_bitmap_t* b) {
    CRoaringBitmapPtr tmp(b);
    bitmap = std::move(tmp);
}
void RoaringBitmap::NewCRoaringBitmap() {
    roaring_bitmap_t* b = roaring_bitmap_create();
    roaring_bitmap_init_cleared(b);
    // roaring_bitmap_set_copy_on_write(b, true);
    CRoaringBitmapPtr tmp(b);
    bitmap = std::move(tmp);
}
bool RoaringBitmap::Put(uint32_t id) {
    return roaring_bitmap_add_checked(bitmap.get(), id);
}
bool RoaringBitmap::Remove(uint32_t id) {
    return roaring_bitmap_remove_checked(bitmap.get(), id);
}
int RoaringBitmap::Save(FILE* fp, bool readonly) {
    // off_t cur = ::lseek(fd, 0, SEEK_CUR);
    roaring_bitmap_run_optimize(bitmap.get());
    roaring_bitmap_shrink_to_fit(bitmap.get());
    char* mbuf = nullptr;
    size_t data_len = 0;
    if (readonly) {
        size_t nbytes = roaring_bitmap_frozen_size_in_bytes(bitmap.get());
        mbuf = (char*)malloc(nbytes);
        roaring_bitmap_frozen_serialize(bitmap.get(), mbuf);
        data_len = nbytes;
    } else {
        size_t nbytes = roaring_bitmap_size_in_bytes(bitmap.get());
        mbuf = (char*)malloc(nbytes);
        data_len = roaring_bitmap_serialize(bitmap.get(), mbuf);
    }
    int rc = file_write_uint32(fp, data_len);
    if (0 != rc) {
        free(mbuf);
        return rc;
    }
    rc = fwrite(&readonly, sizeof(bool), 1, fp);
    if (rc != 1) {
        free(mbuf);
        return -1;
    }
    rc = fwrite(mbuf, data_len, 1, fp);
    if (rc != 1) {
        free(mbuf);
        return -1;
    }
    free(mbuf);
    return 0;
}

int RoaringBitmap::Load(FILE* fp) {
    uint32_t n = 0;
    int rc = file_read_uint32(fp, n);
    if (0 != rc) {
        return -1;
    }
    bool readonly = false;
    rc = fread(&readonly, sizeof(bool), 1, fp);
    if (rc != 1) {
        return -1;
    }
    readonly_ = readonly;
    char* mbuf = (char*)(::aligned_alloc(32, n));
    rc = fread(mbuf, n, 1, fp);
    if (rc != 1) {
        std::free(mbuf);
        return -1;
    }
    if (!readonly) {
        CRoaringBitmapPtr tmp(roaring_bitmap_deserialize(mbuf));
        bitmap = std::move(tmp);
        std::free(mbuf);
    } else {
        const roaring_bitmap_t* b = roaring_bitmap_frozen_view(mbuf, n);
        if (nullptr == b) {
            std::cout<<"roaring_bitmap_frozen_view failed with {"<<n<<"}"<<std::endl;
        }
        CRoaringBitmapDeleter deleter(true);
        CRoaringBitmapPtr tmp((const_cast<roaring_bitmap_t*>(b)), deleter);
        bitmap = std::move(tmp);
        underly_buf_ = mbuf;
    }
    if (!bitmap) {
        return -1;
    }
    return 0;
}

RoaringBitmap::~RoaringBitmap() {
    if (nullptr != underly_buf_) {
        std::free(underly_buf_);
    }
}

void bitmap_extract_ids(const roaring_bitmap_t* out, std::vector<uint32_t>& ids) {
    uint32_t n = roaring_bitmap_get_cardinality(out);
    ids.resize(n);
    roaring_bitmap_to_uint32_array(out, &ids[0]);
}

int file_write_uint32(FILE* fp, uint32_t n) {
    n = htonl(n);
    int rc = fwrite(&n, sizeof(n), 1, fp);
    if (rc != 1) {
        return -1;
    }
    return 0;
}

int file_read_uint32(FILE* fp, uint32_t& n) {
    int rc = fread(&n, sizeof(n), 1, fp);
    if (rc != 1) {
        return -1;
    }
    n = ntohl(n);
    return 0;
}

}  // namespace infinity
