/*
 * zsv_strencode(): standalone file to allow zsv utilities that use this
 * to be used on a standalone basis without the zsv parser
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#include <zsv/utils/utf8.h>
#include <zsv/utils/compiler.h>

/**
 * Ensure valid UTF8 encoding by, if needed, replacing malformed bytes
 */
ZSV_EXPORT
size_t zsv_strencode(unsigned char *s, size_t n, unsigned char replace,
                     int (*malformed_handler)(void *, const unsigned char *s, size_t n, size_t offset),
                     void *handler_ctx) {
  size_t new_len = 0;
  int clen;
  for (size_t i2 = 0; i2 < n; i2 += (size_t)clen) {
    clen = ZSV_UTF8_CHARLEN(s[i2]);
    if (LIKELY(clen == 1))
      s[new_len++] = s[i2];
    else if (UNLIKELY(clen < 0) || UNLIKELY(i2 + clen > n)) {
      if (malformed_handler)
        malformed_handler(handler_ctx, s, n, new_len);
      if (replace)
        s[new_len++] = replace;
      clen = 1;
    } else { /* might be valid multi-byte utf8; check */
      unsigned char valid_n;
      for (valid_n = 1; valid_n < clen; valid_n++)
        if (!ZSV_UTF8_SUBSEQUENT_CHAR_OK(s[i2 + valid_n]))
          break;
      if (valid_n == clen) { /* valid_n utf8; copy it */
        memmove(s + new_len, s + i2, clen);
        new_len += clen;
      } else { /* invalid; valid_n smaller than expected */
        if (malformed_handler)
          malformed_handler(handler_ctx, s, n, new_len);
        if (replace) {
          memset(s + new_len, replace, valid_n);
          new_len += valid_n;
        }
        clen = valid_n;
      }
    }
  }
  return new_len; // new length
}
