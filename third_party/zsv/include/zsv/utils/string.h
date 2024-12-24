/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ASCII_STRING_H
#define ASCII_STRING_H

#include "../zsv_export.h"
#include <stddef.h>
#include <stdint.h>

/*
 * zsv_strtolowercase(): convert to lower case. if built with utf8proc, converts unicode points
 *
 * @param s     string to convert
 * @param lenp  pointer to length of input string; will be set to length of output string
 *
 * @returns     newly-allocated string; caller must free()
 */
unsigned char *zsv_strtolowercase(const unsigned char *s, size_t *lenp);

const unsigned char *zsv_strstr(const unsigned char *hay, const unsigned char *needle);

/*
 * zsv_stricmp, zsv_strincmp(): case-insensitive comparison (unicode-compatible if built with utf8proc)
 * zsv_strincmp_ascii: ascii case-insensitive comparison
 *
 * @param   s1     string to convert
 * @param   len1   length of s1
 * @param   s2     string to convert
 * @param   len2   length of s2
 *
 * @returns 0 if the strings are equal, -1 if s1 < s2, else 1
 */
int zsv_stricmp(const unsigned char *s1, const unsigned char *s2);
int zsv_strincmp(const unsigned char *s1, size_t len1, const unsigned char *s2, size_t len2);
int zsv_strincmp_ascii(const unsigned char *s1, size_t len1, const unsigned char *s2, size_t len2);

#define ZSV_STRWHITE_FLAG_NO_EMBEDDED_NEWLINE 1
/**
 * zsv_strwhite(): convert consecutive white to single space
 *
 * @param s     string to convert
 * @param len   length of input string
 * @param flags bitfield of any of the following flags:
 *                ZSV_STRWHITE_FLAG_NO_EMBEDDED_NEWLINE
 */
size_t zsv_strwhite(unsigned char *s, size_t len, unsigned int flags);

/**
 * Force a string to conform to UTF8 encoding. Replaces any non-conforming utf8
 * with the specified char, or removes from the string (and shortens the string)
 * if replace = 0
 * @param  s        input string. invalid UTF8 bytes will be overwritten
 * @param  n        length (in bytes) of input
 * @param  replace  the character to replace any malformed UTF8 bytes with, or 0
 *                  to remove and shorten the result
 * @param  callback optional callback invoked upon scanning malformed UTF8
 * @param  ctx      context pointer passed to callback
 * @return          length of the valid string
 */
ZSV_EXPORT
size_t zsv_strencode(unsigned char *s, size_t n, unsigned char replace,
                     int (*malformed_handler)(void *, const unsigned char *s, size_t n, size_t offset),
                     void *handler_ctx);

size_t zsv_strip_trailing_zeros(const char *s, size_t len);

/**
 * Trim trailing whitespace
 * Returns the input string and new length
 */
const unsigned char *zsv_strtrim_right(const char unsigned *restrict s, size_t *lenp);

/**
 * Trim leading whitespace
 * Returns the pointer to the new string start and new length
 */
const unsigned char *zsv_strtrim_left(const char unsigned *restrict s, size_t *lenp);

/**
 * Trim leading and trailing whitespace
  Returns the pointer to the new string start and new length
 */
const unsigned char *zsv_strtrim(const char unsigned *restrict s, size_t *lenp);

/**
 * zsv_strunescape_backslash(): convert consecutive white to single space
 *
 * @param s     string to convert
 * @param len   length of input string
 * @param flags bitfield of ZSV_STRWHITE_FLAG_XXX values
 */
size_t zsv_strunescape_backslash(unsigned char *s, size_t len);

/**
 * Get the next UTF8 codepoint in a string
 * Return: length of next character (in bytes), or 0 on error or end of string
 */
size_t zsv_strnext(const unsigned char *s, size_t len, int32_t *codepoint);

/**
 * Get the last UTF8 codepoint in a string
 * Return: length of last character (in bytes), or 0 on error or end of string
 */
size_t zsv_strlast(const unsigned char *s, size_t len, int32_t *codepoint);

/**
 *  Check if the next char is a plus or minus. If so, return its length, else return 0
 */
size_t zsv_strnext_is_sign(const unsigned char *s, size_t len);

/**
 *  Check if the next char is a currency char. If so, return its length, else return 0
 */
size_t zsv_strnext_is_currency(const unsigned char *s, size_t len);

/*
 * Convert a string to a double. must convert entire string, else returns error
 * @param s     string to convert
 * @param d     pointer to converted value, on success
 *
 * @returns     0 on success, non-zero on error
 */
int zsv_strtod_exact(const char *s, double *d);

/*
 * `zsv_get_cell_trimmed` is equivalent to `zsv_get_cell`, except that it
 * @param s     string to convert
 * @param len   length of input string
 * returns a value with leading and trailing whitespace removed
 */
#include <zsv.h>
struct zsv_cell zsv_get_cell_trimmed(zsv_parser parser, size_t index);

#endif
