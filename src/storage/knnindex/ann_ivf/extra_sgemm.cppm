//
// Created by yzq on 11/6/23.
//
module;
#include "/home/yzq/GitHub/infinity/third_party/mlas/inc/mlas.h"
#ifndef FINTEGER
#define FINTEGER long
#endif
export module extra_sgemm;
namespace infinity{
export int sgemm_(
    const char* transa,
    const char* transb,
    FINTEGER* m,
    FINTEGER* n,
    FINTEGER* k,
    const float* alpha,
    const float* a,
    FINTEGER* lda,
    const float* b,
    FINTEGER* ldb,
    float* beta,
    float* c,
    FINTEGER* ldc){


    //return dnnl::impl::cpu::sgemm_compute(transa,transb,(dim_t*)m,(dim_t*)n,(dim_t*)k,a,(dim_t*)lda,b,(dim_t*)ldb,beta,c,(dim_t*)ldc);
    //return (int)dnnl::sgemm('n','t',*n,*m,*k,*alpha,b,*ldb,a,*lda,*beta,c,*m);
    //return (int)dnnl::sgemm(*transb,*transa,*n,*m,*k,*alpha,b,*ldb,a,*lda,*beta,c,*ldc);
    MlasGemm(
        CblasNoTrans,
        CblasTrans,
        *n,
        *m,
        *k,
        *alpha,
        b,
        *ldb,
        a,
        *lda,
        *beta,
        c,
        *m,
        nullptr);
    return 0;

}
}
