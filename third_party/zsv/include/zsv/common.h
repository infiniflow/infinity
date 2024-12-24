/*
 * Copyright (C) 2021 Tai Chi Minh Ralph Eastwood, Matt Wong and Guarnerix dba Liquidaty
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_COMMON_H
#define ZSV_COMMON_H

#ifdef __cplusplus
#define ZSV_BEGIN_DECL extern "C" {
#define ZSV_END_DECL }
#else
#define ZSV_BEGIN_DECL
#define ZSV_END_DECL /* empty */
#endif

enum zsv_status {
  zsv_status_ok = 0,
  zsv_status_cancelled,
  zsv_status_no_more_input,
  zsv_status_invalid_option,
  zsv_status_memory,
  zsv_status_error,
  zsv_status_row,
  zsv_status_done = 100
#ifdef ZSV_EXTRAS
    ,
  zsv_status_max_rows_read = 999
#endif
};

/**
 * `zsv_parser` is the type of a zsv parser handle
 */
typedef struct zsv_scanner *zsv_parser;

/**
 * Structure returned by `zsv_get_cell()` for fetching a parsed CSV cell value
 */
struct zsv_cell {
  /**
   * address of cell contents (not null-terminated)
   */
  unsigned char *str;

  /**
   * length of cell contents
   */
  size_t len;

  /**
   * bitfield values for `quoted` flags
   */
#define ZSV_PARSER_QUOTE_NONE 0     /* content does not need to be quoted */
#define ZSV_PARSER_QUOTE_UNCLOSED 1 /* only used internally by parser */
#define ZSV_PARSER_QUOTE_CLOSED 2   /* value was quoted */
#define ZSV_PARSER_QUOTE_NEEDED 4   /* value contains delimiter or dbl-quote */
#define ZSV_PARSER_QUOTE_EMBEDDED 8 /* value contains dbl-quote */
#define ZSV_PARSER_QUOTE_PENDING 16 /* only used internally by parser */
  /**
   * quoted flags enable additional efficiency, in particular when input data will
   * be output as text (csv, json etc), by indicating whether the cell contents may
   * require special handling. For example, if the caller will output the cell value as
   * CSV and quoted == 0, the caller need not scan the cell contents to check if
   * quoting or escaping will be required
   */
  char quoted;
  unsigned char overwritten : 1;
};

typedef size_t (*zsv_generic_write)(const void *restrict, size_t, size_t, void *);
typedef size_t (*zsv_generic_read)(void *restrict, size_t n, size_t size, void * __restrict__);
typedef int (*zsv_generic_seek)(void *, long, int);

#ifdef ZSV_EXTRAS
/**
 * progress callback function signature
 * @param context pointer set in parser opts.progress.ctx
 * @param cumulative_row_count number of input rows read so far
 * @return zero to continue processing, non-zero to cancel parse
 */
typedef int (*zsv_progress_callback)(void *ctx, size_t cumulative_row_count);

/**
 * completed callback function signature
 * @param context pointer set in parser opts.progress.ctx
 * @param exit code
 */
typedef void (*zsv_completed_callback)(void *ctx, int code);

/**
 * Data can be "overwritten" on-the-fly by providing custom callbacks
 * data from the calling code is passed to the zsv library
 * via the `zsv_overwrite_data` structure
 */
struct zsv_overwrite_data {
  size_t row_ix; // 0-based
  size_t col_ix; // 0-based
  size_t timestamp;
  struct zsv_cell val;
  struct zsv_cell author;
  struct zsv_cell old_value;
  char have; // 1 = we have unprocessed overwrites
};

struct zsv_opt_overwrite {
  void *ctx;
  enum zsv_status (*open)(void *ctx);
  enum zsv_status (*next)(void *ctx, struct zsv_overwrite_data *odata);
  enum zsv_status (*close)(void *ctx);
  char cancel; // explicitly cancel application of overwrites
};

#endif

struct zsv_opts {
  /**
   * Callback that is called for each row that is parsed. In most use cases,
   * this is where most of the code logic resides
   */
  void (*row_handler)(void *ctx);

  /**
   * Callback that is called immediately after a cell is parsed.
   * The most common usage pattern is to omit the cell handler, and just loop
   * through each cell in the `row_handler`. But if you prefer, you can use
   * only a cell handler, or both a cell handler and a row handler.
   */
  void (*cell_handler)(void *ctx, unsigned char *utf8_value, size_t len);

  /**
   * If a row was too long to fit in the allocated memory, then
   * if `overflow_row_handler` is set, it will be called after the remaining
   * portion of the row is parsed. For example, if a row consists of
   * XXXXX,YYYYY and there is only enough memory to hold XXXX
   * then `row_handler()` will be called after parsing `XXXX,` and
   * `overflow_row_handler()` called twice, after parsing `X,YY` and `YYY`,
   * before the subsequent row is parsed
   *
   * Note: we considered adding a callback that would be called before the initial
   * `row_handler()` call, in the event that a subsequent `overflow_row_handler()`
   * call was anticipated. Because this scenario occurs so infrequently, we
   * decided to keep it simple with a single callback. But we may reconsider
   * if there is demand for that (or another) alternative approach
   */
  void (*overflow_row_handler)(void *ctx);

  /**
   * The context that is passed to each of our handlers
   */
  void *ctx;

