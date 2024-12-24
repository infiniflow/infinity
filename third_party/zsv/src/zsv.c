/*
 * Copyright (C) 2021 Tai Chi Minh Ralph Eastwood (self), Matt Wong (Guarnerix Inc dba Liquidaty)
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_MIN_SCANNER_BUFFSIZE
#define ZSV_MIN_SCANNER_BUFFSIZE 4096
#endif

#include "zsv.h"
#include <zsv/utils/compiler.h>
#ifdef ZSV_EXTRAS
#include <zsv/utils/arg.h>
#endif

struct zsv_cell zsv_get_cell_1(zsv_parser parser, size_t ix);
static struct zsv_cell zsv_get_cell_with_overwrite(zsv_parser parser, size_t col_ix);
#include "zsv_internal.c"

#ifndef ZSV_VERSION
#define ZSV_VERSION "unknown"
#endif

ZSV_EXPORT
const char *zsv_lib_version(void) {
  return ZSV_VERSION;
}

#include "zsv_strencode.c"

/**
 * When we parse a chunk, if it was not the first parse call, we might have a partial
 * row at the end of our buffer that must be moved. The reason we do this at the beginning
 * of a parse and not at the end of the prior parse is so that between chunks, the input
 * chunk remains available in a contiguous block of one or more rows
 */
// __attribute__((always_inline))
inline static size_t scanner_pre_parse(struct zsv_scanner *scanner) {
  scanner->last = '\0';
  if (VERY_LIKELY(scanner->old_bytes_read)) {
    scanner->last = scanner->buff.buff[scanner->old_bytes_read - 1];
    if (scanner->row_start < scanner->old_bytes_read) {
      size_t len = scanner->old_bytes_read - scanner->row_start;
      memmove(scanner->buff.buff, scanner->buff.buff + scanner->row_start, len);
      scanner->partial_row_length = len;
    } else {
      scanner->cell_start = 0;
      scanner->row_start = 0;
      zsv_clear_cell(scanner);
    }
    scanner->cell_start -= scanner->row_start;
    for (size_t i2 = 0; i2 < scanner->row.used; i2++)
      scanner->row.cells[i2].str -= scanner->row_start;
    scanner->row_start = 0;
    scanner->old_bytes_read = 0;
  }

  scanner->cum_scanned_length += scanner->scanned_length - scanner->partial_row_length;

  size_t capacity = scanner->buff.size - scanner->partial_row_length;
  if (VERY_UNLIKELY(capacity == 0)) {
    // our row size was too small to fit a single row of data
    fprintf(stderr, "Warning: row %zu truncated\n", scanner->data_row_count);
    if (scanner->mode == ZSV_MODE_FIXED) {
      if (VERY_UNLIKELY(row_fx(scanner, scanner->buff.buff, 0, scanner->buff.size)))
        return zsv_status_cancelled;
    } else if (VERY_UNLIKELY(row_dl(scanner)))
      return zsv_status_cancelled;

    // throw away the next row end
    scanner->buffer_exceeded = 1;
    scanner->opts.row_handler = zsv_throwaway_row;
    scanner->opts.ctx = scanner;

    scanner->partial_row_length = 0;
    capacity = scanner->buff.size;
  }
  return capacity;
}

/**
 * apply --header-row option
 */
static enum zsv_status zsv_insert_string(struct zsv_scanner *scanner) {
  // to do: replace below with
  // return parse_bytes(scanner, bytes, len);
  size_t len = strlen(scanner->insert_string);
  if (len > scanner->buff.size - scanner->partial_row_length)
    len = scanner->buff.size - 1; // to do: throw an error instead
  memcpy(scanner->buff.buff + scanner->partial_row_length, scanner->insert_string, len);
  if (scanner->buff.buff[len] != '\n')
    scanner->buff.buff[len] = '\n';
  enum zsv_status stat = zsv_scan(scanner, scanner->buff.buff, len + 1);
  scanner->insert_string = NULL;
  return stat;
}

/**
 * Read the next chunk of data from our input stream and parse it, calling our
 * custom handlers as each cell and row are parsed
 */
ZSV_EXPORT
enum zsv_status zsv_parse_more(struct zsv_scanner *scanner) {
  if (VERY_UNLIKELY(scanner->insert_string != NULL))
    zsv_insert_string(scanner);

