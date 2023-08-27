#ifndef ZSV_CACHE_H
#define ZSV_CACHE_H

/**
 * Overridable constants: ZSV_CACHE_PREFIX and ZSV_TEMPFILE_SUFFIX
 */

#ifndef ZSV_CACHE_PREFIX
#define ZSV_CACHE_PREFIX "zsv"
#endif

#ifndef ZSV_TEMPFILE_SUFFIX
#define ZSV_TEMPFILE_SUFFIX ".~zsv"
#endif

/**
 * Cache path definitions
 */
#ifdef _WIN32
#define ZSV_CACHE_DIR "."ZSV_CACHE_PREFIX "\\data"
#else
#define ZSV_CACHE_DIR "."ZSV_CACHE_PREFIX "/data"
#endif

#define ZSV_CACHE_PROPERTIES_NAME "props"


/**
 * Return the folder or file path to the cache for a given data file
 * Caller must free the returned result
 */
unsigned char *zsv_cache_path(const unsigned char *data_filepath,
                              const unsigned char *cache_filename,
                              char temp_file);

enum zsv_cache_type {
  zsv_cache_type_property = 1,
  zsv_cache_type_tag
};

unsigned char *zsv_cache_filepath(const unsigned char *data_filepath,
                                  enum zsv_cache_type type, char create_dir,
                                  char temp_file);

int zsv_cache_print(const unsigned char *filepath, enum zsv_cache_type ctype,
                    const unsigned char *default_value);

int zsv_cache_remove(const unsigned char *filepath, enum zsv_cache_type ctype);

int zsv_modify_cache_file(const unsigned char *filepath,
                          enum zsv_cache_type ctype,
                          const unsigned char *json_value1,
                          const unsigned char *json_value2,
                          const unsigned char *filter
                          );

#endif
