/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_JSON_H
#define ZSV_JSON_H

/**
 * Convert a string to json (including the surrounding quotes)
 * Returns newly-allocated string (caller must free())
 */
unsigned char *zsv_json_from_str(const unsigned char *s);
unsigned char *zsv_json_from_str_n(const unsigned char *s, size_t len);

#endif
