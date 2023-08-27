/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_ARG_H
#define ZSV_ARG_H

#define ZSV_OPTS_SIZE_MAX 16

#include <zsv/common.h>

/* havearg(): case-insensitive partial arg matching */
char havearg(const char *arg,
             const char *form1, size_t min_len1,
             const char *form2, size_t min_len2);

/**
 * set or get default parser options
 */
void zsv_set_default_opts(struct zsv_opts);

struct zsv_opts zsv_get_default_opts(void);

void zsv_clear_default_opts(void);

#  ifdef ZSV_EXTRAS

/**
 * set the default option progress callback (e.g. from wasm where `struct zsv_opts`
 * cannot be independently accessed)
 * @param cb callback to call
 * @param ctx pointer passed to callback
 * @param frequency number of rows to parse between progress calls
 */
void zsv_set_default_progress_callback(zsv_progress_callback cb, void *ctx, size_t rows_interval, unsigned int seconds_interval);

/**
 * set the default option completed callback (e.g. from wasm where `struct zsv_opts`
 * cannot be independently accessed)
 * @param cb callback to call
 * @param ctx pointer passed to callback
 */
void zsv_set_default_completed_callback(zsv_completed_callback cb, void *ctx);

#  endif

/**
 * Convert common command-line arguments to zsv_opts
 * Return new argc/argv values with processed args stripped out
 * Initializes opts_out with `zsv_get_default_opts()`, then with
 * the below common options if present:
 *     -B,--buff-size <N>
 *     -c,--max-column-count <N>
 *     -r,--max-row-size <N>
 *     -t,--tab-delim
 *     -O,--other-delim <C>
 *     -q,--no-quote
 *     -S,--keep-blank-headers: disable default behavior of ignoring leading blank rows
 *     -d,--header-row-span <n>: apply header depth (rowspan) of n
 *     -v,--verbose
 *
 * @param  argc      count of args to process
 * @param  argv      args to process
 * @param  argc_out  count of unprocessed args
 * @param  argv_out  array of unprocessed arg values. Must be allocated by caller
 *                   with size of at least argc * sizeof(*argv)
 * @param  opts_out  options, updated to reflect any processed args
 * @param  opts_used optional; if provided:
 *                   - must point to >= ZSV_OPTS_SIZE_MAX bytes of storage
 *                   - all used options will be returned in this string
 *                   e.g. if -R and -q are used, then opts_used will be set to:
 *                     "     q R   "
 * @return           zero on success, non-zero on error
 */
enum zsv_status zsv_args_to_opts(int argc, const char *argv[],
                                 int *argc_out, const char **argv_out,
                                 struct zsv_opts *opts_out,
                                 char *opts_used
                                 );

/**
 * Fetch the next arg, if it exists, else print an error message
 * The argc_i argument does not need to be valid; it will be checked
 * against argc.
 *
 * Example:
 *   const char *value = zsv_next_arg(++arg_i, argc, argv, &err);
 *
 * @param  argc_i index of next arg to test
 * @param  argc   total arg count
 * @param  argv   args to process
 * @param  err    return non-zero in the case of error
 * @return        next argument, if it exists, else NULL
 */
const char *zsv_next_arg(int arg_i, int argc, const char *argv[], int *err);

#endif
