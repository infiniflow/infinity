/*
 * Copyright (C) 2021 Tai Chi Minh Ralph Eastwood (self), Matt Wong (Guarnerix Inc dba Liquidaty)
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if VECTOR_BYTES == 64 && (defined(HAVE__BLSR_U64) || defined(HAVE___BLSR_U64))
# if defined(HAVE__BLSR_U64)
#  define clear_lowest_bit(n) _blsr_u64(n)
# else
#  define clear_lowest_bit(n) __blsr_u64(n)
# endif
#elif VECTOR_BYTES == 32 && (defined(HAVE__BLSR_U32) || defined(HAVE___BLSR_U32))
# if defined(HAVE__BLSR_U32)
#  define clear_lowest_bit(n) _blsr_u32(n)
# else
#  define clear_lowest_bit(n) __blsr_u32(n)
# endif
#else
# define clear_lowest_bit(n) (n & (n - 1))
#endif

// vec_delims: return bitfield of next 32 bytes that contain at least 1 token
__attribute__((always_inline))
static inline int vec_delims(const unsigned char *s, size_t n,
                             zsv_uc_vector *char_match1,
                             zsv_uc_vector *char_match2,
                             zsv_uc_vector *char_match3,
                             zsv_uc_vector *char_match4,
                             zsv_mask_t *maskp
                             ) {
  zsv_uc_vector* pSrc1 = (zsv_uc_vector *)s;
  zsv_uc_vector str_simd;

  unsigned j = n / sizeof(str_simd); // VECTOR_BYTES;
  zsv_mask_t mask = 0;
  unsigned total_bytes = 0;

  for(unsigned i = 0; i < j; i++) {
    memcpy(&str_simd, s + i*sizeof(str_simd), sizeof(str_simd));
    zsv_uc_vector vtmp = str_simd == *char_match1;
    vtmp += (str_simd == *char_match2);
    vtmp += (str_simd == *char_match3);
    vtmp += (str_simd == *char_match4);
    mask = movemask_pseudo(vtmp);

    if(LIKELY(mask != 0)) { // check if we found one of the 4 chars
      *maskp = mask;
      return total_bytes;
    } else {
      // not found, moving to next chunk
      total_bytes += sizeof(*pSrc1);
    }
  }
  return total_bytes; // nothing found in entire buffer
}
