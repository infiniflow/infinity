#ifndef ZSV_OVERWRITE_H
#define ZSV_OVERWRITE_H

#include <sqlite3.h>

struct zsv_overwrite_ctx {
  char *src;
  size_t author_ix;
  size_t timestamp_ix;
  size_t old_value_ix;
  // enum zsv_overwrite_type type;
  enum zsv_status (*next)(void *ctx, struct zsv_overwrite_data *odata);
  struct {
    FILE *f;
    zsv_parser parser;
  } csv;
  struct {
    char *filename;
    sqlite3 *db;
    sqlite3_stmt *stmt; // select row, column, overwrite
    const char *sql;
  } sqlite3;
  size_t row_ix;
};

/**
 * The easiest way to enable overwrite support is to use zsv_overwrite_auto()
 * which, given an input located at /path/to/my-data.csv, will assume an overwrite source
 * located at /path/to/.zsv/data/my-data.csv/overwrite.sqlite3
 * in a table named 'overwrites'
 *
 * zsv_overwrite_auto() returns:
 * - zsv_status_done if a valid overwrite file was found
 * - zsv_status_no_more_input if no overwrite file was found
 * - a different status code on error
 *
 * This function is used in app/cli.c via the --apply-overwrites option
 */
enum zsv_status zsv_overwrite_auto(struct zsv_opts *opts, const char *csv_filename);

/**
 * As an alternative to zsv_overwrite_auto(), you can specify your own
 * source from which to fetch overwrite data.
 * The specified source can be a sqlite3 file or a CSV file
 *
 * This approach is used by app/echo.c
 */
struct zsv_overwrite_opts {
  /* src may be either:
   *
   * 1. sqlite3 file source:
   *    sqlite3://<filename>[?sql=<query>]",
   *
   * e.g. sqlite3:///path/to/my-overwritedb.sqlite3?sql=select row, column, value from overwrites order by row, column",
   *
   * or
   *
   * 2. CSV file source
   *   /path/to/file.csv",
   * where the CSV columns are row,col,val (in that order, with a header row),
   * and rows already-sorted by row and column",
   */
  const char *src;
};

void *zsv_overwrite_context_new(struct zsv_overwrite_opts *);
enum zsv_status zsv_overwrite_next(void *h, struct zsv_overwrite_data *odata);
enum zsv_status zsv_overwrite_open(void *h);
enum zsv_status zsv_overwrite_context_delete(void *h);

#endif
