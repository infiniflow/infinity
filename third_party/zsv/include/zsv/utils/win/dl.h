/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_DL_WIN_H
#define ZSV_DL_WIN_H

#ifdef _WIN32

#include <windows.h>

#define RTLD_LAZY 0
void *dlopen(const char *dll_name, int flags);

#define dlsym(x, y) (GetProcAddress((HINSTANCE)(x), (y)))

int dlclose(void *handle);

#endif

#endif