  size_t capacity = scanner_pre_parse(scanner);
  size_t bytes_read;
  if (VERY_UNLIKELY(scanner->checked_bom == 0)) {
#ifdef ZSV_EXTRAS
    // initialize progress timer
    if (scanner->opts.progress.seconds_interval)
      scanner->progress.last_time = time(NULL);
#endif
    size_t bom_len = strlen(ZSV_BOM);
    scanner->checked_bom = 1;
    if ((bytes_read = scanner->read(scanner->buff.buff, 1, bom_len, scanner->in)) == bom_len &&
        !memcmp(scanner->buff.buff, ZSV_BOM, bom_len)) {
      // have bom. disregard what we just read
      bytes_read = scanner->read(scanner->buff.buff, 1, capacity, scanner->in);
      scanner->had_bom = 1;
    } else { // no BOM. keep the bytes we just read
      // bytes_read = bom_len + scanner->read(scanner->buff.buff + bom_len, 1, capacity - bom_len, scanner->in);
      if (bytes_read == bom_len) // maybe we only read < 3 bytes
        bytes_read += scanner->read(scanner->buff.buff + bom_len, 1, capacity - bom_len, scanner->in);
    }
  } else // already checked bom. read as usual
    bytes_read = scanner->read(scanner->buff.buff + scanner->partial_row_length, 1, capacity, scanner->in);
  scanner->started = 1;
  if (VERY_UNLIKELY(scanner->filter != NULL))
    bytes_read = scanner->filter(scanner->filter_ctx, scanner->buff.buff + scanner->partial_row_length, bytes_read);
  if (VERY_LIKELY(bytes_read))
    return zsv_scan(scanner, scanner->buff.buff, bytes_read);

  scanner->scanned_length = scanner->partial_row_length;
  return zsv_status_no_more_input;
}

ZSV_EXPORT
void zsv_abort(zsv_parser parser) {
  parser->abort = 1;
}

ZSV_EXPORT
char zsv_row_is_blank(zsv_parser parser) {
  return zsv_internal_row_is_blank(parser);
}

static void zsv_pull_row(void *ctx) {
  zsv_parser parser = ctx;
  parser->pull.now = 1;
  parser->pull.row_used = parser->row.used;
}

/**
 * For pull parsing, use zsv_next_row(). Not quite as fast as push parsing, but pretty close
 * @return zsv_status_row on success
 */
ZSV_EXPORT
enum zsv_status zsv_next_row(zsv_parser parser) {
  if (VERY_UNLIKELY(!parser->pull.regs)) {
    if (parser->started)
      return zsv_status_error; // error: already started a push parser
    if (!(parser->pull.regs = calloc(1, sizeof(*parser->pull.regs))))
      return zsv_status_memory;
    parser->mode = ZSV_MODE_DELIM_PULL;
    zsv_set_row_handler(parser, zsv_pull_row);
    zsv_set_context(parser, parser);
    if (parser->insert_string != NULL)
      parser->pull.stat = zsv_insert_string(parser);
    if (parser->pull.stat == zsv_status_row)
      return parser->pull.stat;
  }
  if (VERY_LIKELY(parser->pull.stat == zsv_status_row))
    parser->pull.stat = zsv_scan_delim_pull(parser, parser->pull.buff, parser->pull.bytes_read);
  if (VERY_UNLIKELY(parser->pull.stat == zsv_status_ok)) {
    do {
      parser->pull.stat = zsv_parse_more(parser); // should return zsv_status_row or zsv_status_no_more_input
    } while (parser->pull.stat == zsv_status_ok);
    if (VERY_LIKELY(parser->pull.stat == zsv_status_row))
      return parser->pull.stat;
  }

  if (VERY_UNLIKELY(parser->pull.stat == zsv_status_no_more_input)) {
    parser->pull.now = 0;
    zsv_finish(parser);

    parser->pull.stat = zsv_status_done;
    if (parser->pull.now) {
      parser->pull.now = 0;
      parser->row.used = parser->pull.row_used;
      return zsv_status_row;
    }
  }
  return parser->pull.stat;
}

// to do: rename to zsv_column_count(). rename all other zsv_hand to just zsv_
ZSV_EXPORT
size_t zsv_cell_count(zsv_parser parser) {
  return parser->row.used;
}

ZSV_EXPORT
void zsv_set_row_handler(zsv_parser parser, void (*row_handler)(void *ctx)) {
  if (parser->opts.row_handler == parser->opts_orig.row_handler)
    parser->opts.row_handler = row_handler;
  parser->opts_orig.row_handler = row_handler;
}

ZSV_EXPORT
void zsv_set_context(zsv_parser parser, void *ctx) {
  if (parser->opts.ctx == parser->opts_orig.ctx)
    parser->opts.ctx = ctx;
  parser->opts_orig.ctx = ctx;
}

ZSV_EXPORT
void zsv_set_read(zsv_parser parser, size_t (*read_func)(void *restrict, size_t n, size_t size, void *restrict)) {
  parser->read = read_func;
}

