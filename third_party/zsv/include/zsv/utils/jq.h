#ifndef LQJQ_INTERNAL_H
#define LQJQ_INTERNAL_H

#include <ctype.h>
#include <jq.h>

enum zsv_jq_status {
  zsv_jq_status_ok = 0,
  zsv_jq_status_compile,
  zsv_jq_status_memory,
  zsv_jq_status_error
};

size_t zsv_jq_fwrite1(void *restrict FILE_ptr, const void *restrict buff, size_t len);

struct jv_to_json_ctx {
  size_t (*write1)(void *restrict ctx, const void *restrict buff, size_t len); // e.g. zsv_jq_fwrite1
  void *ctx; // e.g. FILE *
  int flags; // passed on to jv_dumpf / jv_dump_string
};

void jv_to_json_w_ctx(jv value, void *jv_to_json_ctx);

typedef struct zsv_jq_data * zsv_jq_handle;

zsv_jq_handle zsv_jq_new(const unsigned char *filter,
                       void (*func)(jv, void *), void *ctx,
                       enum zsv_jq_status *statusp);

// for streaming parsing
zsv_jq_handle zsv_jq_new_stream(const unsigned char *filter,
                              void (*func)(jv, void *), void *ctx,
                              enum zsv_jq_status *statusp);

enum zsv_jq_status zsv_jq_parse_file(zsv_jq_handle h, FILE *f);

enum zsv_jq_status zsv_jq_parse(zsv_jq_handle restrict h, const void * restrict s, size_t len);

size_t zsv_jq_write(const char *s, size_t n, size_t m, zsv_jq_handle h);

enum zsv_jq_status zsv_jq_finish(zsv_jq_handle h);

void zsv_jq_trace(zsv_jq_handle h, FILE *);

void zsv_jq_delete(zsv_jq_handle h);

// helper functions
void jv_to_lqjq(jv value, void *zsv_jq_handle);

void jv_to_csv(jv value, void *file);

void jv_to_bool(jv value, void *char_result);

void jv_to_txt(jv value, void *file);

struct jv_to_csv_multi_ctx {
  FILE *(*get_file)(void *ctx);
  void *ctx;
};

// jv_to_csv_multi: write csv rows to different files
void jv_to_csv_multi(jv value, void *jv_to_csv_multi_ctx);

void jv_to_json_func(jv value, void *jv_to_json_func_dat);

#endif
