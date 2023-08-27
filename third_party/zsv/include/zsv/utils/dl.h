/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_DL_H
#define ZSV_DL_H

#ifdef _WIN32
#include <zsv/utils/win/dl.h>
#define DL_SUFFIX "dll"
#else
#include <dlfcn.h>
#define DL_SUFFIX "so"
#endif

void (*zsv_dlsym(void *restrict handle, const char *restrict name))(void);

#endif
