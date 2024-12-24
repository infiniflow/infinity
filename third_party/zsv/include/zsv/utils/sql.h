#ifndef ZSV_UTILS_SQL_H
#define ZSV_UTILS_SQL_H

typedef struct zsv_sqlite3_db *zsv_sqlite3_db_t;

struct zsv_sqlite3_dbopts {
  unsigned char in_memory : 1;
  unsigned char _ : 7;
};

#endif
