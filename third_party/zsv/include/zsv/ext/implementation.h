/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_EXT_PRIVATE_H
#define ZSV_EXT_PRIVATE_H

#include <zsv/ext.h>

/**
 * @file zsv_ext.h
 * @brief ZSV extension implementation specification
 * @defgroup zsv_extension implementation specification
 * @ingroup ZSV
 * @{
 */

/**
 * Overridable compiler flag ZSV_EXT=1 indicates that we are compiling a ZSV
 * extension
 */
#ifndef ZSV_EXT
# define ZSV_EXT 1
#endif

ZSV_BEGIN_DECL

/**
 * To build a zsv extension:
 * - implement the following required functions:
 *   `zsv_ext_id`
 *   `zsv_ext_init`
 *   `zsv_ext_main`
 *
 * - optionally, implement the following recommended functions:
 *  `zsv_ext_help`
 *  `zsv_ext_errstr`
 *  `zsv_ext_version`
 *  `zsv_ext_license`
 *
 * - on Windows, be sure to decorate your function definitions
 *   with ZSV_EXT_EXPORT
 */

#ifdef _WIN32
#define ZSV_EXT_EXPORT __declspec(dllexport)
#elif __EMSCRIPTEN__
#include <emscripten.h>
#define ZSV_EXT_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define ZSV_EXT_EXPORT
#endif

#include "implementation_private.h"

/** @} */
ZSV_END_DECL
#endif
