/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_ERR_H
#define ZSV_ERR_H

int zsv_app_printerr(const char *appname, int eno, const char *fmt, ... );

#ifdef APPNAME
# define zsv_printerr(... ) zsv_app_printerr(APPNAME, __VA_ARGS__)
#else
# pragma message("zsv_printerr defined without APPNAME")
# pragma message("to avoid this, include err.h after defining APPNAME to quoted string")
# define zsv_printerr(... ) zsv_app_printerr(0x0, __VA_ARGS__)
#endif


#endif
