#ifndef ZSV_PROP_H
#define ZSV_PROP_H

#include <zsv/common.h>
#include <yajl_helper/yajl_helper.h>

struct zsv_file_properties {
  enum zsv_status stat;
  unsigned int skip;
  unsigned int header_span;

  // flags used by parser only to indicate whether property was specified
  unsigned int skip_specified : 1;
  unsigned int header_span_specified : 1;
  unsigned int _ : 6;
};

struct zsv_prop_handler {
  // handler should return 0 on success or non-zero on error. a non-zero return code will abort the property processing
  int (*handler)(void *property_parser, const char *property_name, struct json_value *value);
  void *ctx;
};

void *zsv_properties_parser_get_custom_ctx(void *property_parser);
const unsigned char *zsv_properties_parser_get_filepath(void *property_parser);
struct zsv_opts *zsv_properties_parser_get_opts(void *property_parser_);

/**
 * set or get default custom property handler
 */
void zsv_set_default_custom_prop_handler(struct zsv_prop_handler custom_prop_handler);

struct zsv_prop_handler zsv_get_default_custom_prop_handler(void);

void zsv_clear_default_custom_prop_handler(void);

/**
 * Load cached file properties into a zsp_opts and/or zsv_file_properties struct
 *
 * @param data_filepath            required file path
 * @param opts (optional)          parser options to load
 * @param custom_prop              custom property handler
 * @return struct zsv_file_properties, with .stat set to zsv_status_ok on success
 */
struct zsv_file_properties zsv_cache_load_props(const char *data_filepath, struct zsv_opts *opts,
                                                struct zsv_prop_handler *custom_prop);

/**
 * Create a new properties parser
 */
struct zsv_properties_parser;
struct zsv_properties_parser *zsv_properties_parser_new(const unsigned char *path,
                                                        struct zsv_prop_handler *custom_prop_handler,
                                                        struct zsv_file_properties *fp, struct zsv_opts *opts);

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
 * @param cust_prop  optional custom file property handler
 * @param input_path path of file whose zsv properties should be loaded. this
 *                   param is used solely for loading properties and has no
 *                   impact on the data that is actually parsed, which is
 *                   determined by opts->stream
 * @param handle_out returns zsv parser handle, or NULL on fail
 */
enum zsv_status zsv_new_with_properties(struct zsv_opts *opts, struct zsv_prop_handler *custom_prop,
                                        const char *input_path, zsv_parser *handle_out);

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

#include "dirs.h"

struct zsv_dir_filter; /* opaque structure for internal use */
struct zsv_dir_filter *zsv_prop_get_or_set_is_prop_file(int (*is_prop_file)(struct zsv_foreach_dirent_handle *, size_t),
                                                        int max_depth, char set);

/**
 * If you provide your own is_prop_file() function and you also want to include any
 * zsv property file, your is_prop_file() can call zsv_is_prop_file()
 */
int zsv_is_prop_file(struct zsv_foreach_dirent_handle *h, size_t depth);
#endif
