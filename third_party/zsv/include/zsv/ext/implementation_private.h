/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

/**
 * @file zsv_ext_private.h
 * @brief ZSV extension implementation specification
 * @defgroup zsv_extension implementation specification
 * @ingroup ZSV
 * @{
 */

/**
 * Return a module identifier, which must be a 2-char string
 *
 * Required. Any sub-command that starts with this id followed by a dash
 * will be passed on to this module
 *
 * Your DLL or shared lib filename must correspondingly be "zsvext<id>"
 *
 * For example, to implement sub-commands that begin with "my-":
 *   - your function zsv_ext_id() would return "my"
 *   - your library would be named zsvextmy.dll (or .so or .dylib)
 */
ZSV_EXT_EXPORT
const char *zsv_ext_id();

/**
 * Initialize your module
 *
 * Called once, when your library is loaded
 *
 * @param callbacks Pointer to struct holding zsvlib function pointers
 * @return Zero on success, non-zero on fail
 */
ZSV_EXT_EXPORT
enum zsv_ext_status zsv_ext_init(struct zsv_ext_callbacks *callbacks, zsv_execution_context ctx);

/**
 * Get your extension's internal error code
 *
 * In the case that any of your module functions return zsv_ext_status_error
 * this function is called to obtain your extension's internal error code
 *
 * @param context Execution context
 * @return Error code
 */
ZSV_EXT_EXPORT
int zsv_ext_errcode(zsv_execution_context context);

/**
 * Get an error message
 *
 * In the case that any of your module functions return zsv_ext_status_error,
 * this function is called to obtain the message corresponding to the internal
 * error code returned by zsv_ext_errcode()
 *
 * @param context Execution context
 * @param int Return code from zsv_ext_errcode()
 * @return Error message to display
 */
ZSV_EXT_EXPORT
char *zsv_ext_errstr(zsv_execution_context context, int err);

/**
 * Free an error message
 *
 * Called after obtaining any non-NULL result from `zsv_ext_errstr()`
 *
 * @param errstr The string returned from zsv_ext_errstr()
 */
ZSV_EXT_EXPORT
void zsv_ext_errfree(char *errstr);

/**
 * Exit your module
 *
 * Called once, when your library is unloaded
 *
 * @return Zero on success, non-zero on fail
 */
ZSV_EXT_EXPORT
enum zsv_ext_status zsv_ext_exit();

/**
 * Help message. Displayed when user enters any command beginning with
 *   `zsv help <module-id>`
 *
 * @param argc Number of arguments
 * @param argv Arguments
 */
ZSV_EXT_EXPORT
const char * const *zsv_ext_help(int argc, const char *argv[]);

/**
 * License message. Displayed when user enters any command beginning with
 *   `zsv license <module-id>`
 *
 */
ZSV_EXT_EXPORT
const char * const *zsv_ext_license();

/**
 * Version message. Displayed when user enters any command beginning with
 *   `zsv version <module-id>`
 *
 */
ZSV_EXT_EXPORT
const char * const *zsv_ext_version();
