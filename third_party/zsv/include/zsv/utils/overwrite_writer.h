#include <zsv.h>

enum zsvsheet_mode {
  zsvsheet_mode_add,
  zsvsheet_mode_remove,
  zsvsheet_mode_clear,
  zsvsheet_mode_list,
  zsvsheet_mode_bulk,
};

struct zsv_overwrite {
  struct zsv_overwrite_ctx *ctx;
  struct zsv_overwrite_data *overwrite;
  zsv_csv_writer writer;
  enum zsv_status (*next)(struct zsv_overwrite *data);
  enum zsvsheet_mode mode;
  unsigned char force : 1;
  unsigned char a1 : 1;
  unsigned char all : 1;
  unsigned char *bulk_file;
  // bulk indexes
  size_t row_ix;
  size_t col_ix;
  size_t val_ix;
  size_t old_value_ix;
  size_t timestamp_ix;
  size_t author_ix;
};

struct zsv_overwrite_args {
  char *filepath;
  enum zsv_status (*next)(struct zsv_overwrite *data);
  // options
  unsigned char force : 1;
  unsigned char a1 : 1;
  unsigned char all : 1;
  unsigned char *bulk_file;
  struct zsv_overwrite_data *overwrite;
  // commands
  enum zsvsheet_mode mode;
};

struct zsv_overwrite *zsv_overwrite_writer_new(struct zsv_overwrite_args *args, struct zsv_overwrite_opts *ctx_opts);

void zsv_overwrite_writer_delete();

enum zsv_status zsv_overwrite_writer_add(struct zsv_overwrite *data);

enum zsv_status zsv_overwrite_writer_remove(struct zsv_overwrite *data);

enum zsv_status zsv_overwrite_writer_bulk(struct zsv_overwrite *data);

enum zsv_status zsv_overwrite_writer_clear(struct zsv_overwrite *data);
