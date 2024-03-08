/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef __ZSV_UTIL_CLOCK_H__
#define __ZSV_UTIL_CLOCK_H__

#include <time.h>
#include <stdio.h>

void zsv_clocks_begin();
void zsv_clocks_end();

size_t
zsv_fread_clock(void *restrict ptr, size_t size, size_t nitems,
                FILE *restrict stream);

size_t
zsv_fwrite_clock(const void *restrict ptr, size_t size, size_t nitems,
                 FILE *restrict stream);

int
zsv_fflush_clock(FILE *stream);


#if USE_CLOCK

#define ZSV_FREAD zsv_fread_clock
#define ZSV_FWRITE zsv_fwrite_clock
#define ZSV_FFLUSH zsv_fflush_clock

#else

#define ZSV_FREAD fread
#define ZSV_FWRITE fwrite
#define ZSV_FFLUSH fflush

#endif

#endif
