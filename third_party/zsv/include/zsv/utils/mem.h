/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_MEM_H
#define ZSV_MEM_H

/* zsv_memdup(): return copy with double-NULL terminator. caller must free() */
void *zsv_memdup(const void *src, size_t n);

#ifndef MEMDUP
#define MEMDUP zsv_memdup
#endif

#endif
