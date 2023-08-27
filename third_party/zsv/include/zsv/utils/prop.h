#ifndef ZSV_PROP_H
#define ZSV_PROP_H

#include <yajl_helper/yajl_helper.h>

struct zsv_file_properties {
  unsigned int skip;
  unsigned int header_span;
  int err;

  /* to process custom properties, you can provide your own handler and context */
  struct {
    int (*handler)(void *ctx, const char *path, struct json_value *value);
    void *ctx;
  } custom_property;

  /* flags used by parser only to indicate whether property was specified */
  unsigned int skip_specified:1;
  unsigned int header_span_specified:1;
  unsigned int _:6;
};

/**
 * Load cached file properties into a zsp_opts and/or zsv_file_properties struct
 * If cmd_opts_used is provided, then do not set any zsv_opts values, if the
 * corresponding option code is already present in cmd_opts_used, and instead
 * print a warning to stderr
 *
 * @param data_filepath            required file path
 * @param opts (optional)          parser options to load
 * @param fp (optional)            parsed file properties
 * @param cmd_opts_used (optional) cmd option codes to skip + warn if found
 * @return zsv_status_ok on success
 */
enum zsv_status zsv_cache_load_props(const char *data_filepath,
                                     struct zsv_opts *opts,
                                     struct zsv_file_properties *fp,
                                     const char *cmd_opts_used);

/**
 * Create a new properties parser
 */
struct zsv_properties_parser;
struct zsv_properties_parser *zsv_properties_parser_new(struct zsv_file_properties *fp);

/**
 * Finished parsing
 */
enum zsv_status zsv_properties_parse_complete(struct zsv_properties_parser *parser);

/**
 * Clean up
 */
enum zsv_status zsv_properties_parser_destroy(struct zsv_properties_parser *parser);

/**
 * zsv_new_with_properties(): use in lieu of zsv_new() to also merge zsv options
 * with any saved properties (such as rows_to_ignore or header_span) for the
 * specified input file. In the event that saved properties conflict with a
 * command-line option, the command-line option "wins" (the property value is
 * ignored), but a warning is printed
 *
 * @param opts       parser options. see `zsv_new()`
 * @param input_path path of file whose zsv properties should be loaded. this
 *                   param is used solely for loading properties and has no
 *                   impact on the data that is actually parsed, which is
 *                   determined by opts->stream
 * @param opts_used  string specifyig which other command-line options were
 *                   already used (may be useful to differentiate between
 *                   unspecified default values vs specified values)
 * @param handle_out returns zsv parser handle, or NULL on fail
 */
enum zsv_status zsv_new_with_properties(struct zsv_opts *opts,
                                        const char *input_path,
                                        const char *opts_used,
                                        zsv_parser *handle_out
                                        );

/**
 * If you are building your own CLI and incorporating zsv CLI commands into it,
 * the `prop` command can be customized by providing your own function
 * for determining whether a file in the property cache is a property file,
 * which can be set using zsv_prop_get_or_set_is_prop_file()
 *
 * @param is_prop_file: your function, that returns non-zero if the given file entry
 *                             is a property file. If NULL, is set to zsv_is_prop_file
 * @param max_depth   : maximum depth of any property file. if is_prop_file was NULL,
 *                      max_depth is set to 1
 */

/**
 * zsv_new_with_custom_properties(): same as zsv_new_with_properties(), but
 * with a custom-provided `struct zsv_file_properties` (for example, with a non-NULL
 * custom_property.handler and custom_property.ctx)
 */
enum zsv_status zsv_new_with_custom_properties(struct zsv_opts *opts,
                                               struct zsv_file_properties *fp,
                                               const char *input_path,
                                               const char *opts_used,
                                               zsv_parser *handle_out
                                        );

#include "dirs.h"

struct zsv_dir_filter; /* opaque structure for internal use */
struct zsv_dir_filter *
zsv_prop_get_or_set_is_prop_file(
                                 int (*is_prop_file)(struct zsv_foreach_dirent_handle *, size_t),
                                 int max_depth,
                                 char set
                                 );

/**
 * If you provide your own is_prop_file() function and you also want to include any
 * zsv property file, your is_prop_file() can call zsv_is_prop_file()
 */
int zsv_is_prop_file(struct zsv_foreach_dirent_handle *h, size_t depth);
#endif
