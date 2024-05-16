include(CheckCXXSourceCompiles)

set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})

set(SSE4PROG "

#include<smmintrin.h>
int main(){
__m128 x=_mm_set1_ps(0.5);
x=_mm_dp_ps(x,x,0x77);
return _mm_movemask_ps(x);
}")

set(AVXPROG "

#include<immintrin.h>
int main(){
__m128 x=_mm_set1_ps(0.5);
x=_mm_permute_ps(x,1);
return _mm_movemask_ps(x);
}")

set(AVX2PROG "

#include<immintrin.h>
int main(){
__m256i x=_mm256_set1_epi32(5);
x=_mm256_add_epi32(x,x);
return _mm256_movemask_epi8(x);
}")

if(MSVC)
	message(STATUS "TEST 2")
	set(CMAKE_REQUIRED_FLAGS "/EHsc /arch:SSE2")
	check_cxx_source_compiles("${SSE4PROG}" SUPPORT_SSE42)
	message(STATUS "SUPPORT_SSE42 ${SUPPORT_SSE42}")
	set(CMAKE_REQUIRED_FLAGS "/EHsc /arch:AVX")
	check_cxx_source_compiles("${AVXPROG}" SUPPORT_AVX)
	message(STATUS "SUPPORT_AVX ${SUPPORT_AVX}")
	set(CMAKE_REQUIRED_FLAGS "/EHsc /arch:AVX2")
	check_cxx_source_compiles("${AVX2PROG}" SUPPORT_AVX2)
	message(STATUS "SUPPORT_AVX2 ${SUPPORT_AVX2}")
else()
	set(CMAKE_REQUIRED_FLAGS "-march=native -msse4.2")
	check_cxx_source_compiles("${SSE4PROG}" SUPPORT_SSE42)
	set(CMAKE_REQUIRED_FLAGS "-march=native -mavx")
	check_cxx_source_compiles("${AVXPROG}" SUPPORT_AVX)
	set(CMAKE_REQUIRED_FLAGS "-march=native -mavx2")
	check_cxx_source_compiles("${AVX2PROG}" SUPPORT_AVX2)
endif()	

set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
	
