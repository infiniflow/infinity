#ifndef ZSVSHEET_H
#define ZSVSHEET_H

/* custom sheet handler id */
typedef int zsvsheet_proc_id_t;

typedef struct zsvsheet_proc_context *zsvsheet_proc_context_t;

typedef enum zsvsheet_status {
  zsvsheet_status_ok = 0,
  zsvsheet_status_error,
  zsvsheet_status_ignore,
  zsvsheet_status_duplicate,
  zsvsheet_status_memory,
  zsvsheet_status_exit,
  zsvsheet_status_busy,
} zsvsheet_status;

typedef struct zsvsheet_context *zsvsheet_context_t;
typedef struct zsvsheet_subcommand_context *zsvsheet_subcommand_context_t;

typedef void *zsvsheet_buffer_t;
// int zsvsheet_ext_keypress(zsvsheet_proc_context_t);

typedef struct zsvsheet_transformation *zsvsheet_transformation;
/**
 * Transformation options passed to zsvsheet_push_transformation
 */
struct zsvsheet_buffer_transformation_opts {
  /**
   * Caller supplied context; can be retrieved with ext_sheet_transformation_user_context(trn).
   *
   * This is free'd by the library after on_done is called unless it is NULL.
   */
  void *user_context;
  /**
   * This is called each time a row is parsed and available.
   *
   * It can be used to write out a new row immediately or contribute to a calculation
   * where the partial results are stored in the context.
   *
   * Note that when doing a reduction which does not produce output until the end;
   * it is best to write out a header row in the row_handler and flush the writer. Otherwise the
   * main thread will be blocked waiting for something to display.
   *
   * Once some data has been written to the output file execution is moved to a background thread.
   *
   * The parser can be retrieved with ext_sheet_transformation_parser(trn),
   * the output file writer can be retrieved with ext_sheet_transformation_writer(trn),
   * the context can be retrieved with ext_sheet_transformation_user_context(trn),
   * and so on
   */
  void (*row_handler)(zsvsheet_transformation trn);
  /**
   * Called when the parser has no more data or was cancelled.
   *
   * This can be used to free resources, complete a calculation or to write out one
   * or more final rows.
   *
   * For small input files this may be executed in the main thread, for larger ones
   * it will be done in a background thread. So if an operation done inside this handler
   * can take a long time even on small inputs then it could block the main thread.
   */
  void (*on_done)(zsvsheet_transformation trn);
};

struct zsvsheet_rowcol {
  size_t row;
  size_t col;
};

#endif
