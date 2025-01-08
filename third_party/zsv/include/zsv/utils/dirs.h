/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_DIRS_H
#define ZSV_DIRS_H

#include <stdio.h>

/* Maximum length of file name */
#if !defined(FILENAME_MAX)
#define FILENAME_MAX MAX_PATH
#endif

/* file slash */
#if !defined(FILESLASH)
#ifdef _WIN32
#define FILESLASH '\\'
#else
#define FILESLASH '/'
#endif
#endif

/**
 * Get os-independent configuration file path
 * prefix should be determined at compile time e.g. /usr/local or ""
 * @return length written to buff, or 0 if failed
 */
size_t zsv_get_config_dir(char *buff, size_t buffsize, const char *prefix);

/**
 * Get the path of the current executable
 */
size_t zsv_get_executable_path(char *buff, size_t buffsize);

/**
 * Get current user's home dir, without trailing slash
 * On win, any backslashes are replaced with fwd slash
 *   ex: zsv_get_home_dir(char[MAX_PATH], MAX_PATH)
 * returns 0 if no home dir could be found
 * returns > 0 and < bufflen on success
 * returns > 0 and >= bufflen if buffer was too small
 */
int zsv_get_home_dir(char *buff, size_t bufflen);

/**
 * Check if a directory exists
 * return true (non-zero) or false (zero)
 */
int zsv_dir_exists(const char *path);

/**
 * mkdir that works with UNC paths on Win
 * if fail, use zsv_perror() instead of perror()
 */
int zsv_mkdir(const char *path);

/**
 * Make a directory, as well as any intermediate dirs
 * return zero on success
 */
int zsv_mkdirs(const char *path, char path_is_filename);

/**
 * Recursively remove a directory and all of its contents
 * return zero on success
 */
int zsv_remove_dir_recursive(const unsigned char *path);

#include <sys/stat.h>

struct zsv_foreach_dirent_handle {
  const char *parent;           /* name of the parent directory */
  const char *entry;            /* file / dir name of current entry being processed */
  const char *parent_and_entry; /* parent + entry separated by file separator */
  const struct stat stat;       /* stat of current entry */

  void *ctx; /* caller-provided context to pass to handler */

  unsigned char verbose : 1;
  unsigned char is_dir : 1;     /* non-zero if this entry is a directory */
  unsigned char no_recurse : 1; /* set to 1 when handling a dir to prevent recursing into it */
  unsigned char _ : 5;
};

typedef int (*zsv_foreach_dirent_handler)(struct zsv_foreach_dirent_handle *h, size_t depth);

/**
 * Recursively process entries (files and folders) in a directory
 *
 * @param dir_path    : path of directory to begin processing children of
 * @param max_depth   : maximum depth to recurse, or 0 for no maximum
 * @param handler     : caller-provided entry handler. return 0 on success, non-zero on error
 * @param ctx         : pointer passed to the handler
 * @param verbose     : non-zero for verbose output
 *
 * returns error
 */
int zsv_foreach_dirent(const char *dir_path, size_t max_depth, zsv_foreach_dirent_handler handler, void *ctx,
                       char verbose);

struct zsv_dir_filter {
  zsv_foreach_dirent_handler
    filter;         /* filter function; return 1 to process this node. if node is dir, return 0 to skip entirely */
  size_t max_depth; /* max depth to recurse */
  void *ctx;        /* pointer to pass to filter function */
};

/**
 * Convert directory contents into a single JSON file
 * Output schema is a dictionary where key = path and value = contents
 * Files named with .json suffix will be exported as JSON (content must be valid JSON)
 * Files named with any other suffix will be exported as a single string value (do not try with large files)
 *
 * @param parent_dir      : directory to export
 * @param output_filename : file path to output to, or NULL to output to stdout
 * @param file_filter     : filter determining which files to export
 */
int zsv_dir_to_json(const unsigned char *parent_dir, const unsigned char *output_filename,
                    struct zsv_dir_filter *file_filter, unsigned char verbose);

/**
 * Convert a JSON stream into file and directory contents
 * This function is the inverse of zsv_dir_to_json()
 * Output schema is a dictionary where key = path and value = contents
 * Files named with .json suffix will be exported as JSON (content must be valid JSON)
 * Files named with any other suffix will be exported as a single string value (do not try with large files)
 *
 * @param target_dir : directory to create / import into
 * @param        : file path to output to, or NULL to output to stdout
 * @flags        : ZSV_DIR_FLAG_xxx values as defined below
 *
 * Returns 0 on success, non-zero on error
 */

#define ZSV_DIR_FLAG_FORCE 1 /* overwrite target files if they already exist */
#define ZSV_DIR_FLAG_DRY 2   /* dry run, output names of files that would be created/overwritten */

int zsv_dir_from_json(const unsigned char *target_dir, FILE *fsrc,
                      unsigned int flags, // ZSV_DIR_FLAG_XX
                      unsigned char verbose);

#endif
