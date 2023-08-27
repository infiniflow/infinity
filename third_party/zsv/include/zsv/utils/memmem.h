#ifndef ZSV_MEMMEM_H
#define ZSV_MEMMEM_H

#ifdef NO_MEMMEM

#include <stdint.h>
void *zsvmemmem(const void *h0, size_t k, const void *n0, size_t l);

#define memmem zsvmemmem

#endif

#endif
