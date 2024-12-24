/*
 * Copyright (C) 2021 Tai Chi Minh Ralph Eastwood, Matt Wong and Guarnerix dba Liquidaty
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_API_H
#define ZSV_API_H

#define ZSV_ROW_MAX_SIZE_DEFAULT 65536
#define ZSV_ROW_MAX_SIZE_DEFAULT_S "64k"

#define ZSV_MAX_COLS_DEFAULT 1024

#define ZSV_ROW_MAX_SIZE_MIN 1024
#define ZSV_ROW_MAX_SIZE_MIN_S "1024"

#define ZSV_MIN_SCANNER_BUFFSIZE 4096
#define ZSV_DEFAULT_SCANNER_BUFFSIZE (1 << 18) // 256k

#include "zsv_export.h"
/*****************************************************************************
 * libzsv API
 *
 * Functions provided by the zsv library are described herein. This document is
 * organized into three sections:
 * - required functions. any time libzsv is used and any input at all is parsed,
 *   each of these functions should be used (usually, exactly once)
 * - minimal access functions. these functions are generally necessary to use
 *   libzsv for any non-trivial task
 * - other functions
 ******************************************************************************/

/******************************************************************************
 * Required functions:
 * - zsv_new(): allocate a parser
 * - zsv_parse_more(): parse some data
 * - zsv_finish(): tie up loose ends
 * - zsv_delete(): dispose the parser
 ******************************************************************************/

ZSV_EXPORT
int zsv_peek(zsv_parser);

/**
 * Create a zsv parser. Typically, passed options will at least include a
 * a `row_handler()` callback. Many, but not all, options can be subsequently
 * set or modified after calling `zsv_new()`
 *
 * @param options see `struct zsv_opts` in common.h
 * @returns zsv parser handle
 */
ZSV_EXPORT
zsv_parser zsv_new(struct zsv_opts *opts);

/**
 * Parse the next chunk of data from the input stream:
 * - Immediately after a cell (column) delimiter is parsed, the configured
 *   `cell_handler()` callback, if any, is called
 * - Immediately after a row delimiter is parsed, the configured
 *   `row_handler()` callback, if any, is called.
 *
 * @param parser
 * @returns zsv_status_ok if more data remains to be parsed,
 *          zsv_status_no_more_input if the stream's EOF has been reached,
 *          or other zsv status code in the event of error or cancellation
 */
ZSV_EXPORT enum zsv_status zsv_parse_more(zsv_parser parser);

/**
 * Finish any remaining processing, after all input has been read
 */
ZSV_EXPORT enum zsv_status zsv_finish(zsv_parser);

/**
 * Dispose of a parser that was created with `zsv_new()` or `zsv_new_with_properties()`
 */
ZSV_EXPORT enum zsv_status zsv_delete(zsv_parser);

/******************************************************************************
 * minimal access functions:
 * - zsv_cell_count(): get the number of cells in the row
 * - zsv_get_cell(): retrieve a cell value
 ******************************************************************************/

/**
 * Get the number of cells in the row that was just parsed. This function
 * is typically called from within your `row_handler()` callback. In the event
 * that the last row did not contain a single cell delimiter, returns 1
 *
 * @param parser
 * @returns number, >= 1, of cells in the row that was just parsed
 */
ZSV_EXPORT
size_t zsv_cell_count(zsv_parser parser);

/**
 * Get the contents of a cell in the row that was just parsed. See `struct zsv_cell`
 * in common.h for further details
 *
 * @param parser
 * @param index zero-based index of the cell to fetch
 * @return `zsv_cell` structure with the bytes and length of this cell value
 *
 * An example of a `row_handler()` loop to print each cell in a row might be:
 * ```
 *   size_t cell_count = zsv_cell_count(parser);
 *   for(size_t i = 0; i < cell_count; i++) {
 *     struct zsv_cell c = zsv_get_cell(parser, i);
 *     printf("%.*s", c.len, (const char *)c.str);
 *   }
 * ```
 */
struct zsv_cell zsv_get_cell(zsv_parser parser, size_t index);

/**
 * `zsv_get_cell_len()` is not needed in most cases, but may be useful in
 * restrictive cases such as when calling from Javascript into wasm
 *
 * @param  parser
 * @param  ix     0-based index of the cell to copy
 * @return length of cell contents (0 if cell is empty)
 */
ZSV_EXPORT
size_t zsv_get_cell_len(zsv_parser parser, size_t ix);

/**
 * get a pointer to the cell contents (not NULL-terminated)
 * @param parser
 * @param ix     0-based index of the cell to copy. Caller must ensure validity
 * @return pointer to the cell contents, or NULL if contents are empty
 */
ZSV_EXPORT
unsigned char *zsv_get_cell_str(zsv_parser parser, size_t ix);

/******************************************************************************
 * other functions
 ******************************************************************************/

/**
 * Get the library version
 */
ZSV_EXPORT
const char *zsv_lib_version(void);

/**
 * Change a parser's row handler. This function may be called at any time
 * during the parsing process to change the row handler that is called each
 * time a row is parsed
 *
 * @param parser
 * @param row_handler new callback value
 */
ZSV_EXPORT void zsv_set_row_handler(zsv_parser, void (*row_handler)(void *ctx));

/**
 * Check if the row we just parsed consisted entirely of blank data
 *
 * @param parser
 * @return non-zero if blank, 0 if non-blank
 */
ZSV_EXPORT
char zsv_row_is_blank(zsv_parser parser);