ZSV_EXPORT
void zsv_set_input(zsv_parser parser, void *in) {
  parser->in = in;
}

ZSV_EXPORT
char zsv_quoted(zsv_parser parser) {
  return parser->quoted || parser->opts.no_quotes;
}

struct zsv_cell zsv_get_cell_1(zsv_parser parser, size_t ix) {
  if (VERY_LIKELY(ix < parser->row.used))
    return parser->row.cells[ix];

  struct zsv_cell c = {0, 0, 0, 0};
  return c;
}

// to do: benchmark returning zsv_cell struct vs just a zsv_cell pointer
ZSV_EXPORT
struct zsv_cell zsv_get_cell(zsv_parser parser, size_t ix) {
  return parser->get_cell(parser, ix);
}

/**
 * `zsv_get_cell_len()` is not needed in most cases, but may be useful in
 * restrictive cases such as when calling from Javascript into wasm
 */
ZSV_EXPORT
size_t zsv_get_cell_len(zsv_parser parser, size_t ix) {
  if (ix < parser->row.used)
    return parser->row.cells[ix].len;
  return 0;
}

ZSV_EXPORT
unsigned char *zsv_get_cell_str(zsv_parser parser, size_t ix) {
  struct zsv_cell c = zsv_get_cell_1(parser, ix);
  return c.len ? c.str : NULL;
}

ZSV_EXPORT enum zsv_status zsv_set_fixed_offsets(zsv_parser parser, size_t count, size_t *offsets) {
  if (!count) {
    fprintf(stderr, "Fixed offset count must be greater than zero\n");
    return zsv_status_invalid_option;
  }
  if (offsets[0] == 0)
    fprintf(stderr, "Warning: first cell width is zero\n");
  for (size_t i = 1; i < count; i++) {
    if (offsets[i - 1] > offsets[i]) {
      fprintf(stderr, "Invalid offset %zu may not exceed prior offset %zu\n", offsets[i], offsets[i - 1]);
      return zsv_status_invalid_option;
    } else if (offsets[i - 1] == offsets[i])
      fprintf(stderr, "Warning: offset %zu repeated, will always yield empty cell\n", offsets[i - 1]);
  }

  if (offsets[count - 1] > parser->buff.size) {
    fprintf(stderr, "Offset %zu exceeds total buffer size %zu\n", offsets[count - 1], parser->buff.size);
    return zsv_status_invalid_option;
  }
  if (parser->cum_scanned_length) {
    fprintf(stderr, "Scanner mode cannot be changed after parsing has begun\n");
    return zsv_status_invalid_option;
  }

  free(parser->fixed.offsets);
  parser->fixed.offsets = calloc(count, sizeof(*parser->fixed.offsets));
  if (parser->fixed.offsets == NULL) {
    fprintf(stderr, "Out of memory!\n");
    return zsv_status_memory;
  }
  parser->fixed.count = count;
  for (unsigned i = 0; i < count; i++)
    parser->fixed.offsets[i] = offsets[i];

  parser->mode = ZSV_MODE_FIXED;
  parser->checked_bom = 1;

  set_callbacks(parser);

  return zsv_status_ok;
}

ZSV_EXPORT
int zsv_peek(zsv_parser z) {
  if (z->scanned_length + 1 < z->buff.size)
    return z->buff.buff[z->scanned_length + 1];
  return -1;
}

/**
 * Create a zsv parser
 * @param opts
 * @returns parser handle
 */
ZSV_EXPORT
zsv_parser zsv_new(struct zsv_opts *opts) {
  struct zsv_opts tmp;
  if (!opts) {
    opts = &tmp;
    memset(opts, 0, sizeof(*opts));
  }

  if (!opts->max_row_size)
    opts->max_row_size = ZSV_ROW_MAX_SIZE_DEFAULT;
  if (!opts->max_columns)
    opts->max_columns = ZSV_MAX_COLS_DEFAULT;
  if (opts->delimiter == '\n' || opts->delimiter == '\r' || opts->delimiter == '"') {
    fprintf(stderr, "Invalid delimiter\n");
    return NULL;
  }
  struct zsv_scanner *scanner = calloc(1, sizeof(*scanner));
  if (scanner) {
    if (zsv_scanner_init(scanner, opts)) {
      zsv_delete(scanner);
      scanner = NULL;
    }
  }
  return scanner;
}

