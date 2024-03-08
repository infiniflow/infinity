/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_DB_H
#define ZSV_DB_H

#include <sqlite3.h>
#include <jsonwriter.h>

// zsv_dbtable2json: convert a db table to json
// @limit: if nonzero, limit number of rows processed
int zsv_dbtable2json(sqlite3 *db, const char *tname, jsonwriter_handle jsw, size_t limit);

#endif