  /**
   * Caller can specify its own read function for fetching data to be parsed
   * If not specified, the default value is `fread()`
   */
  zsv_generic_read read;

  /**
   * Caller can specify its own seek function for setting the file position
   * with zsv_index_seek. If not specified, the default value is `fseek()`
   */
  zsv_generic_seek seek;

  /**
   * Caller can specify its own stream that is passed to the read function
   * If not specified, the default value is stdin
   */
  void *stream;

  /**
   * Caller can specify its own buffer for the parser to use of at least
   * ZSV_MIN_SCANNER_BUFFSIZE (4096) in size. If not provided, an internal
   * buffer is allocated
   */
  unsigned char *buff;

  /**
   * If caller specifies its own buffer, this should be its size
   * Otherwise, this can be specified as the size of the internal buffer that
   * will be created, subject to increase if/as appropriate if max_row_size
   * is specified. Defaults to 256k
   *
   * cli option: -B,--buff-size
   */
  size_t buffsize;

  /**
   * Maximum number of columns to parse. defaults to 1024
   *
   * cli option: -c,--max-column-count
   */
  unsigned max_columns;

  /**
   * maximum row size can be used as an alternative way to specify the
   * internal buffer size, which will be at least as large as max_row_size * 2
   *
   * cli option: -r,--max-row-size
   */
  unsigned max_row_size;

  /**
   * delimiter: typically a comma or tab
   * can be any char other than newline, form feed or quote
   * defaults to comma
   *
   * cli option: -t,--tab-delim or -O,--other-delim <delim>
   */
  char delimiter;

  /**
   * no_quotes: if > 0, this flag indicates that the parser should treat double-quotes
   * just like any ordinary character
   * defaults to 0
   *
   * cli option: -q,--no-quote
   */
  char no_quotes;

  /**
   * flag to print more verbose messages to the console
   * cli option: -v,--verbose
   */
  char verbose;

  /**
   * if the actual data does not have a header row with column names, the caller
   * should provide one (in CSV format) which will be treated as if it was the
   * first row of data
   *
   * cli option: -0,--header-row
   */
  const char *insert_header_row;

  /**
   * number of rows that the header row spans. If 0 or 1, header is assumed to span 1 row
   * otherwise, set to number > 1 to span multiple rows
   */
  unsigned int header_span;

  /**
   * number of rows to ignore before the initial row is processed
   */
  unsigned int rows_to_ignore;

  /**
   * by default, zsv ignores empty header rows; the number of ignored rows
   * can be fetched via `zsv_get_empty_header_rows()`). To disable this
   * behavior, set `keep_empty_header_rows` to 1
   */
  unsigned char keep_empty_header_rows;

  /**
   * by default, zsv ignores malformed UTF8; set malformed_utf8_replace to
   * a value between 1 and 127 to replace malformed UTF8 with that single
   * char, to ZSV_MALFORMED_UTF8_REMOVE to remove
   * or ZSV_MALFORMED_UTF8_DO_NOT_REPLACE to explicitly leave untouched
   */
#define ZSV_MALFORMED_UTF8_DO_NOT_REPLACE -2
#define ZSV_MALFORMED_UTF8_REMOVE -1
  char malformed_utf8_replace;

  /**
   * `overrides` is a bitfield that indicates what ZSV options, if any, were
   * specifically set in the command invocation and is used to ensure
   * that option values set in the command invocation take priority over
   * default values, or values saved in related property values such as
   * .zsv/data/<filename>/props.json
   *
   * For example, if a file has a saved header row span of 2, but the
   * command-line arguments explicitly included `--header-row-span 3`,
   * then setting header_span to 3 and setting overrides.header_row_span
   * ensures that the value of 3 is used
   */
  struct {
    unsigned char header_row_span : 1;
    unsigned char skip_head : 1;
    unsigned char max_column_count : 1;
    unsigned char malformed_utf8_replacement : 1;
    unsigned char _ : 4;
  } option_overrides;

#ifdef ZSV_EXTRAS
  struct {
    /**
     * min number of rows between progress callback calls
     */
    size_t rows_interval;

    /**
     * min number of seconds b/w callback calls
     */
    unsigned int seconds_interval;

    /**
     * Progress callback, called periodically to provide progress updates
     */
    zsv_progress_callback callback;

    /**
     * Context passed to the callback, when the callback is invoked
     */
    void *ctx;
  } progress;

  struct {
    /**
     * Optional callback. If set, it is called by zsv_finish()
     */
    zsv_completed_callback callback;
    /**
     * Context passed to the callback, when the callback is invoked
     */
    void *ctx;
  } completed;

  /**
   * maximum number of rows to parse (including any header rows)
   */
  size_t max_rows;

  /**
   * If non-zero, automatically apply overwrites located in
   * /path/to/.zsv/data/my-data.csv/overwrite.sqlite3 for a given
   * input /path/to/my-data.csv
   *
   * This flag is only used by zsv_new_with_properties()
   * if using zsv_new(), this flag is ignored (use the `overwrite` structure instead)
   */
  char overwrite_auto;

  /**
   * Optional cell-level values that overwrite data returned to the caller by the API
   * Use when not using overwrite_auto together with zsv_new_with_properties()
   */
  struct zsv_opt_overwrite overwrite;

#endif /* ZSV_EXTRAS */
};

#endif
