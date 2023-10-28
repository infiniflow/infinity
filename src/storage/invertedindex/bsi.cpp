module;

import stl;

#include <roaring/roaring.hh>

module bsi;

namespace infinity {

u32 float_to_u32(float v) {
    u32 iv = 0;
    memcpy(&iv, &v, sizeof(v));
    if (v >= 0) {
        iv |= (0x80000000);
    } else {
        iv = (~iv);
    }
    return iv;
}

float u32_to_float(u32 v) {
    if ((v & 0x80000000) > 0) {
        v &= (~0x80000000);
    } else {
        v = (~v);
    }
    float d;
    memcpy(&d, &v, sizeof(v));
    return d;
}

u64 double_to_u64(double v) {
    u64 iv = 0;
    memcpy(&iv, &v, sizeof(v));
    if (v >= 0) {
        iv |= (0x8000000000000000ull);
    } else {
        iv = (~iv);
    }
    return iv;
}

double u64_to_double(u64 v) {
    if ((v & 0x8000000000000000ull) > 0) {
        v &= (~0x8000000000000000ull);
    } else {
        v = (~v);
    }
    double d;
    memcpy(&d, &v, sizeof(v));
    return d;
}

BitSlicedIndex::BitSlicedIndex(u64 schema_id, u64 table_id, u64 column_id) : schema_id_(schema_id), table_id_(table_id), column_id_(column_id) {}

BitSlicedIndex::~BitSlicedIndex() {}

bool BitSlicedIndex::DoPut(u32 id, u64 val, u64 &old_val) {
    bool inserted = bitmaps_[0]->addChecked(id); // set Not NULL bitmap
    if (inserted) {
        u64 set_val = val;
        for (i8 i = 0; i < bit_depth_; i++) {
            if (set_val & (1ull << i)) {
                bitmaps_[i + 1]->add(id);
            } else {
                bitmaps_[i + 1]->remove(id);
            }
        }
        return false;
    } else {
        old_val = 0;
        u64 set_val = val;
        for (i8 i = 0; i < bit_depth_; i++) {
            if (bitmaps_[1 + i]->contains(id)) {
                old_val += (1ull << i);
            }
            if (set_val & (1ull << i)) {
                bitmaps_[1 + i]->add(id);
            } else {
                bitmaps_[1 + i]->remove(id);
            }
        }
        return true;
    }
}

void BitSlicedIndex::DoRemove(u32 id, u64 val) {
    bitmaps_[0]->remove(id);
    u64 set_val = val;
    for (i8 i = 0; i < bit_depth_; i++) {
        if (set_val & (1ull << i)) {
            bitmaps_[i + 1]->remove(id);
        }
    }
}

bool BitSlicedIndex::DoGet(u32 id, u64 &val) {
    if (!bitmaps_[0]->contains(id)) {
        return false;
    }
    val = 0;
    for (i8 i = 0; i < bit_depth_; i++) {
        if (bitmaps_[1 + i]->contains(id)) {
            val += (1ull << i);
        }
    }
    return true;
}

int BitSlicedIndex::DoRangeLT(u64 expect_val, bool allow_eq, UniquePtr<Roaring> &filter) {
    if (filter->isEmpty()) {
        (*filter) = (*bitmaps_[0]);
    } else {
        (*filter) &= (*bitmaps_[0]);
    }
    bool leading_zero = true;
    UniquePtr<Roaring> keep = MakeUnique<Roaring>();
    UniquePtr<Roaring> tmp = MakeUnique<Roaring>();
    for (i8 i = bit_depth_ - 1; i >= 0; i--) {
        u8 bit = (expect_val >> i) & 0x1;
        if (leading_zero) {
            if (bit == 0) {
                (*filter) &= (*bitmaps_[1 + i]);
                continue;
            } else {
                leading_zero = false;
            }
        }

        if (i == 0 && !allow_eq) {
            if (bit == 0) {
                (*filter) = (*keep);
                return 0;
            } else {
                (*keep) ^= (*bitmaps_[1 + i]);
                (*keep) -= (*bitmaps_[1 + i]);
                (*filter) -= (*keep);
                return 0;
            }
        }

        // If bit is zero then remove all set columns not in excluded bitmap.
        if (bit == 0) {
            (*tmp) = (*bitmaps_[1 + i]);
            (*tmp) -= (*keep);
            (*filter) -= (*tmp);
            continue;
        }

        // If bit is set then add columns for set bits to exclude.
        // Don't bother to compute this on the final iteration.
        if (i > 0) {
            (*tmp) = (*filter);
            (*tmp) -= (*bitmaps_[1 + i]);
            (*keep) |= (*tmp);
        }
    }
    return 0;
}

int BitSlicedIndex::DoRangeGT(u64 expect_val, bool allow_eq, UniquePtr<Roaring> &filter) {
    if (filter->isEmpty()) {
        (*filter) = (*bitmaps_[0]);
    } else {
        (*filter) &= (*bitmaps_[0]);
    }

    UniquePtr<Roaring> keep = MakeUnique<Roaring>();
    UniquePtr<Roaring> tmp = MakeUnique<Roaring>();
    for (i8 i = bit_depth_ - 1; i >= 0; i--) {
        u8 bit = (expect_val >> i) & 0x1;
        // Handle last bit differently.
        // If bit is one then return only already kept columns.
        // If bit is zero then remove any unset columns.
        if (i == 0 && !allow_eq) {
            if (bit == 1) {
                (*filter) = (*keep);
                return 0;
            }
            (*tmp) = (*filter);
            (*tmp) -= (*bitmaps_[1 + i]);
            (*tmp) -= (*keep);
            (*filter) -= (*tmp);
            return 0;
        }

        // If bit is set then remove all unset columns not already kept.
        if (bit == 1) {
            (*tmp) = (*filter);
            (*tmp) -= (*bitmaps_[1 + i]);
            (*tmp) -= (*keep);
            (*filter) -= (*tmp);
            continue;
        }
        // If bit is unset then add columns with set bit to keep.
        // Don't bother to compute this on the final iteration.
        if (i > 0) {
            (*tmp) = (*filter);
            (*tmp) &= (*bitmaps_[1 + i]);
            (*keep) |= (*tmp);
        }
    }
    return 0;
}

int BitSlicedIndex::DoRangeEQ(u64 expect_val, UniquePtr<Roaring> &filter) {
    if (filter->isEmpty()) {
        (*filter) = (*bitmaps_[0]);
    } else {
        (*filter) &= (*bitmaps_[0]);
    }
    for (int32_t i = bit_depth_ - 1; i >= 0; i--) {
        uint8_t bit = (expect_val >> i) & 0x1;
        if (bit) {
            (*filter) &= (*bitmaps_[1 + i]);
        } else {
            (*filter) -= (*bitmaps_[1 + i]);
        }
    }
    return 0;
}

int BitSlicedIndex::DoRangeNEQ(u64 expect, UniquePtr<Roaring> &filter) {
    (*filter) = (*bitmaps_[0]);
    UniquePtr<Roaring> eq = MakeUnique<Roaring>();
    DoRangeEQ(expect, eq);
    (*filter) -= (*eq);
    return 0;
}

int BitSlicedIndex::DoRangeBetween(u64 expect_min, u64 expect_max, UniquePtr<Roaring> &filter) {
    if (filter->isEmpty()) {
        (*filter) = (*bitmaps_[0]);
    } else {
        (*filter) &= (*bitmaps_[0]);
    }

    UniquePtr<Roaring> keep1 = MakeUnique<Roaring>();
    UniquePtr<Roaring> keep2 = MakeUnique<Roaring>();
    UniquePtr<Roaring> tmp = MakeUnique<Roaring>();

    for (i8 i = bit_depth_ - 1; i >= 0; i--) {
        auto row = bitmaps_[1 + i];
        u8 bit1 = (expect_min >> i) & 0x1;
        u8 bit2 = (expect_max >> i) & 0x1;
        // GTE predicateMin
        // If bit is set then remove all unset columns not already kept.
        if (bit1 == 1) {
            (*tmp) = (*filter);
            (*tmp) -= (*row);
            (*tmp) -= (*keep1);
            (*filter) -= (*tmp);
        } else {
            // If bit is unset then add columns with set bit to keep.
            // Don't bother to compute this on the final iteration.
            if (i > 0) {
                (*tmp) = (*filter);
                (*tmp) -= (*row);
                (*keep1) |= (*tmp);
            }
        }

        // LTE predicateMax
        // If bit is zero then remove all set bits not in excluded bitmap.
        if (bit2 == 0) {
            (*tmp) = (*row);
            (*tmp) -= (*keep2);
            (*filter) -= (*tmp);
        } else {
            // If bit is set then add columns for set bits to exclude.
            // Don't bother to compute this on the final iteration.
            if (i > 0) {
                (*tmp) = (*filter);
                (*tmp) -= (*row);
                (*keep2) |= (*tmp);
            }
        }
    }
    return 0;
}

} // namespace infinity