/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_UTF8_CHARLEN_NOERR
// ZSV_UTF8_CHARLEN_NOERR: return > 0 if valid utf8, +1 on error
#define ZSV_UTF8_CHARLEN_NOERR(c)                     \
  (!(c & 128) ? 1 :                                     \
   (c & 224) == 192 ? 2 : /* 110xxxxx */                \
   (c & 240) == 224 ? 3 : /* 1110xxxx */                \
   (c & 248) == 240 ? 4 : /* 11110xxx */                \
   (c & 252) == 248 ? 5 : /* 111110xx */                \
   (c & 254) == 252 ? 6 : /* 1111110x */                \
   1 /* error */                                        \
   )
#endif

#ifndef ZSV_UTF8_CHARLEN
// ZSV_UTF8_CHARLEN: return > 0 if valid utf8, -1 on error
#define ZSV_UTF8_CHARLEN(c)                           \
  (!(c & 128) ? 1 :                                     \
   (c & 224) == 192 ? 2 : /* 110xxxxx */                \
   (c & 240) == 224 ? 3 : /* 1110xxxx */                \
   (c & 248) == 240 ? 4 : /* 11110xxx */                \
   (c & 252) == 248 ? 5 : /* 111110xx */                \
   (c & 254) == 252 ? 6 : /* 1111110x */                 \
   -1 /* error */                                        \
   )
#endif

#ifndef ZSV_UTF8_SUBSEQUENT_CHAR_OK
#define ZSV_UTF8_SUBSEQUENT_CHAR_OK(c) ((c & 192) == 128)
#endif