/**
 * Set the context pointer that is passed to our callbacks
 * @param parser
 * @param ctx new context pointer value
 */
ZSV_EXPORT
void zsv_set_context(zsv_parser parser, void *ctx);

/**
 * Set the read function that is invoked by `zsv_parse_more()` to fetch more data.
 * If not explicitly set, defaults to fread
 *
 * @param parser
 * @param read_function
 * @param stream        value that is passed to read_function when it is called
 */
ZSV_EXPORT
void zsv_set_read(zsv_parser parser, size_t (*read_func)(void *restrict, size_t n, size_t size, void * __restrict__));

/**
 * Set the input stream our parser reads from. If not explicity set, defaults to
 * stdin. This can be used to read multiple inputs as a single combined input
 * by calling `zsv_set_input()` after `zsv_parse_more()` returns
 * `zsv_status_no_more_input`
 */
ZSV_EXPORT
void zsv_set_input(zsv_parser, void *in);

/**
 * Insert a filter to process or modify, before parsing, the next chunk of raw
 * bytes read from the input stream. For example, to save a copy of the raw
 * input to a file, `zsv_set_scan_filter()` could be called with
 * `zsv_filter_write` passed as the filter argument, and the target FILE *
 * passed as the context pointer.
 *
 * @param parser
 * @param filter callback that is called on each chunk that is read from the
 *               input stream, before the chunk is parsed. The callback may
 *               modify the contents of the buffer so long as its return value
 *               does not exceed the bufflen it was passed
 */
ZSV_EXPORT enum zsv_status zsv_set_scan_filter(zsv_parser parser,
                                               size_t (*filter)(void *ctx, unsigned char *buff, size_t bufflen),
                                               void *ctx);

/**
 * Set parsing mode to fixed-width. Once set to fixed mode, a parser may not be
 *   set back to CSV mode
 * @return status code
 * @param parser parser handle
 * @param count number of elements in offsets
 * @param offsets array of cell-end offsets. offsets[0] should be the length of the first cell
 */
ZSV_EXPORT enum zsv_status zsv_set_fixed_offsets(zsv_parser parser, size_t count, size_t *offsets);

/**
 * Parse a buffer of bytes. This function is usually not needed, but
 * can be used to parse in a push instead of pull manner
 *
 * @param parser
 * @param buff   the input buffer to parse. This buffer may not overlap with
 *               the parser buffer!
 * @param len    length of the input to parse
 */
ZSV_EXPORT enum zsv_status zsv_parse_bytes(zsv_parser parser, const unsigned char * __restrict__ buff, size_t len);

/**
 * Get a text description of a status code
 */
ZSV_EXPORT
const unsigned char *zsv_parse_status_desc(enum zsv_status status);

/**
 * Abort parsing. After this function is called, no further
 * `row_handler()` or `cell_handler()` calls will be made, and parse functions
 * will return zsv_status_cancelled
 */
ZSV_EXPORT
void zsv_abort(zsv_parser);

/**
 * @return number of bytes scanned from the last zsv_parse_more() invocation
 */
ZSV_EXPORT size_t zsv_scanned_length(zsv_parser);

/**
 * @return cumulative number of bytes scanned across all requests by this parser
 */
ZSV_EXPORT size_t zsv_cum_scanned_length(zsv_parser parser);

/**
 * @return number of raw bytes scanned from the beginning to the end of this row
 */
ZSV_EXPORT size_t zsv_row_length_raw_bytes(zsv_parser parser);

/**
 * Check the quoted status of the last cell that was read. This function is only
 * applicable when called from within a cell_handler() callback. Furthermore, this
 * function is generally only useful when the cell value will subsequent be
 * output in CSV format
 *
 * @returns 0 if value will *not* need to be quoted when output as CSV, or
 *          1 if it might need to be quoted
 */
ZSV_EXPORT
char zsv_quoted(zsv_parser parser);

/**
 * Create a zsv_opts structure and return its handle
 *
 * This is only necessary in environments where structures cannot be directly
 * instantiated such as web assembly. Otherwise, you should avoid this function
 * and just create a `struct zsv_opts` on the stack
 *
 * Each argument to `zsv_opts_new()` corresponds to the same-named `struct zsv_opts` element
 * See common.h for details
 */
ZSV_EXPORT struct zsv_opts *zsv_opts_new(void (*row_handler)(void *ctx),
                                         void (*cell_handler)(void *ctx, unsigned char *utf8_value, size_t len),
                                         void *ctx, zsv_generic_read read, void *stream, unsigned char *buff,
                                         size_t buffsize, unsigned max_columns, unsigned max_row_size, char delimiter,
                                         char no_quotes
#ifdef ZSV_EXTRAS
                                         ,
                                         size_t max_rows
#endif
);

/**
 * Destroy an option structure that was created by zsv_opts_new()
 */
ZSV_EXPORT void zsv_opts_delete(struct zsv_opts *);

/******************************************************************************
 * Pull parsing functions
 ******************************************************************************/

/**
 * To use pull parsing, do not use row or cell handlers, handler context
 * or zsv_parse_more(). Instead, call zsv_next_row() until a non-ok result
 * @param  parser parser handle
 * @return zsv_status_ok on success, other status code on error
 */
ZSV_EXPORT
enum zsv_status zsv_next_row(zsv_parser parser);

/******************************************************************************
 * Miscellaneous functions used by the parser that may have standalone utility
 ******************************************************************************/

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

#endif