ZSV_EXPORT
enum zsv_status zsv_finish(struct zsv_scanner *scanner) {
  enum zsv_status stat = zsv_status_ok;
  if (!scanner)
    return zsv_status_error;
  if (!scanner->abort) {
    if (scanner->mode == ZSV_MODE_FIXED) {
      if (scanner->partial_row_length && memchr("\n\r", scanner->buff.buff[scanner->partial_row_length - 1], 2))
        scanner->partial_row_length--;
      if (scanner->partial_row_length)
        return row_fx(scanner, scanner->buff.buff, 0, scanner->partial_row_length);
      return zsv_status_ok;
    }

    if ((scanner->quoted & ZSV_PARSER_QUOTE_UNCLOSED) && scanner->partial_row_length > scanner->cell_start) {
      int quote = '"';
      scanner->quoted |= ZSV_PARSER_QUOTE_CLOSED;
      scanner->quoted -= ZSV_PARSER_QUOTE_UNCLOSED;
      if (scanner->last == quote)
        scanner->quote_close_position = scanner->partial_row_length - scanner->cell_start;
      else {
        scanner->quote_close_position = scanner->partial_row_length - scanner->cell_start;
        scanner->scanned_length++;
      }
    }
  }

  if (!scanner->finished) {
    scanner->finished = 1;
    if (!scanner->abort) {
      if (scanner->scanned_length > 0 && scanner->scanned_length >= scanner->cell_start)
        cell_dl(scanner, scanner->buff.buff + scanner->cell_start, scanner->scanned_length - scanner->cell_start);
      if (scanner->have_cell) {
        if (row_dl(scanner))
          stat = zsv_status_cancelled;
      }
    } else
      stat = zsv_status_cancelled;
#ifdef ZSV_EXTRAS
    if (scanner->opts.completed.callback)
      scanner->opts.completed.callback(scanner->opts.completed.ctx, stat);
#endif
  }
  return stat;
}

ZSV_EXPORT
enum zsv_status zsv_delete(zsv_parser parser) {
  if (parser) {
    if (parser->free_buff && parser->buff.buff)
      free(parser->buff.buff);

    free(parser->row.cells);
    free(parser->fixed.offsets);
    collate_header_destroy(&parser->collate_header);
    free(parser->pull.regs);

#ifdef ZSV_EXTRAS
    if (parser->overwrite.close)
      parser->overwrite.close(parser->overwrite.ctx);
#endif

    free(parser);
  }
  return zsv_status_ok;
}

ZSV_EXPORT
const unsigned char *zsv_parse_status_desc(enum zsv_status status) {
  switch (status) {
  case zsv_status_ok:
    return (unsigned char *)"OK";
  case zsv_status_cancelled:
    return (unsigned char *)"User cancelled";
  case zsv_status_no_more_input:
    return (unsigned char *)"No more input";
  case zsv_status_invalid_option:
    return (unsigned char *)"Invalid option";
  case zsv_status_memory:
    return (unsigned char *)"Out of memory";
  case zsv_status_done:
    return (unsigned char *)"No more rows to pull";
  case zsv_status_row:
    return (unsigned char *)"Row successfully pulled";
  case zsv_status_error:
    return (unsigned char *)"Unexpected error";
#ifdef ZSV_EXTRAS
  case zsv_status_max_rows_read:
    return (unsigned char *)"Maximum specified rows have been parsed";
#endif
  }
  return (unsigned char *)"Unknown";
}

ZSV_EXPORT
size_t zsv_scanned_length(zsv_parser parser) {
  return parser->scanned_length;
}

ZSV_EXPORT
size_t zsv_cum_scanned_length(zsv_parser parser) {
  return parser->cum_scanned_length + (parser->finished ? 0 : parser->scanned_length) +
         (parser->had_bom ? strlen(ZSV_BOM) : 0);
}

ZSV_EXPORT
size_t zsv_row_length_raw_bytes(zsv_parser parser) {
  return parser->scanned_length - parser->row_start;
}

/**
 * @param parser parser handle
 * @param buff   the input buffer. Note: this buffer may not overlap with
 *               the parser buffer!
 * @param len    length of the input to parse
 */
enum zsv_status zsv_parse_bytes(struct zsv_scanner *scanner, const unsigned char *bytes, size_t len) {
  enum zsv_status stat = zsv_status_ok;
  const unsigned char *cursor = bytes;
  while (len && stat == zsv_status_ok) {
    size_t capacity = scanner_pre_parse(scanner);
    size_t this_chunk_size = len > capacity ? capacity : len;
    memcpy(scanner->buff.buff + scanner->partial_row_length, cursor, this_chunk_size);
    cursor += this_chunk_size;
    len -= this_chunk_size;
    if (scanner->filter)
      this_chunk_size =
        scanner->filter(scanner->filter_ctx, scanner->buff.buff + scanner->partial_row_length, this_chunk_size);
    if (this_chunk_size)
      stat = zsv_scan(scanner, scanner->buff.buff, this_chunk_size);
  }
  return stat;
}
