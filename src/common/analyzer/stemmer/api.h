// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char symbol;

/* Or replace 'char' above with 'short' for 16 bit characters.

   More precisely, replace 'char' with whatever type guarantees the
   character width you need. Note however that sizeof(symbol) should divide
   HEAD, defined in header.h as 2*sizeof(int), without remainder, otherwise
   there is an alignment problem. In the unlikely event of a problem here,
   consult Martin Porter.

*/

struct SN_env {
    symbol *p;
    int c;
    int l;
    int lb;
    int bra;
    int ket;
    symbol **S;
    int *I;
    unsigned char *B;
};

extern struct SN_env *SN_create_env(int S_size, int I_size, int B_size);
extern void SN_close_env(struct SN_env *z, int S_size);

extern int SN_set_current(struct SN_env *z, int size, const symbol *s);

#ifdef __cplusplus
}

#endif
