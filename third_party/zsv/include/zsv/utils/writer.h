/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_WRITER_H
#define ZSV_WRITER_H

#include <stdio.h>
#include <stdint.h> // uint64_t

#define ZSV_WRITER_NEW_ROW 1
#define ZSV_WRITER_SAME_ROW 0

/**
 * csv writer options
 *
 * If `output_path` is non-NULL, `stream` must be NULL or stdout and `write` must be NULL or fwrite
 ***/
struct zsv_csv_writer_options {
  char with_bom;
  size_t (*write)(const void *restrict, size_t size, size_t nitems, void *restrict stream);
  void *stream;
  void (*table_init)(void *);
  void *table_init_ctx;
  const char *output_path; // if provided, will be created by zsv_writer_new() and closed by zsv_writer_delete()
  void (*on_row)(void *);
  void *on_row_ctx;

  void (*on_delete)(void *);
  void *on_delete_ctx;
};

void zsv_writer_set_default_opts(struct zsv_csv_writer_options opts);
struct zsv_csv_writer_options zsv_writer_get_default_opts(void);

enum zsv_writer_status {
  zsv_writer_status_ok = 0,
  zsv_writer_status_error,
  zsv_writer_status_missing_handle,
  zsv_writer_status_unknown = 999
};

struct zsv_writer_data;
typedef struct zsv_writer_data *zsv_csv_writer;

/**
 * Get a new CSV writer
 *
 * @returns handle, or NULL on error, in which case errno will be set
 */
zsv_csv_writer zsv_writer_new(struct zsv_csv_writer_options *opts);
enum zsv_writer_status zsv_writer_delete(zsv_csv_writer w);

enum zsv_writer_status zsv_writer_flush(zsv_csv_writer w);

void zsv_writer_set_temp_buff(zsv_csv_writer w, unsigned char *buff, size_t buffsize);

enum zsv_writer_status zsv_writer_cell(zsv_csv_writer,
                                       char new_row, // ZSV_WRITER_NEW_ROW or ZSV_WRITER_SAME_ROW
                                       const unsigned char *s, size_t len, char check_if_needs_quoting);

/*
 * Get total bytes that have been written (to disk and buffer)
 */
uint64_t zsv_writer_cum_bytes_written(zsv_csv_writer);

unsigned char *zsv_writer_str_to_csv(const unsigned char *s, size_t len);

/*
 * quote a CSV string, if needed
 * @return NULL if no quoting required, `buff` if quoted value written to buff,
 * or newly-allocated memory if buff not large enough (caller must free)
 */
unsigned char *zsv_csv_quote(const unsigned char *utf8_value, size_t len, unsigned char *buff, size_t buffsize);

// zsv_writer_cell convenience funcs: zsv_writer_cell_XX where XX = printf specifier
enum zsv_writer_status zsv_writer_cell_zu(zsv_csv_writer w, char new_row, size_t zu);

enum zsv_writer_status zsv_writer_cell_s(zsv_csv_writer w, char new_row, const unsigned char *s,
                                         char check_if_needs_quoting);

enum zsv_writer_status zsv_writer_cell_Lf(zsv_csv_writer w, char new_row,
                                          const char *fmt_spec, // provide X in %XLf e.g. ".2" or ""
                                          long double ldbl);

// write a blank cell
enum zsv_writer_status zsv_writer_cell_blank(zsv_csv_writer w, char new_row);

/**
 * set a value to be prepended to the next cell value that is written
 *
 * @param w  handle to the writer
 * @param s  text value to write (can be NULL)
 */
void zsv_writer_cell_prepend(zsv_csv_writer w, const unsigned char *s);

#endif
