// Copyright (c) 2016, the SDSL Project Authors (https://github.com/xxsds/sdsl-lite).
// All rights reserved. Please see the AUTHORS below for details.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the <organization> nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// SDSL Project Authors:
//
// Simon Gog <simon.gog@gmail.com>
// Matthias Petri <matthias.petri@gmail.com>
//
// Contributors:
//
// Alexander Diehm
// Christian Ocker
// Francisco Montoto
// Johannes Bader
// Jouni Siren
// Maike Zwerger
// Markus Brenner
// Shanika Kuruppu
// Stefan Arnold
// Timo Beller
// Uwe Baier

/*! \file int_vector.hpp
    \brief int_vector.hpp contains the sdsl::int_vector class.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_INT_VECTOR
#define INCLUDED_SDSL_INT_VECTOR

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file bits.hpp
    \brief bits.hpp contains the sdsl::bits class.
	\author Simon Gog
*/
#ifndef INCLUDED_SDSL_BITS
#define INCLUDED_SDSL_BITS

#include <stdint.h> // for uint64_t uint32_t declaration
#include <iostream> // for cerr
#include <cassert>
#ifdef __SSE4_2__
#include <xmmintrin.h>
#endif
#ifdef __BMI2__
#include <x86intrin.h>
#endif

#ifdef WIN32
#include <ciso646>
#endif

#ifdef __cpp_constexpr
#   if __cpp_constexpr >= 201304
#       define SDSL_CONSTEXPR constexpr
#   else
#       define SDSL_CONSTEXPR
#   endif
#else
#   define SDSL_CONSTEXPR
#endif

//! Namespace for the succinct data structure library.
namespace sdsl {

//! A helper class for bitwise tricks on 64 bit words.
/*!
	bits is a helper class for bitwise tricks and
	techniques. For the basic tricks and techiques we refer to Donald E. Knuth's
	"The Art of Computer Programming", Volume 4A, Chapter 7.1.3 and
	the informative website of Sean E. Anderson about the topic:
	http://www-graphics.stanford.edu/~seander/bithacks.html .

	We have added new functions like: cnt11 and sel11.

	All members of this class are static variables or methods.
	This class cannot be instantiated.

	\author Simon Gog
 */
template <typename T = void>
struct bits_impl {
    bits_impl() = delete;
    //! 64bit mask with all bits set to 1.
    constexpr static uint64_t all_set{-1ULL};

    //! This constant represents a de Bruijn sequence B(k,n) for k=2 and n=6.
    /*! Details for de Bruijn sequences see
       http://en.wikipedia.org/wiki/De_bruijn_sequence
       deBruijn64 is used in combination with the
       array lt_deBruijn_to_idx.
    */
    constexpr static uint64_t deBruijn64{0x0218A392CD3D5DBFULL};

    //! This table maps a 6-bit subsequence S[idx...idx+5] of constant deBruijn64 to idx.
    /*! \sa deBruijn64
    */
    constexpr static uint32_t lt_deBruijn_to_idx[64] = {
        0,  1,  2,  7,  3,  13, 8,  19, 4,  25, 14, 28, 9,  34, 20, 40, 5,  17, 26, 38, 15, 46,
        29, 48, 10, 31, 35, 54, 21, 50, 41, 57, 63, 6,  12, 18, 24, 27, 33, 39, 16, 37, 45, 47,
        30, 53, 49, 56, 62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58};

    //! Array containing Fibonacci numbers less than \f$2^64\f$.
    constexpr static uint64_t lt_fib[92] = {1,
                                            2,
                                            3,
                                            5,
                                            8,
                                            13,
                                            21,
                                            34,
                                            55,
                                            89,
                                            144,
                                            233,
                                            377,
                                            610,
                                            987,
                                            1597,
                                            2584,
                                            4181,
                                            6765,
                                            10946,
                                            17711,
                                            28657,
                                            46368,
                                            75025,
                                            121393,
                                            196418,
                                            317811,
                                            514229,
                                            832040,
                                            1346269,
                                            2178309,
                                            3524578,
                                            5702887,
                                            9227465,
                                            14930352,
                                            24157817,
                                            39088169,
                                            63245986,
                                            102334155,
                                            165580141,
                                            267914296,
                                            433494437,
                                            701408733,
                                            1134903170,
                                            1836311903,
                                            2971215073ULL,
                                            0x11e8d0a40ULL,
                                            0x1cfa62f21ULL,
                                            0x2ee333961ULL,
                                            0x4bdd96882ULL,
                                            0x7ac0ca1e3ULL,
                                            0xc69e60a65ULL,
                                            0x1415f2ac48ULL,
                                            0x207fd8b6adULL,
                                            0x3495cb62f5ULL,
                                            0x5515a419a2ULL,
                                            0x89ab6f7c97ULL,
                                            0xdec1139639ULL,
                                            0x1686c8312d0ULL,
                                            0x2472d96a909ULL,
                                            0x3af9a19bbd9ULL,
                                            0x5f6c7b064e2ULL,
                                            0x9a661ca20bbULL,
                                            0xf9d297a859dULL,
                                            0x19438b44a658ULL,
                                            0x28e0b4bf2bf5ULL,
                                            0x42244003d24dULL,
                                            0x6b04f4c2fe42ULL,
                                            0xad2934c6d08fULL,
                                            0x1182e2989ced1ULL,
                                            0x1c5575e509f60ULL,
                                            0x2dd8587da6e31ULL,
                                            0x4a2dce62b0d91ULL,
                                            0x780626e057bc2ULL,
                                            0xc233f54308953ULL,
                                            0x13a3a1c2360515ULL,
                                            0x1fc6e116668e68ULL,
                                            0x336a82d89c937dULL,
                                            0x533163ef0321e5ULL,
                                            0x869be6c79fb562ULL,
                                            0xd9cd4ab6a2d747ULL,
                                            0x16069317e428ca9ULL,
                                            0x23a367c34e563f0ULL,
                                            0x39a9fadb327f099ULL,
                                            0x5d4d629e80d5489ULL,
                                            0x96f75d79b354522ULL,
                                            0xf444c01834299abULL,
                                            0x18b3c1d91e77decdULL,
                                            0x27f80ddaa1ba7878ULL,
                                            0x40abcfb3c0325745ULL,
                                            0x68a3dd8e61eccfbdULL,
                                            0xa94fad42221f2702ULL};

    //! Lookup table for byte popcounts.
    constexpr static uint8_t lt_cnt[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

    //! Lookup table for most significant set bit in a byte.
    constexpr static uint32_t lt_hi[256] = {
        0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

    //! lo_set[i] is a 64-bit word with the i least significant bits set and the high bits not set.
    /*! lo_set[0] = 0ULL, lo_set[1]=1ULL, lo_set[2]=3ULL...
     */
    constexpr static uint64_t lo_set[65] = {
        0x0000000000000000ULL, 0x0000000000000001ULL, 0x0000000000000003ULL, 0x0000000000000007ULL,
        0x000000000000000FULL, 0x000000000000001FULL, 0x000000000000003FULL, 0x000000000000007FULL,
        0x00000000000000FFULL, 0x00000000000001FFULL, 0x00000000000003FFULL, 0x00000000000007FFULL,
        0x0000000000000FFFULL, 0x0000000000001FFFULL, 0x0000000000003FFFULL, 0x0000000000007FFFULL,
        0x000000000000FFFFULL, 0x000000000001FFFFULL, 0x000000000003FFFFULL, 0x000000000007FFFFULL,
        0x00000000000FFFFFULL, 0x00000000001FFFFFULL, 0x00000000003FFFFFULL, 0x00000000007FFFFFULL,
        0x0000000000FFFFFFULL, 0x0000000001FFFFFFULL, 0x0000000003FFFFFFULL, 0x0000000007FFFFFFULL,
        0x000000000FFFFFFFULL, 0x000000001FFFFFFFULL, 0x000000003FFFFFFFULL, 0x000000007FFFFFFFULL,
        0x00000000FFFFFFFFULL, 0x00000001FFFFFFFFULL, 0x00000003FFFFFFFFULL, 0x00000007FFFFFFFFULL,
        0x0000000FFFFFFFFFULL, 0x0000001FFFFFFFFFULL, 0x0000003FFFFFFFFFULL, 0x0000007FFFFFFFFFULL,
        0x000000FFFFFFFFFFULL, 0x000001FFFFFFFFFFULL, 0x000003FFFFFFFFFFULL, 0x000007FFFFFFFFFFULL,
        0x00000FFFFFFFFFFFULL, 0x00001FFFFFFFFFFFULL, 0x00003FFFFFFFFFFFULL, 0x00007FFFFFFFFFFFULL,
        0x0000FFFFFFFFFFFFULL, 0x0001FFFFFFFFFFFFULL, 0x0003FFFFFFFFFFFFULL, 0x0007FFFFFFFFFFFFULL,
        0x000FFFFFFFFFFFFFULL, 0x001FFFFFFFFFFFFFULL, 0x003FFFFFFFFFFFFFULL, 0x007FFFFFFFFFFFFFULL,
        0x00FFFFFFFFFFFFFFULL, 0x01FFFFFFFFFFFFFFULL, 0x03FFFFFFFFFFFFFFULL, 0x07FFFFFFFFFFFFFFULL,
        0x0FFFFFFFFFFFFFFFULL, 0x1FFFFFFFFFFFFFFFULL, 0x3FFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL,
        0xFFFFFFFFFFFFFFFFULL};

    //! lo_unset[i] is a 64-bit word with the i least significant bits not set and the high bits set.
    /*! lo_unset[0] = FFFFFFFFFFFFFFFFULL, lo_unset_set[1]=FFFFFFFFFFFFFFFEULL, ...
     */
    constexpr static uint64_t lo_unset[65] = {
        0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFEULL, 0xFFFFFFFFFFFFFFFCULL, 0xFFFFFFFFFFFFFFF8ULL,
        0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFE0ULL, 0xFFFFFFFFFFFFFFC0ULL, 0xFFFFFFFFFFFFFF80ULL,
        0xFFFFFFFFFFFFFF00ULL, 0xFFFFFFFFFFFFFE00ULL, 0xFFFFFFFFFFFFFC00ULL, 0xFFFFFFFFFFFFF800ULL,
        0xFFFFFFFFFFFFF000ULL, 0xFFFFFFFFFFFFE000ULL, 0xFFFFFFFFFFFFC000ULL, 0xFFFFFFFFFFFF8000ULL,
        0xFFFFFFFFFFFF0000ULL, 0xFFFFFFFFFFFE0000ULL, 0xFFFFFFFFFFFC0000ULL, 0xFFFFFFFFFFF80000ULL,
        0xFFFFFFFFFFF00000ULL, 0xFFFFFFFFFFE00000ULL, 0xFFFFFFFFFFC00000ULL, 0xFFFFFFFFFF800000ULL,
        0xFFFFFFFFFF000000ULL, 0xFFFFFFFFFE000000ULL, 0xFFFFFFFFFC000000ULL, 0xFFFFFFFFF8000000ULL,
        0xFFFFFFFFF0000000ULL, 0xFFFFFFFFE0000000ULL, 0xFFFFFFFFC0000000ULL, 0xFFFFFFFF80000000ULL,
        0xFFFFFFFF00000000ULL, 0xFFFFFFFE00000000ULL, 0xFFFFFFFC00000000ULL, 0xFFFFFFF800000000ULL,
        0xFFFFFFF000000000ULL, 0xFFFFFFE000000000ULL, 0xFFFFFFC000000000ULL, 0xFFFFFF8000000000ULL,
        0xFFFFFF0000000000ULL, 0xFFFFFE0000000000ULL, 0xFFFFFC0000000000ULL, 0xFFFFF80000000000ULL,
        0xFFFFF00000000000ULL, 0xFFFFE00000000000ULL, 0xFFFFC00000000000ULL, 0xFFFF800000000000ULL,
        0xFFFF000000000000ULL, 0xFFFE000000000000ULL, 0xFFFC000000000000ULL, 0xFFF8000000000000ULL,
        0xFFF0000000000000ULL, 0xFFE0000000000000ULL, 0xFFC0000000000000ULL, 0xFF80000000000000ULL,
        0xFF00000000000000ULL, 0xFE00000000000000ULL, 0xFC00000000000000ULL, 0xF800000000000000ULL,
        0xF000000000000000ULL, 0xE000000000000000ULL, 0xC000000000000000ULL, 0x8000000000000000ULL,
        0x0000000000000000ULL};

    //! Lookup table for least significant set bit in a byte.
    constexpr static uint8_t lt_lo[256] = {
        0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x05, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x06, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x05, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x07, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x05, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x06, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x05, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00,
        0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00};

    //! Lookup table for select on bytes.
    /*! Entry at idx = 256*j + i equals the position of the
        (j+1)-th set bit in byte i. Positions lie in the range \f$[0..7]\f$.
     */
    constexpr static uint8_t lt_sel[256 * 8] = {
        0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,

        0, 0, 0, 1, 0, 2, 2, 1, 0, 3, 3, 1, 3, 2, 2, 1, 0, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        0, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        0, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1, 6, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        0, 7, 7, 1, 7, 2, 2, 1, 7, 3, 3, 1, 3, 2, 2, 1, 7, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        7, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        7, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1, 6, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,

        0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 3, 3, 2, 0, 0, 0, 4, 0, 4, 4, 2, 0, 4, 4, 3, 4, 3, 3, 2,
        0, 0, 0, 5, 0, 5, 5, 2, 0, 5, 5, 3, 5, 3, 3, 2, 0, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,
        0, 0, 0, 6, 0, 6, 6, 2, 0, 6, 6, 3, 6, 3, 3, 2, 0, 6, 6, 4, 6, 4, 4, 2, 6, 4, 4, 3, 4, 3, 3, 2,
        0, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2, 6, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,
        0, 0, 0, 7, 0, 7, 7, 2, 0, 7, 7, 3, 7, 3, 3, 2, 0, 7, 7, 4, 7, 4, 4, 2, 7, 4, 4, 3, 4, 3, 3, 2,
        0, 7, 7, 5, 7, 5, 5, 2, 7, 5, 5, 3, 5, 3, 3, 2, 7, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,
        0, 7, 7, 6, 7, 6, 6, 2, 7, 6, 6, 3, 6, 3, 3, 2, 7, 6, 6, 4, 6, 4, 4, 2, 6, 4, 4, 3, 4, 3, 3, 2,
        7, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2, 6, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 4, 0, 4, 4, 3,
        0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 5, 5, 3, 0, 0, 0, 5, 0, 5, 5, 4, 0, 5, 5, 4, 5, 4, 4, 3,
        0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 3, 0, 0, 0, 6, 0, 6, 6, 4, 0, 6, 6, 4, 6, 4, 4, 3,
        0, 0, 0, 6, 0, 6, 6, 5, 0, 6, 6, 5, 6, 5, 5, 3, 0, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
        0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 3, 0, 0, 0, 7, 0, 7, 7, 4, 0, 7, 7, 4, 7, 4, 4, 3,
        0, 0, 0, 7, 0, 7, 7, 5, 0, 7, 7, 5, 7, 5, 5, 3, 0, 7, 7, 5, 7, 5, 5, 4, 7, 5, 5, 4, 5, 4, 4, 3,
        0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 3, 0, 7, 7, 6, 7, 6, 6, 4, 7, 6, 6, 4, 6, 4, 4, 3,
        0, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 3, 7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 5, 5, 4,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 4,
        0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 5, 0, 0, 0, 6, 0, 6, 6, 5, 0, 6, 6, 5, 6, 5, 5, 4,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 4,
        0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 5, 0, 0, 0, 7, 0, 7, 7, 5, 0, 7, 7, 5, 7, 5, 5, 4,
        0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6, 0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 4,
        0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 5, 0, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 5,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 5,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6,
        0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6, 0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 5,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6,

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7};

    //! Use to help to decide if a prefix sum stored in a byte overflows.
    constexpr static uint64_t ps_overflow[65] = {
        0x8080808080808080ULL, 0x7f7f7f7f7f7f7f7fULL, 0x7e7e7e7e7e7e7e7eULL, 0x7d7d7d7d7d7d7d7dULL,
        0x7c7c7c7c7c7c7c7cULL, 0x7b7b7b7b7b7b7b7bULL, 0x7a7a7a7a7a7a7a7aULL, 0x7979797979797979ULL,
        0x7878787878787878ULL, 0x7777777777777777ULL, 0x7676767676767676ULL, 0x7575757575757575ULL,
        0x7474747474747474ULL, 0x7373737373737373ULL, 0x7272727272727272ULL, 0x7171717171717171ULL,
        0x7070707070707070ULL, 0x6f6f6f6f6f6f6f6fULL, 0x6e6e6e6e6e6e6e6eULL, 0x6d6d6d6d6d6d6d6dULL,
        0x6c6c6c6c6c6c6c6cULL, 0x6b6b6b6b6b6b6b6bULL, 0x6a6a6a6a6a6a6a6aULL, 0x6969696969696969ULL,
        0x6868686868686868ULL, 0x6767676767676767ULL, 0x6666666666666666ULL, 0x6565656565656565ULL,
        0x6464646464646464ULL, 0x6363636363636363ULL, 0x6262626262626262ULL, 0x6161616161616161ULL,
        0x6060606060606060ULL, 0x5f5f5f5f5f5f5f5fULL, 0x5e5e5e5e5e5e5e5eULL, 0x5d5d5d5d5d5d5d5dULL,
        0x5c5c5c5c5c5c5c5cULL, 0x5b5b5b5b5b5b5b5bULL, 0x5a5a5a5a5a5a5a5aULL, 0x5959595959595959ULL,
        0x5858585858585858ULL, 0x5757575757575757ULL, 0x5656565656565656ULL, 0x5555555555555555ULL,
        0x5454545454545454ULL, 0x5353535353535353ULL, 0x5252525252525252ULL, 0x5151515151515151ULL,
        0x5050505050505050ULL, 0x4f4f4f4f4f4f4f4fULL, 0x4e4e4e4e4e4e4e4eULL, 0x4d4d4d4d4d4d4d4dULL,
        0x4c4c4c4c4c4c4c4cULL, 0x4b4b4b4b4b4b4b4bULL, 0x4a4a4a4a4a4a4a4aULL, 0x4949494949494949ULL,
        0x4848484848484848ULL, 0x4747474747474747ULL, 0x4646464646464646ULL, 0x4545454545454545ULL,
        0x4444444444444444ULL, 0x4343434343434343ULL, 0x4242424242424242ULL, 0x4141414141414141ULL,
        0x4040404040404040ULL};

    //! Counts the number of set bits in x.
    /*! \param  x 64-bit word
        \return Number of set bits.
     */
    SDSL_CONSTEXPR static uint64_t cnt(uint64_t x);

    //! Position of the most significant set bit the 64-bit word x
    /*! \param x 64-bit word
        \return The position (in 0..63) of the most significant set bit
                in `x` or 0 if x equals 0.
    	\sa sel, lo
    */
    SDSL_CONSTEXPR static uint32_t hi(uint64_t x);

    //! Calculates the position of the rightmost 1-bit in the 64bit integer x if it exists
    /*! \param x 64 bit integer.
    	\return The position (in 0..63) of the rightmost 1-bit in the 64bit integer x if
    	        x>0 and 0 if x equals 0.
    	\sa sel, hi
    */
    SDSL_CONSTEXPR static uint32_t lo(uint64_t x);

    //! Counts the number of 1-bits in the 32bit integer x.
    /*! This function is a variant of the method cnt. If
    	32bit multiplication is fast, this method beats the cnt.
    	for 32bit integers.
    	\param x 64bit integer to count the bits.
    	\return The number of 1-bits in x.
     */
    SDSL_CONSTEXPR static uint32_t cnt32(uint32_t x);

    //! Count the number of consecutive and distinct 11 in the 64bit integer x.
    /*!
      	\param x 64bit integer to count the terminating sequence 11 of a Fibonacci code.
    	\param c Carry equals msb of the previous 64bit integer.
     */
    SDSL_CONSTEXPR static uint32_t cnt11(uint64_t x, uint64_t& c);

    //! Count the number of consecutive and distinct 11 in the 64bit integer x.
    /*!
      	\param x 64bit integer to count the terminating sequence 11 of a Fibonacci code.
     */
    SDSL_CONSTEXPR static uint32_t cnt11(uint64_t x);

    //! Count 10 bit pairs in the word x.
    /*!
     * \param x 64bit integer to count the 10 bit pairs.
     * \param c Carry equals msb of the previous 64bit integer.
     */
    SDSL_CONSTEXPR static uint32_t cnt10(uint64_t x, uint64_t& c);

    //! Count 01 bit pairs in the word x.
    /*!
     * \param x 64bit integer to count the 01 bit pairs.
     * \param c Carry equals msb of the previous 64bit integer.
     */
    SDSL_CONSTEXPR static uint32_t cnt01(uint64_t x, uint64_t& c);

    //! Map all 10 bit pairs to 01 or 1 if c=1 and the lsb=0. All other pairs are mapped to 00.
    SDSL_CONSTEXPR static uint64_t map10(uint64_t x, uint64_t c = 0);

    //! Map all 01 bit pairs to 01 or 1 if c=1 and the lsb=0. All other pairs are mapped to 00.
    SDSL_CONSTEXPR static uint64_t map01(uint64_t x, uint64_t c = 1);

    //! Calculate the position of the i-th rightmost 1 bit in the 64bit integer x
    /*!
      	\param x 64bit integer.
    	\param i Argument i must be in the range \f$[1..cnt(x)]\f$.
    	\pre Argument i must be in the range \f$[1..cnt(x)]\f$.
      	\sa hi, lo
     */
    SDSL_CONSTEXPR static uint32_t sel(uint64_t x, uint32_t i);
    SDSL_CONSTEXPR static uint32_t _sel(uint64_t x, uint32_t i);

    //! Calculates the position of the i-th rightmost 11-bit-pattern which terminates a Fibonacci coded integer in x.
    /*!	\param x 64 bit integer.
        \param i Index of 11-bit-pattern. \f$i \in [1..cnt11(x)]\f$
    	\param c Carry bit from word before
     	\return The position (in 1..63) of the i-th 11-bit-pattern which terminates a Fibonacci coded integer in x if
    	        x contains at least i 11-bit-patterns and a undefined value otherwise.
        \sa cnt11, hi11, sel

     */
    SDSL_CONSTEXPR static uint32_t sel11(uint64_t x, uint32_t i, uint32_t c = 0);

    //! Calculates the position of the leftmost 11-bit-pattern which terminates a Fibonacci coded integer in x.
    /*! \param x 64 bit integer.
        \return The position (in 1..63) of the leftmost 1 of the leftmost 11-bit-pattern which
    	        terminates a Fibonacci coded integer in x if x contains a 11-bit-pattern
    			and 0 otherwise.
    	\sa cnt11, sel11
    */
    SDSL_CONSTEXPR static uint32_t hi11(uint64_t x);

    //! Writes value x to an bit position in an array.
    SDSL_CONSTEXPR static void
    write_int(uint64_t* word, uint64_t x, const uint8_t offset = 0, const uint8_t len = 64);

    //! Writes value x to an bit position in an array and moves the bit-pointer.
    SDSL_CONSTEXPR static void write_int_and_move(uint64_t*& word, uint64_t x, uint8_t& offset, const uint8_t len);

    //! Reads a value from a bit position in an array.
    SDSL_CONSTEXPR static uint64_t read_int(const uint64_t* word, uint8_t offset = 0, const uint8_t len = 64);

    //! Reads a value from a bit position in an array and moved the bit-pointer.
    SDSL_CONSTEXPR static uint64_t
    read_int_and_move(const uint64_t*& word, uint8_t& offset, const uint8_t len = 64);

    //! Reads an unary decoded value from a bit position in an array.
    SDSL_CONSTEXPR static uint64_t read_unary(const uint64_t* word, uint8_t offset = 0);

    //! Reads an unary decoded value from a bit position in an array and moves the bit-pointer.
    SDSL_CONSTEXPR static uint64_t read_unary_and_move(const uint64_t*& word, uint8_t& offset);

    //! Move the bit-pointer (=uint64_t word and offset) `len` to the right.
    /*!\param word   64-bit word part of the bit pointer
     * \param offset Offset part of the bit pointer
     * \param len    Move distance. \f$ len \in [0..64] \f$
     * \sa move_left
     */
    SDSL_CONSTEXPR static void move_right(const uint64_t*& word, uint8_t& offset, const uint8_t len);

    //! Move the bit-pointer (=uint64_t word and offset) `len` to the left.
    /*!\param word   64-bit word part of the bit pointer
     * \param offset Offset part of the bit pointer
     * \param len    Move distance. \f$ len \in [0..64] \f$
     * \sa move_right
     */
    SDSL_CONSTEXPR static void move_left(const uint64_t*& word, uint8_t& offset, const uint8_t len);

    //! Get the first one bit in the interval \f$[idx..\infty )\f$
    SDSL_CONSTEXPR static uint64_t next(const uint64_t* word, uint64_t idx);

    //! Get the one bit with the greatest position in the interval \f$[0..idx]\f$
    SDSL_CONSTEXPR static uint64_t prev(const uint64_t* word, uint64_t idx);

    //! reverses a given 64 bit word
    SDSL_CONSTEXPR static uint64_t rev(uint64_t x);
};

// ============= inline - implementations ================

// see page 11, Knuth TAOCP Vol 4 F1A
template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::cnt(uint64_t x)
{
#ifdef __SSE4_2__
    return __builtin_popcountll(x);
#else
    #ifdef POPCOUNT_TL
	return lt_cnt[x & 0xFFULL] + lt_cnt[(x >> 8) & 0xFFULL] + lt_cnt[(x >> 16) & 0xFFULL] +
		   lt_cnt[(x >> 24) & 0xFFULL] + lt_cnt[(x >> 32) & 0xFFULL] + lt_cnt[(x >> 40) & 0xFFULL] +
		   lt_cnt[(x >> 48) & 0xFFULL] + lt_cnt[(x >> 56) & 0xFFULL];
#else
	x = x - ((x >> 1) & 0x5555555555555555ull);
	x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
	return (0x0101010101010101ull * x >> 56);
#endif
#endif
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::cnt32(uint32_t x)
{
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    return (0x10101010 * x >> 28) + (0x01010101 * x >> 28);
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::cnt11(uint64_t x, uint64_t& c)
{
    // extract "11" 2bit blocks
    uint64_t ex11 = (x & (x >> 1)) & 0x5555555555555555ULL, t{};
    // extract "10" 2bit blocks
    uint64_t ex10or01 = (ex11 | (ex11 << 1)) ^ x;

    x = ex11 | ((t = (ex11 | (ex11 << 1)) + (((ex10or01 << 1) & 0x5555555555555555ULL) | c)) &
        (ex10or01 & 0x5555555555555555ULL));
    c = (ex10or01 >> 63) or (t < (ex11 | (ex11 << 1)));

    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (0x0101010101010101ULL * x >> 56);
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::cnt11(uint64_t x)
{
    // extract "11" 2bit blocks
    uint64_t ex11 = (x & (x >> 1)) & 0x5555555555555555ULL;
    // extract "10" 2bit blocks
    uint64_t ex10or01 = (ex11 | (ex11 << 1)) ^ x;

    x = ex11 | (((ex11 | (ex11 << 1)) + ((ex10or01 << 1) & 0x5555555555555555ULL)) &
        (ex10or01 & 0x5555555555555555ULL));

    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (0x0101010101010101ULL * x >> 56);
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::cnt10(uint64_t x, uint64_t& c)
{
    uint32_t res = cnt((x ^ ((x << 1) | c)) & (~x));
    c			 = (x >> 63);
    return res;
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::map10(uint64_t x, uint64_t c)
{
    return ((x ^ ((x << 1) | c)) & (~x));
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::cnt01(uint64_t x, uint64_t& c)
{
    uint32_t res = cnt((x ^ ((x << 1) | c)) & x);
    c			 = (x >> 63);
    return res;
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::map01(uint64_t x, uint64_t c)
{
    return ((x ^ ((x << 1) | c)) & x);
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::sel(uint64_t x, uint32_t i)
{
#ifdef __BMI2__
    // taken from folly
    return _tzcnt_u64(_pdep_u64(1ULL << (i - 1), x));
#endif
#ifdef __SSE4_2__
    uint64_t s = x, b{};
    s		   = s - ((s >> 1) & 0x5555555555555555ULL);
    s		   = (s & 0x3333333333333333ULL) + ((s >> 2) & 0x3333333333333333ULL);
    s		   = (s + (s >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    s		   = 0x0101010101010101ULL * s;
    // now s contains 8 bytes s[7],...,s[0]; s[j] contains the cumulative sum
    // of (j+1)*8 least significant bits of s
    b = (s + ps_overflow[i]) & 0x8080808080808080ULL;
    // ps_overflow contains a bit mask x consisting of 8 bytes
    // x[7],...,x[0] and x[j] is set to 128-j
    // => a byte b[j] in b is >= 128 if cum sum >= j

    // __builtin_ctzll returns the number of trailing zeros, if b!=0
    int byte_nr = __builtin_ctzll(b) >> 3; // byte nr in [0..7]
    s <<= 8;
    i -= (s >> (byte_nr << 3)) & 0xFFULL;
    return (byte_nr << 3) + lt_sel[((i - 1) << 8) + ((x >> (byte_nr << 3)) & 0xFFULL)];
#endif
    return _sel(x, i);
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::_sel(uint64_t x, uint32_t i)
{
    uint64_t s = x, b{}; // s = sum
    s		   = s - ((s >> 1) & 0x5555555555555555ULL);
    s		   = (s & 0x3333333333333333ULL) + ((s >> 2) & 0x3333333333333333ULL);
    s		   = (s + (s >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    s		   = 0x0101010101010101ULL * s;
    b		   = (s + ps_overflow
    [i]); //&0x8080808080808080ULL;// add something to the partial sums to cause overflow
    i = (i - 1) << 8;
    if (b & 0x0000000080000000ULL)	 // byte <=3
        if (b & 0x0000000000008000ULL) //byte <= 1
            if (b & 0x0000000000000080ULL)
                return lt_sel[(x & 0xFFULL) + i];
            else
                return 8 + lt_sel[(((x >> 8) & 0xFFULL) + i - ((s & 0xFFULL) << 8)) &
                    0x7FFULL];											  //byte 1;
        else																			  //byte >1
        if (b & 0x0000000000800000ULL)													  //byte <=2
            return 16 + lt_sel[(((x >> 16) & 0xFFULL) + i - (s & 0xFF00ULL)) & 0x7FFULL]; //byte 2;
        else
            return 24 + lt_sel[(((x >> 24) & 0xFFULL) + i - ((s >> 8) & 0xFF00ULL)) &
                0x7FFULL]; //byte 3;
    else								  //  byte > 3
    if (b & 0x0000800000000000ULL)		  // byte <=5
        if (b & 0x0000008000000000ULL)	//byte <=4
            return 32 + lt_sel[(((x >> 32) & 0xFFULL) + i - ((s >> 16) & 0xFF00ULL)) &
                0x7FFULL]; //byte 4;
        else
            return 40 + lt_sel[(((x >> 40) & 0xFFULL) + i - ((s >> 24) & 0xFF00ULL)) &
                0x7FFULL]; //byte 5;
    else								  // byte >5
    if (b & 0x0080000000000000ULL)		  //byte<=6
        return 48 +
            lt_sel[(((x >> 48) & 0xFFULL) + i - ((s >> 32) & 0xFF00ULL)) & 0x7FFULL]; //byte 6;
    else
        return 56 +
            lt_sel[(((x >> 56) & 0xFFULL) + i - ((s >> 40) & 0xFF00ULL)) & 0x7FFULL]; //byte 7;
    return 0;
}

// using built-in method or
// 64-bit version of 32-bit proposal of
// http://www-graphics.stanford.edu/~seander/bithacks.html
template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::hi(uint64_t x)
{
#ifdef __SSE4_2__
    if (x == 0) return 0;
    return 63 - __builtin_clzll(x);
#else
    uint64_t t{}, tt{};			  // temporaries
	if ((tt = x >> 32)) {	 // hi >= 32
		if ((t = tt >> 16)) { // hi >= 48
			return (tt = t >> 8) ? 56 + lt_hi[tt] : 48 + lt_hi[t];
		} else { // hi < 48
			return (t = tt >> 8) ? 40 + lt_hi[t] : 32 + lt_hi[tt];
		}
	} else {				 // hi < 32
		if ((t = x >> 16)) { // hi >= 16
			return (tt = t >> 8) ? 24 + lt_hi[tt] : 16 + lt_hi[t];
		} else { // hi < 16
			return (tt = x >> 8) ? 8 + lt_hi[tt] : lt_hi[x];
		}
	}
#endif
}

// details see: http://citeseer.ist.psu.edu/leiserson98using.html
// or page 10, Knuth TAOCP Vol 4 F1A
template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::lo(uint64_t x)
{
#ifdef __SSE4_2__
    if (x == 0) return 0;
    return __builtin_ctzll(x);
#else
    if (x & 1) return 0;
	if (x & 3) return 1;
	if (x & 7) return 2;
	if (x & 0x7FF) { // in average every second random number x can be answered this way
		return lt_lo[(x & 0x7FF) >> 3] + 3;
	}
	// x&-x equals x with only the lsb set
	return lt_deBruijn_to_idx[((x & -x) * deBruijn64) >> 58];
#endif
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::hi11(uint64_t x)
{
    // extract "11" 2bit blocks
    uint64_t ex11 = (x & (x >> 1)) & 0x5555555555555555ULL;
    // extract "10" 2bit blocks
    uint64_t ex10or01 = (ex11 | (ex11 << 1)) ^ x;
    // extract "10" 2bit blocks
    ex11 += (((ex11 | (ex11 << 1)) + ((ex10or01 << 1) & 0x5555555555555555ULL)) &
        ((ex10or01 & 0x5555555555555555ULL) | ex11));
    return hi(ex11);
}

template <typename T>
SDSL_CONSTEXPR inline uint32_t bits_impl<T>::sel11(uint64_t x, uint32_t i, uint32_t c)
{
    uint64_t ex11	 = (x & (x >> 1)) & 0x5555555555555555ULL;
    uint64_t ex10or01 = (ex11 | (ex11 << 1)) ^ x;
    ex11 += (((ex11 | (ex11 << 1)) + (((ex10or01 << 1) & 0x5555555555555555ULL) | c)) &
        ((ex10or01 & 0x5555555555555555ULL) | ex11));
    return sel(ex11, i);
}

template <typename T>
SDSL_CONSTEXPR inline void bits_impl<T>::write_int(uint64_t* word, uint64_t x, uint8_t offset, const uint8_t len)
{
    x &= bits_impl<T>::lo_set[len];
    if (offset + len < 64) {
        *word &= ((bits_impl<T>::all_set << (offset + len)) |
            bits_impl<T>::lo_set[offset]); // mask 1..10..01..1
        *word |= (x << offset);
        //		*word ^= ((*word ^ x) & (bits_impl<T>::lo_set[len] << offset) );
        //      surprisingly the above line is slower than the lines above
    } else {
        *word &= ((bits_impl<T>::lo_set[offset])); // mask 0....01..1
        *word |= (x << offset);
        if ((offset = (offset + len) & 0x3F)) {				// offset+len > 64
            *(word + 1) &= (~bits_impl<T>::lo_set[offset]); // mask 1...10..0
            //			*(word+1) &= bits_impl<T>::lo_unset[offset]; // mask 1...10..0
            //          surprisingly the above line is slower than the line above
            *(word + 1) |= (x >> (len - offset));
        }
    }
}

template <typename T>
SDSL_CONSTEXPR inline void
bits_impl<T>::write_int_and_move(uint64_t*& word, uint64_t x, uint8_t& offset, const uint8_t len)
{
    x &= bits_impl<T>::lo_set[len];
    if (offset + len < 64) {
        *word &= ((bits_impl<T>::all_set << (offset + len)) |
            bits_impl<T>::lo_set[offset]); // mask 1..10..01..1
        *word |= (x << offset);
        offset += len;
    } else {
        *word &= ((bits_impl<T>::lo_set[offset])); // mask 0....01..1
        *word |= (x << offset);
        if ((offset = (offset + len)) > 64) { // offset+len >= 64
            offset &= 0x3F;
            *(++word) &= (~bits_impl<T>::lo_set[offset]); // mask 1...10..0
            *word |= (x >> (len - offset));
        } else {
            offset = 0;
            ++word;
        }
    }
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::read_int(const uint64_t* word, uint8_t offset, const uint8_t len)
{
    uint64_t w1 = (*word) >> offset;
    if ((offset + len) > 64) {												// if offset+len > 64
        return w1 |															// w1 or w2 adepted:
            ((*(word + 1) & bits_impl<T>::lo_set[(offset + len) & 0x3F]) // set higher bits zero
                << (64 - offset));											// move bits to the left
    } else {
        return w1 & bits_impl<T>::lo_set[len];
    }
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t
bits_impl<T>::read_int_and_move(const uint64_t*& word, uint8_t& offset, const uint8_t len)
{
    uint64_t w1 = (*word) >> offset;
    if ((offset = (offset + len)) >= 64) { // if offset+len > 64
        if (offset == 64) {
            offset &= 0x3F;
            ++word;
            return w1;
        } else {
            offset &= 0x3F;
            return w1 | (((*(++word)) & bits_impl<T>::lo_set[offset]) << (len - offset));
        }
    } else {
        return w1 & bits_impl<T>::lo_set[len];
    }
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::read_unary(const uint64_t* word, uint8_t offset)
{
    uint64_t w = *word >> offset;
    if (w) {
        return bits_impl<T>::lo(w);
    } else {
        if (0 != (w  = *(++word))) return bits_impl<T>::lo(w) + 64 - offset;
        uint64_t cnt = 2;
        while (0 == (w = *(++word)))
            ++cnt;
        return bits_impl<T>::lo(w) + (cnt << 6) - offset;
    }
    return 0;
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::read_unary_and_move(const uint64_t*& word, uint8_t& offset)
{
    uint64_t w = (*word) >> offset; // temporary variable is good for the performance
    if (w) {
        uint8_t r = bits_impl<T>::lo(w);
        offset	= (offset + r + 1) & 0x3F;
        // we know that offset + r +1 <= 64, so if the new offset equals 0 increase word
        word += (offset == 0);
        return r;
    } else {
        uint8_t rr = 0;
        if (0 != (w = *(++word))) {
            rr	 = bits_impl<T>::lo(w) + 64 - offset;
            offset = (offset + rr + 1) & 0x3F;
            word += (offset == 0);
            return rr;
        } else {
            uint64_t cnt_1 = 1;
            while (0 == (w = *(++word)))
                ++cnt_1;
            rr	 = bits_impl<T>::lo(w) + 64 - offset;
            offset = (offset + rr + 1) & 0x3F;
            word += (offset == 0);
            return ((cnt_1) << 6) + rr;
        }
    }
    return 0;
}

template <typename T>
SDSL_CONSTEXPR inline void bits_impl<T>::move_right(const uint64_t*& word, uint8_t& offset, const uint8_t len)
{
    if ((offset += len) & 0xC0) { // if offset >= 65
        offset &= 0x3F;
        ++word;
    }
}

template <typename T>
SDSL_CONSTEXPR inline void bits_impl<T>::move_left(const uint64_t*& word, uint8_t& offset, const uint8_t len)
{
    if ((offset -= len) & 0xC0) { // if offset-len<0
        offset &= 0x3F;
        --word;
    }
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::next(const uint64_t* word, uint64_t idx)
{
    word += (idx >> 6);
    if (*word & ~lo_set[idx & 0x3F]) {
        return (idx & ~((size_t)0x3F)) + lo(*word & ~lo_set[idx & 0x3F]);
    }
    idx = (idx & ~((size_t)0x3F)) + 64;
    ++word;
    while (*word == 0) {
        idx += 64;
        ++word;
    }
    return idx + lo(*word);
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::prev(const uint64_t* word, uint64_t idx)
{
    word += (idx >> 6);
    if (*word & lo_set[(idx & 0x3F) + 1]) {
        return (idx & ~((size_t)0x3F)) + hi(*word & lo_set[(idx & 0x3F) + 1]);
    }
    idx = (idx & ~((size_t)0x3F)) - 64;
    --word;
    while (*word == 0) {
        idx -= 64;
        --word;
    }
    return idx + hi(*word);
}

template <typename T>
SDSL_CONSTEXPR inline uint64_t bits_impl<T>::rev(uint64_t x)
{
    x = ((x & 0x5555555555555555ULL) << 1) | ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
    x = ((x & 0x3333333333333333ULL) << 2) | ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
    x = ((x & 0x0F0F0F0F0F0F0F0FULL) << 4) | ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
    x = ((x & 0x00FF00FF00FF00FFULL) << 8) | ((x & 0xFF00FF00FF00FF00ULL) >> 8);
    x = ((x & 0x0000FFFF0000FFFFULL) << 16) | ((x & 0xFFFF0000FFFF0000ULL) >> 16);
    x = ((x & 0x00000000FFFFFFFFULL) << 32) | ((x & 0xFFFFFFFF00000000ULL) >> 32);
    return x;
}

template <typename T>
constexpr uint8_t bits_impl<T>::lt_cnt[256];
template <typename T>
constexpr uint32_t bits_impl<T>::lt_deBruijn_to_idx[64];
template <typename T>
constexpr uint32_t bits_impl<T>::lt_hi[256];
template <typename T>
constexpr uint64_t bits_impl<T>::lo_set[65];
template <typename T>
constexpr uint64_t bits_impl<T>::lo_unset[65];
template <typename T>
constexpr uint64_t bits_impl<T>::ps_overflow[65];
template <typename T>
constexpr uint8_t bits_impl<T>::lt_sel[256 * 8];
template <typename T>
constexpr uint64_t bits_impl<T>::lt_fib[92];
template <typename T>
constexpr uint8_t bits_impl<T>::lt_lo[256];

using bits = bits_impl<>;

} // end namespace sdsl

#endif

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*!\file structure_tree.hpp
   \brief structure_tree.hpp contains a helper class which can represent the memory structure of a class.
   \author Simon Gog
*/
#ifndef INCLUDED_SDSL_STRUCTURE_TREE
#define INCLUDED_SDSL_STRUCTURE_TREE

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
#ifndef INCLUDED_SDSL_UINTX_T
#define INCLUDED_SDSL_UINTX_T

#include <cstdint>

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

#endif

#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
#ifndef SDSL_CONFIG
#define SDSL_CONFIG

#include <map>
#include <string>

#ifndef MSVC_COMPILER
#define SDSL_UNUSED __attribute__((unused))
#else
#define SDSL_UNUSED
#endif

namespace sdsl {

// forward declarations
namespace util {
template <typename T>
std::string to_string(const T& t, int w = 1);
uint64_t pid();
uint64_t id();
}

namespace conf // namespace for library constant
{
// size of the buffer for reading and writing data in elements (not in bytes)
const uint64_t SDSL_BLOCK_SIZE = (uint64_t)1 << 22;

constexpr char KEY_BWT[]		 = "bwt";
constexpr char KEY_BWT_INT[]	 = "bwt_int";
constexpr char KEY_SA[]			 = "sa";
constexpr char KEY_CSA[]		 = "csa";
constexpr char KEY_CST[]		 = "cst";
constexpr char KEY_ISA[]		 = "isa";
constexpr char KEY_TEXT[]		 = "text";
constexpr char KEY_TEXT_INT[]	= "text_int";
constexpr char KEY_PSI[]		 = "psi";
constexpr char KEY_LCP[]		 = "lcp";
constexpr char KEY_SAMPLE_CHAR[] = "sample_char";
}

typedef uint64_t int_vector_size_type;

typedef std::map<std::string, std::string> tMSS;

enum format_type { JSON_FORMAT, R_FORMAT, HTML_FORMAT };

enum byte_sa_algo_type { LIBDIVSUFSORT, SE_SAIS };

//! Helper class for construction process
struct cache_config {
    bool delete_files; // Flag which indicates if all files which were created
    bool delete_data;  // Flag which indicates if the original data can be deleted
    // during construction should be deleted.
    std::string dir; // Directory for temporary files.
    std::string id;  // Identifier is part of temporary file names. If
    // id is the empty string, then it will be replace
    // a concatenation of PID and a unique ID inside the
    // current process.
    tMSS file_map; // Files stored during the construction process.
    cache_config(bool		 f_delete_files = true,
                 std::string f_dir			= "./",
                 std::string f_id			= "",
                 tMSS		 f_file_map		= tMSS())
        : delete_files(f_delete_files), delete_data(false), dir(f_dir), id(f_id), file_map(f_file_map)
    {
        if ("" == id) {
            id = sdsl::util::to_string(sdsl::util::pid()) + "_" + sdsl::util::to_string(sdsl::util::id());
        }
    }

};

//! Helper classes to transform width=0 and width=8 to corresponding text key
template <uint8_t width, typename T=void>
struct key_text_trait_impl {
    static const char* KEY_TEXT;
};

template <typename T>
struct key_text_trait_impl<0,T> {
    static const char* KEY_TEXT;
};

template <typename T>
struct key_text_trait_impl<8,T> {
    static const char* KEY_TEXT;
};

//! Helper classes to transform width=0 and width=8 to corresponding bwt key
template <uint8_t width,typename T=void>
struct key_bwt_trait_impl {
    static const char* KEY_BWT;
};

template <typename T>
struct key_bwt_trait_impl<0,T> {
    static const char* KEY_BWT;
};

template <typename T>
struct key_bwt_trait_impl<8,T> {
    static const char* KEY_BWT;
};

template<typename T>
const char* key_text_trait_impl<0,T>::KEY_TEXT = conf::KEY_TEXT_INT;

template<typename T>
const char* key_text_trait_impl<8,T>::KEY_TEXT = conf::KEY_TEXT;

template<typename T>
const char* key_bwt_trait_impl<0,T>::KEY_BWT = conf::KEY_BWT_INT;

template<typename T>
const char* key_bwt_trait_impl<8,T>::KEY_BWT = conf::KEY_BWT;

template<uint8_t width>
using key_text_trait = key_text_trait_impl<width,void>;

template<uint8_t width>
using key_bwt_trait = key_bwt_trait_impl<width,void>;

}

#endif

//! Namespace for the succinct data structure library
namespace sdsl {

inline void output_tab(std::ostream& out, size_t level)
{
    for (size_t i = 0; i < level; i++)
        out << "\t";
}

class structure_tree_node {
private:
    using map_type = std::unordered_map<std::string, std::unique_ptr<structure_tree_node>>;
    map_type m_children;

public:
    const map_type& children = m_children;
    size_t			size	 = 0;
    std::string		name;
    std::string		type;

public:
    structure_tree_node(const std::string& n, const std::string& t) : name(n), type(t) {}
    structure_tree_node* add_child(const std::string& n, const std::string& t)
    {
        auto hash	  = n + t;
        auto child_itr = m_children.find(hash);
        if (child_itr == m_children.end()) {
            // add new child as we don't have one of this type yet
            structure_tree_node* new_node = new structure_tree_node(n, t);
            m_children[hash]			  = std::unique_ptr<structure_tree_node>(new_node);
            return new_node;
        } else {
            // child of same type and name exists
            return (*child_itr).second.get();
        }
    }
    void add_size(size_t s) { size += s; }
};

class structure_tree {
public:
    static structure_tree_node*
    add_child(structure_tree_node* v, const std::string& name, const std::string& type)
    {
        if (v) return v->add_child(name, type);
        return nullptr;
    };
    static void add_size(structure_tree_node* v, uint64_t value)
    {
        if (v) v->add_size(value);
    };
};

template <format_type F>
void write_structure_tree(const structure_tree_node* v, std::ostream& out, size_t level = 0);

template <>
inline void write_structure_tree<JSON_FORMAT>(const structure_tree_node* v,
                                              std::ostream&			  out,
                                              size_t					  level)
{
    if (v) {
        output_tab(out, level);
        out << "{" << std::endl;
        output_tab(out, level + 1);
        out << "\"class_name\":"
            << "\"" << v->type << "\"," << std::endl;
        output_tab(out, level + 1);
        out << "\"name\":"
            << "\"" << v->name << "\"," << std::endl;
        output_tab(out, level + 1);
        out << "\"size\":"
            << "\"" << v->size << "\"";

        if (v->children.size()) {
            out << "," << std::endl; // terminate the size tag from before if there are children
            output_tab(out, level + 1);
            out << "\"children\":[" << std::endl;
            size_t written_child_elements = 0;
            for (const auto& child : v->children) {
                if (written_child_elements++ > 0) {
                    out << "," << std::endl;
                }
                write_structure_tree<JSON_FORMAT>(child.second.get(), out, level + 2);
            }
            out << std::endl;
            output_tab(out, level + 1);
            out << "]" << std::endl;
        } else {
            out << std::endl;
        }
        output_tab(out, level);
        out << "}";
    }
}

inline std::string create_html_header(const char* file_name)
{
    std::stringstream jsonheader;
    jsonheader << "<html>\n"
               << "   <head>\n"
               << "    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\n"
               << "    <title>" << file_name << "</title>\n"
               << "    <script src=\"http://d3js.org/d3.v2.js\"></script>\n"
               << "    <style type=\"text/css\">\n"
               << "      path { stroke: #000; stroke-width: 0.8; cursor: pointer; }\n"
               << "      text { font: 11px sans-serif; cursor: pointer; }\n"
               << "      body { width: 900; margin: 0 auto; }\n"
               << "      h1 { text-align: center; margin: .5em 0; }\n"
               << "      #breadcrumbs { display: none; }\n"
               << "      svg { font: 10px sans-serif; }\n"
               << "     </style>\n"
               << "  </head>\n"
               << "<body marginwidth=\"0\" marginheight=\"0\">\n"
               << "<button><a id=\"download\">Save as SVG</a></button>\n"
               << "  <div id=\"chart\"></div>" << std::endl;
    return jsonheader.str();
}

inline std::string create_js_body(const std::string& jsonsize)
{
    std::stringstream jsonbody;
    jsonbody
        << "<script type=\"text/javascript\">" << std::endl
        << ""
           "var w = 800,\n"
           "  h = w,\n"
           "  r = w / 2,\n"
           "  x = d3.scale.linear().range([0, 2 * Math.PI]),\n"
           "  y = d3.scale.pow().exponent(1.3).domain([0, 1]).range([0, r]),\n"
           "  p = 5,\n"
           "  color = d3.scale.category20c(),\n"
           "  duration = 1000;\n"
           "\n"
           "var vis = d3.select(\"#chart\").append(\"svg:svg\")\n"
           "  .attr(\"width\", w + p * 2)\n"
           "  .attr(\"height\", h + p * 2)\n"
           "  .append(\"g\")\n"
           "  .attr(\"transform\", \"translate(\" + (r + p) + \",\" + (r + p) + \")\");\n"
           "\n"
           "vis.append(\"p\")\n"
           "  .attr(\"id\", \"intro\")\n"
           "  .text(\"Click to zoom!\");\n"
           "\n"
           "var partition = d3.layout.partition()\n"
           "  .sort(null)\n"
           "  .size([2 * Math.PI, r * r])\n"
           "  .value(function(d) { return d.size; });\n"
           "\n"
           "var arc = d3.svg.arc()\n"
           "  .startAngle(function(d) { return Math.max(0, Math.min(2 * Math.PI, x(d.x))); })\n"
           "  .endAngle(function(d) { return Math.max(0, Math.min(2 * Math.PI, x(d.x + d.dx))); })\n"
           "  .innerRadius(function(d) { return Math.max(0, d.y ? y(d.y) : d.y); })\n"
           "  .outerRadius(function(d) { return Math.max(0, y(d.y + d.dy)); });\n"
           "\n"
           "      "
        << std::endl
        << "var spaceJSON = " << jsonsize << ";" << std::endl
        << std::endl
        << "\n"
           "\n"
           "  var nodes = partition.nodes(spaceJSON);\n"
           "\n"
           "  var path = vis.selectAll(\"path\").data(nodes);\n"
           "  path.enter().append(\"path\")\n"
           "    .attr(\"id\", function(d, i) { return \"path-\" + i; })\n"
           "    .attr(\"d\", arc)\n"
           "    .attr(\"fill-rule\", \"evenodd\")\n"
           "    .style(\"fill\", colour)\n"
           "    .on(\"click\", click);\n"
           "\n"
           "  path.append(\"title\").text(function(d) { return 'class name: ' + d.class_name + "
           "'\\nmember_name: ' + d.name + '\\n size: ' + sizeMB(d) });\n"
           "\n"
           "  var text = vis.selectAll(\"text\").data(nodes);\n"
           "  var textEnter = text.enter().append(\"text\")\n"
           "    .style(\"opacity\", 1)\n"
           "    .style(\"fill\", function(d) {\n"
           "    return brightness(d3.rgb(colour(d))) < 125 ? \"#eee\" : \"#000\";\n"
           "    })\n"
           "    .attr(\"text-anchor\", function(d) {\n"
           "    return x(d.x + d.dx / 2) > Math.PI ? \"end\" : \"start\";\n"
           "    })\n"
           "    .attr(\"dy\", \".2em\")\n"
           "    .attr(\"transform\", function(d) {\n"
           "    var multiline = (d.name || \"\").split(\" \").length > 1,\n"
           "      angle = x(d.x + d.dx / 2) * 180 / Math.PI - 90,\n"
           "      rotate = angle + (multiline ? -.5 : 0);\n"
           "    return \"rotate(\" + rotate + \")translate(\" + (y(d.y) + p) + \")rotate(\" + (angle > "
           "90 ? -180 : 0) + \")\";\n"
           "    })\n"
           "    .on(\"click\", click);\n"
           "\n"
           "  textEnter.append(\"title\").text(function(d) { return 'class name: ' + d.class_name + "
           "'\\nmember_name: ' + d.name + '\\n size: ' + sizeMB(d) });\n"
           "\n"
           "  textEnter.append(\"tspan\")\n"
           "    .attr(\"x\", 0)\n"
           "    .text(function(d) { return d.dx < 0.05 ? \"\" : d.depth ? d.name.split(\" \")[0] : "
           "\"\"; });\n"
           "  textEnter.append(\"tspan\")\n"
           "    .attr(\"x\", 0)\n"
           "    .attr(\"dy\", \"1em\")\n"
           "    .text(function(d) { return d.dx < 0.05 ? \"\" : d.depth ? d.name.split(\" \")[1] || "
           "\"\" : \"\"; });\n"
           "\n"
           "  function click(d) {\n"
           "  path.transition()\n"
           "    .duration(duration)\n"
           "    .attrTween(\"d\", arcTween(d));\n"
           "\n"
           "  // Somewhat of a hack as we rely on arcTween updating the scales.\n"
           "  text\n"
           "    .style(\"visibility\", function(e) {\n"
           "    return isParentOf(d, e) ? null : d3.select(this).style(\"visibility\");\n"
           "    })\n"
           "    .transition().duration(duration)\n"
           "    .attrTween(\"text-anchor\", function(d) {\n"
           "    return function() {\n"
           "      return x(d.x + d.dx / 2) > Math.PI ? \"end\" : \"start\";\n"
           "    };\n"
           "    })\n"
           "    .attrTween(\"transform\", function(d) {\n"
           "    var multiline = (d.name || \"\").split(\" \").length > 1;\n"
           "    return function() {\n"
           "      var angle = x(d.x + d.dx / 2) * 180 / Math.PI - 90,\n"
           "        rotate = angle + (multiline ? -.5 : 0);\n"
           "      return \"rotate(\" + rotate + \")translate(\" + (y(d.y) + p) + \")rotate(\" + (angle "
           "> 90 ? -180 : 0) + \")\";\n"
           "    };\n"
           "    })\n"
           "    .style(\"opacity\", function(e) { return isParentOf(d, e) ? 1 : 1e-6; })\n"
           "    .each(\"end\", function(e) {\n"
           "    d3.select(this).style(\"visibility\", isParentOf(d, e) ? null : \"hidden\");\n"
           "    });\n"
           "  }\n"
           "\n"
           "\n"
           "function sizeMB(d) {\n"
           "//  if (d.children) {\n"
           "//  var sum = calcSum(d);\n"
           "//  return (sum / (1024*1024)).toFixed(2) + 'MB';\n"
           "//  } else {\n"
           "  return (d.size / (1024*1024)).toFixed(2) + 'MB';\n"
           "//  }\n"
           "}\n"
           "\n"
           "function calcSum(d) {\n"
           "  if(d.children) {\n"
           "  var sum = 0;\n"
           "  function recurse(d) {\n"
           "    if(d.children) d.children.forEach( function(child) { recurse(child); } );\n"
           "    else sum += d.size;\n"
           "  }\n"
           "  recurse(d,sum);\n"
           "  console.log(sum);\n"
           "  console.log(d.children);\n"
           "  return sum;\n"
           "  } else {\n"
           "  console.log(d.size);\n"
           "  return d.size;\n"
           "  }\n"
           "}\n"
           "\n"
           "function isParentOf(p, c) {\n"
           "  if (p === c) return true;\n"
           "  if (p.children) {\n"
           "  return p.children.some(function(d) {\n"
           "    return isParentOf(d, c);\n"
           "  });\n"
           "  }\n"
           "  return false;\n"
           "}\n"
           "\n"
           "function colour(d) {\n"
           "  return color(d.name);\n"
           "}\n"
           "\n"
           "// Interpolate the scales!\n"
           "function arcTween(d) {\n"
           "  var my = maxY(d),\n"
           "    xd = d3.interpolate(x.domain(), [d.x, d.x + d.dx]),\n"
           "    yd = d3.interpolate(y.domain(), [d.y, my]),\n"
           "    yr = d3.interpolate(y.range(), [d.y ? 20 : 0, r]);\n"
           "  return function(d) {\n"
           "  return function(t) { x.domain(xd(t)); y.domain(yd(t)).range(yr(t)); return arc(d); };\n"
           "  };\n"
           "}\n"
           "\n"
           "// Interpolate the scales!\n"
           "function arcTween2(d) {\n"
           "  var xd = d3.interpolate(x.domain(), [d.x, d.x + d.dx]),\n"
           "    yd = d3.interpolate(y.domain(), [d.y, 1]),\n"
           "    yr = d3.interpolate(y.range(), [d.y ? 20 : 0, radius]);\n"
           "  return function(d, i) {\n"
           "  return i\n"
           "    ? function(t) { return arc(d); }\n"
           "    : function(t) { x.domain(xd(t)); y.domain(yd(t)).range(yr(t)); return arc(d); };\n"
           "  };\n"
           "}\n"
           "\n"
           "function maxY(d) {\n"
           "  return d.children ? Math.max.apply(Math, d.children.map(maxY)) : d.y + d.dy;\n"
           "}\n"
           "\n"
           "// http://www.w3.org/WAI/ER/WD-AERT/#color-contrast\n"
           "function brightness(rgb) {\n"
           "  return rgb.r * .299 + rgb.g * .587 + rgb.b * .114;\n"
           "}\n"
           "d3.select(\"#download\").on(\"click\", function () {\n"
           "d3.select(this).attr(\"href\", 'data:application/octet-stream;base64,' + "
           "btoa(d3.select(\"#chart\").html())).attr(\"download\", \"memorysun.svg\")})\n\n"
           "click(nodes[0]);\n"
           "    "
        << std::endl
        << "</script>" << std::endl
        << "</body>" << std::endl
        << "</html>" << std::endl;
    return jsonbody.str();
}

template <>
inline void write_structure_tree<HTML_FORMAT>(const structure_tree_node* v,
                                              std::ostream&			  out,
                                              SDSL_UNUSED size_t level)
{
    std::stringstream json_data;
    write_structure_tree<JSON_FORMAT>(v, json_data);

    out << create_html_header("sdsl data structure visualization");
    out << create_js_body(json_data.str());
}

}
#endif

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file util.hpp
    \brief util.hpp contains some helper methods for int_vector and other stuff like demangle class names.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_UTIL
#define INCLUDED_SDSL_UTIL

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*!\file sfstream.hpp
   \brief sfstream.hpp contains a two stream class which can be used to read/write from/to files or strings.
   \author Simon Gog
*/
#ifndef INCLUDED_SDSL_SFSTREAM
#define INCLUDED_SDSL_SFSTREAM

#include <fstream>
#include <sstream>
#include <string>
// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file ram_fs.hpp
 * \brief ram_fs.hpp
 * \author Simon Gog
 */
#ifndef INCLUDED_SDSL_RAM_FS
#define INCLUDED_SDSL_RAM_FS

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*!\file memory_tracking.hpp
\brief memory_tracking.hpp contains two function for allocating and deallocating memory
\author Simon Gog
*/
#ifndef INCLUDED_SDSL_MEMORY_TRACKING
#define INCLUDED_SDSL_MEMORY_TRACKING

//#include "ram_fs.hpp"

#include <map>
#include <iostream>
#include <cstdlib>
#include <mutex>
#include <chrono>
#include <cstring>
#include <set>
#include <cstddef>
#include <stack>
#include <vector>
#include <atomic>

#include <fcntl.h>
#include <sstream>
#include <fstream>

#ifdef _WIN32
#ifndef NOMINMAX
// windows.h has min/max macro which causes problems when using std::min/max
#define NOMINMAX 1
#endif
#include <windows.h>
#include <io.h>
#else
#include <sys/mman.h>
#include <unistd.h> // for getpid, file_size, clock_gettime
#endif

namespace sdsl {

//forward declare
void memory_monitor_record(int64_t);

// minimal allocator from http://stackoverflow.com/a/21083096
template <typename T>
struct track_allocator {
    using value_type = T;

    track_allocator() = default;
    template <class U>
    track_allocator(const track_allocator<U>&) {}

    T* allocate(std::size_t n) {
        if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            size_t s = n * sizeof(T);
            if (auto ptr = std::malloc(s)) {
                memory_monitor_record(s);
                return static_cast<T*>(ptr);
            }
        }
        throw std::bad_alloc();
    }
    void deallocate(T* ptr, std::size_t n) {
        std::free(ptr);
        std::size_t s = n * sizeof(T);
        memory_monitor_record(-((int64_t)s));
    }
};

template <typename T, typename U>
inline bool operator == (const track_allocator<T>&, const track_allocator<U>&) {
    return true;
}

template <typename T, typename U>
inline bool operator != (const track_allocator<T>& a, const track_allocator<U>& b) {
    return !(a == b);
}

// spin lock
class spin_lock
{
private:
    std::atomic_flag m_slock;
public:
    spin_lock()
    {
        m_slock.clear();
    }
    void lock()
    {
        while (m_slock.test_and_set(std::memory_order_acquire)) {
            /* spin */
        }
    };
    void unlock()
    {
        m_slock.clear(std::memory_order_release);
    };
};

namespace ram_fs{
typedef std::vector<char, track_allocator<char>> content_type;
}

struct ramfs_storage {
    typedef std::map<std::string, ram_fs::content_type> mss_type;
    typedef std::map<int, std::string> mis_type;
    std::recursive_mutex m_rlock;

    mss_type m_map;
    mis_type m_fd_map;

    ramfs_storage(){
        m_fd_map[-1] = "";
    }

    ~ramfs_storage(){ }
};

struct mm_alloc {
    using timer = std::chrono::high_resolution_clock;
    timer::time_point timestamp;
    int64_t usage;
    mm_alloc(timer::time_point t, int64_t u) : timestamp(t), usage(u) {};
};

struct mm_event {
    using timer = std::chrono::high_resolution_clock;
    std::string name;
    std::vector<mm_alloc> allocations;
    mm_event(std::string n, int64_t usage) : name(n)
    {
        allocations.emplace_back(timer::now(), usage);
    };
    bool operator< (const mm_event& a) const
    {
        if (a.allocations.size() && this->allocations.size()) {
            if (this->allocations[0].timestamp == a.allocations[0].timestamp) {
                return this->allocations.back().timestamp < a.allocations.back().timestamp;
            } else {
                return this->allocations[0].timestamp < a.allocations[0].timestamp;
            }
        }
        return true;
    }
};

struct tracker_storage {
    using timer = std::chrono::high_resolution_clock;
    std::chrono::milliseconds log_granularity = std::chrono::milliseconds(20ULL);
    int64_t current_usage = 0;
    bool track_usage = false;
    std::vector<mm_event> completed_events;
    std::stack<mm_event> event_stack;
    timer::time_point start_log;
    timer::time_point last_event;
    spin_lock spinlock;

    tracker_storage(){ }

    ~tracker_storage(){ }
};

template<format_type F>
void write_mem_log(std::ostream& out, const tracker_storage& m);

class memory_monitor
{
public:
    using timer = std::chrono::high_resolution_clock;

    struct mm_event_proxy {
        bool add;
        timer::time_point created;
        mm_event_proxy(const std::string& name, int64_t usage, bool a) : add(a)
        {
            if (add) {
                auto& m = *(the_monitor().m_tracker);
                std::lock_guard<spin_lock> lock(m.spinlock);
                m.event_stack.emplace(name, usage);
            }
        }
        ~mm_event_proxy()
        {
            if (add) {
                auto& m = *(the_monitor().m_tracker);
                std::lock_guard<spin_lock> lock(m.spinlock);
                auto& cur = m.event_stack.top();
                auto cur_time = timer::now();
                cur.allocations.emplace_back(cur_time, m.current_usage);
                m.completed_events.emplace_back(std::move(cur));
                m.event_stack.pop();
                // add a point to the new "top" with the same memory
                // as before but just ahead in time
                if (!m.event_stack.empty()) {
                    if (m.event_stack.top().allocations.size()) {
                        auto last_usage = m.event_stack.top().allocations.back().usage;
                        m.event_stack.top().allocations.emplace_back(cur_time, last_usage);
                    }
                }
            }
        }
    };

private:
    tracker_storage* m_tracker;
    ramfs_storage* m_ram_fs;

    // disable construction of the object
    memory_monitor() {
        m_tracker = new tracker_storage();
        m_ram_fs = new ramfs_storage();

    };

    ~memory_monitor()
    {
        if (m_tracker->track_usage) {
            stop();
        }
        delete m_ram_fs;
        delete m_tracker;
    }
    memory_monitor(const memory_monitor&) = delete;
    memory_monitor& operator=(const memory_monitor&) = delete;

    static memory_monitor& the_monitor()
    {
        static memory_monitor m;
        return m;
    }
public:

    static void granularity(std::chrono::milliseconds ms)
    {
        auto& m = *(the_monitor().m_tracker);
        m.log_granularity = ms;
    }
    static int64_t peak()
    {
        auto& m = *(the_monitor().m_tracker);
        int64_t max = 0;
        for (auto events : m.completed_events) {
            for (auto alloc : events.allocations) {
                if (max < alloc.usage) {
                    max = alloc.usage;
                }
            }
        }
        return max;
    }

    static ramfs_storage& ram_fs(){
        return *(the_monitor().m_ram_fs);
    }

    static void start()
    {
        auto& m = *(the_monitor().m_tracker);
        m.track_usage = true;
        // clear if there is something there
        if (m.completed_events.size()) {
            m.completed_events.clear();
        }
        while (m.event_stack.size()) {
            m.event_stack.pop();
        }
        m.start_log = timer::now();
        m.current_usage = 0;
        m.last_event = m.start_log;
        m.event_stack.emplace("unknown", 0);
    }
    static void stop()
    {
        auto& m = *(the_monitor().m_tracker);
        while (!m.event_stack.empty()) {
            m.completed_events.emplace_back(std::move(m.event_stack.top()));
            m.event_stack.pop();
        }
        m.track_usage = false;
    }
    static void record(int64_t delta)
    {
        auto& m = *(the_monitor().m_tracker);
        if (m.track_usage) {
            std::lock_guard<spin_lock> lock(m.spinlock);
            auto cur = timer::now();
            if (m.last_event + m.log_granularity < cur) {
                m.event_stack.top().allocations.emplace_back(cur, m.current_usage);
                m.current_usage = m.current_usage + delta;
                m.event_stack.top().allocations.emplace_back(cur, m.current_usage);
                m.last_event = cur;
            } else {
                if (m.event_stack.top().allocations.size()) {
                    m.current_usage = m.current_usage + delta;
                    m.event_stack.top().allocations.back().usage = m.current_usage;
                    m.event_stack.top().allocations.back().timestamp = cur;
                }
            }
        }
    }

    static mm_event_proxy event(const std::string& name)
    {
        auto& m = *(the_monitor().m_tracker);
        if (m.track_usage) {
            return mm_event_proxy(name, m.current_usage, true);
        }
        return mm_event_proxy(name, m.current_usage, false);
    }

    template<format_type F>
    static void write_memory_log(std::ostream& out)
    {
        write_mem_log<F>(out, *(the_monitor().m_tracker));
    }
};

inline void memory_monitor_record(int64_t delta) {
    memory_monitor::record(delta);
}

} // end namespace

#endif

#include <string>
#include <map>
#include <vector>
#include <mutex>

namespace sdsl {

namespace ram_fs{

//! Check if the file exists
inline bool exists(const std::string& name){
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    return rf.m_map.find(name) != rf.m_map.end();
}

inline void store(const std::string& name, content_type data) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if (!exists(name)) {
        std::string cname = name;
        rf.m_map.insert(std::make_pair(std::move(cname), std::move(data)));
    } else {
        rf.m_map[name] = std::move(data);
    }
}


//! Get the file size
inline size_t file_size(const std::string& name) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if (exists(name)) {
        return rf.m_map[name].size();
    } else {
        return 0;
    }
}

//! Get the content
inline content_type& content(const std::string& name) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    return rf.m_map[name];
}

//! Remove the file with key `name`
inline int remove(const std::string& name) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if (exists(name)) {
        rf.m_map.erase(name);
    }
    return 0;
}

//! Rename the file. Change key `old_filename` into `new_filename`.
inline int rename(const std::string old_filename, const std::string new_filename){
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    rf.m_map[new_filename] = std::move(rf.m_map[old_filename]);
    remove(old_filename);
    return 0;
}

//! Get fd for file
inline int open(const std::string& name) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if(!exists(name)) {
        store(name,content_type{});
    }
    int fd = -2;
    auto largest_fd = rf.m_fd_map.rbegin()->first;
    if( largest_fd < 0 ) {
        auto smallest_fd = rf.m_fd_map.begin()->first;
        fd = smallest_fd - 1;
    } else {
        rf.m_fd_map.erase(largest_fd);
        fd = - largest_fd;
    }
    rf.m_fd_map[fd] = name;
    return fd;
}

//! Get fd for file
inline int close(const int fd) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if( fd >= -1 ) return -1;
    if(rf.m_fd_map.count(fd) == 0) {
        return -1;
    } else {
        rf.m_fd_map.erase(fd);
        rf.m_fd_map[-fd] = "";
    }
    return 0;
}

//! Get the content with fd
inline content_type& content(const int fd){
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    auto name = rf.m_fd_map[fd];
    return rf.m_map[name];
}

//! Get the content with fd
inline int truncate(const int fd,size_t new_size) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if(rf.m_fd_map.count(fd) == 0) return -1;
    auto name = rf.m_fd_map[fd];
    rf.m_map[name].reserve(new_size);
    rf.m_map[name].resize(new_size,0);
    return 0;
}

//! Get the file size with fd
inline size_t file_size(const int fd) {
    auto& rf = memory_monitor::ram_fs();
    std::lock_guard<std::recursive_mutex> lock(rf.m_rlock);
    if(rf.m_fd_map.count(fd) == 0) return 0;
    auto name = rf.m_fd_map[fd];
    return rf.m_map[name].size();
}

}// end namespace ram_fs

//! Determines if the given file is a RAM-file.
inline bool is_ram_file(const std::string& file) {
    if (file.size() > 0) {
        if (file[0]=='@') {
            return true;
        }
    }
    return false;
}

//! Determines if the given file is a RAM-file.
inline bool is_ram_file(const int fd) {
    return fd < -1;
}

//! Returns the corresponding RAM-file name for file.
inline std::string ram_file_name(const std::string& file) {
    if (is_ram_file(file)) {
        return file;
    } else {
        return "@" + file;
    }
}

//! Returns for a RAM-file the corresponding disk file name
inline std::string disk_file_name(const std::string& file) {
    if (!is_ram_file(file)) {
        return file;
    } else {
        return file.substr(1);
    }
}

//! Remove a file.
inline int remove(const std::string& file) {
    if (is_ram_file(file)) {
        return ram_fs::remove(file);
    } else {
        return std::remove(file.c_str());
    }

}

//! Rename a file
inline int rename(const std::string& old_filename, const std::string& new_filename){
    if (is_ram_file(old_filename)) {
        if (!is_ram_file(new_filename)) {  // error, if new file is not also RAM-file
            return -1;
        }
        return ram_fs::rename(old_filename, new_filename);
    } else {
        return std::rename(old_filename.c_str(), new_filename.c_str());
    }
}

} // end namespace sdsl
#endif

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
#ifndef INCLUDED_SDSL_RAM_FSTREAMBUF
#define INCLUDED_SDSL_RAM_FSTREAMBUF

#include <fstream>
#include <vector>

namespace sdsl {

class ram_filebuf : public std::streambuf
{
private:
    // TODO:  also store filename/descriptor to implement is_open ???
    ram_fs::content_type* m_ram_file = nullptr;  // file handle
    void pbump64(std::ptrdiff_t x)
    {
        while (x > std::numeric_limits<int>::max()) {
            pbump(std::numeric_limits<int>::max());
            x -= std::numeric_limits<int>::max();
        }
        pbump(x);
    }

public:
    virtual ~ram_filebuf() {};

    ram_filebuf() {};

    ram_filebuf(ram_fs::content_type& ram_file) : m_ram_file(&ram_file)
    {
        char* begin = m_ram_file->data();
        char* end   = begin + m_ram_file->size();
        setg(begin, begin, end); // set get pointers eback(), eptr(), egptr()
    }

    std::streambuf* open(const std::string name, std::ios_base::openmode mode)
    {
        // open ram_file
        if ((mode & std::ios_base::in) and !(mode & std::ios_base::trunc)) {
            // file must exist, initial position at the start
            if (!ram_fs::exists(name)) {
                m_ram_file = nullptr;
            } else {
                m_ram_file = &ram_fs::content(name);
            }
        } else { // existence of file not required
            if (!ram_fs::exists(name)) {
                // create empty file, if it does not yet exist
                ram_fs::store(
                    name, ram_fs::content_type()); // TODO: create method in ram_fs?? or store w 1 arg?
            }
            m_ram_file = &ram_fs::content(name);
            if ((mode & std::ios_base::out) and !(mode & std::ios_base::app)) {
                m_ram_file->clear();
            }
        }

        if (m_ram_file and (mode & std::ios_base::trunc)) {
            m_ram_file->clear();
        }
        if (m_ram_file) {
            if (mode & std::ios_base::ate) {
                // TODO: move put pointer to the end of the file
            }
            setg(m_ram_file->data(), m_ram_file->data(), m_ram_file->data() + m_ram_file->size());
            setp(m_ram_file->data(), m_ram_file->data() + m_ram_file->size());
        }
        // ATTENTION: if m_ram_file->size() == 0, then data might be nullptr !!!
        return m_ram_file ? this : nullptr;
    }

    bool is_open() { return m_ram_file != nullptr; }

    ram_filebuf* close()
    {
        if (!this->is_open()) return nullptr;
        m_ram_file = nullptr;
        setg(nullptr, nullptr, nullptr);
        setp(nullptr, nullptr);
        return this;
    }

    pos_type seekpos(pos_type				 sp,
                     std::ios_base::openmode mode = std::ios_base::in |
                         std::ios_base::out) override
    {
        if (sp >= (pos_type)0 and sp <= (pos_type)m_ram_file->size()) {
            setg(m_ram_file->data(), m_ram_file->data() + sp, m_ram_file->data() + m_ram_file->size());
            setp(m_ram_file->data(), m_ram_file->data() + m_ram_file->size());
            pbump64(sp);
        } else {
            if (mode & std::ios_base::out) {
                // extend buffer
                m_ram_file->reserve(sp);
                m_ram_file->resize(sp, 0);
                setg(
                    m_ram_file->data(), m_ram_file->data() + sp, m_ram_file->data() + m_ram_file->size());
                setp(m_ram_file->data(), m_ram_file->data() + m_ram_file->size());
                pbump64(sp);
            } else {
                return pos_type(off_type(-1));
            }
        }
        return sp;
    }

    pos_type pubseekoff(off_type				off,
                        std::ios_base::seekdir  way,
                        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
        if (std::ios_base::beg == way) {
            if (seekpos(off, which) == pos_type(-1)) {
                return pos_type(-1);
            }
        } else if (std::ios_base::cur == way) {
            if (seekpos(gptr() - eback() + off, which) == pos_type(-1)) {
                return pos_type(-1);
            }
        } else if (std::ios_base::end == way) {
            if (seekpos(egptr() - eback() + off, which) == pos_type(-1)) {
                return pos_type(-1);
            }
        }
        return gptr() - eback();
    }

    pos_type pubseekpos(pos_type				sp,
                        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
        if (seekpos(sp, which) == pos_type(-1)) {
            return pos_type(-1);
        } else {
            return gptr() - eback();
        }
    }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override
    {
        //    std::cout<<"xsputn( , of size "<<n<<")"<<std::endl;
        //    std::cout<<"epptr()-pptr()="<<epptr()-pptr()<<std::endl;
        if (!m_ram_file) {
            return 0;
        }

        if (n < epptr() - pptr()) {
            std::copy(s, s + n, pptr());
            pbump64(n);
            return n;
        } else {
            if (epptr() - pbase() == (std::ptrdiff_t)m_ram_file->size() and epptr() == pptr()) {
                m_ram_file->insert(m_ram_file->end(), s, s + n);
                setp(m_ram_file->data(), m_ram_file->data() + m_ram_file->size());
                std::ptrdiff_t add = epptr() - pbase();
                pbump64(add);
                setg(m_ram_file->data(), gptr(), m_ram_file->data() + m_ram_file->size());
                return n;
            } else {
                for (std::streamsize i = 0; i < n; ++i) {
                    if (traits_type::eq_int_type(sputc(s[i]), traits_type::eof())) {
                        return i;
                    }
                }
                return n;
            }
        }
    }

    int sync() override
    {
        return 0; // we are always in sync, since buffer is sink
    }

    int_type overflow(int_type c = traits_type::eof()) override
    {
        if (m_ram_file) {
            m_ram_file->push_back(c);
            setp(m_ram_file->data(), m_ram_file->data() + m_ram_file->size());
            std::ptrdiff_t add = epptr() - pbase();
            pbump64(add);
            setg(m_ram_file->data(), gptr(), m_ram_file->data() + m_ram_file->size());
        }
        return traits_type::to_int_type(c);
    }
};
}

#endif

namespace sdsl {

class osfstream : public std::ostream {
public:
    typedef std::streambuf* buf_ptr_type;

private:
    buf_ptr_type m_streambuf = nullptr;
    std::string  m_file		 = "";

public:
    typedef void* voidptr;
    //! Standard constructor.
    osfstream() : std::ostream(nullptr) { this->init(m_streambuf); }

    //! Constructor taking a file name and open mode.
    osfstream(const std::string& file, std::ios_base::openmode mode = std::ios_base::out) : std::ostream(nullptr)
    {
        this->init(m_streambuf);
        open(file, mode);
    }

    //! Open the stream.
    buf_ptr_type open(const std::string& file, std::ios_base::openmode mode = std::ios_base::out)
    {
        delete m_streambuf;
        m_streambuf				= nullptr;
        m_file					= file;
        std::streambuf* success = nullptr;
        if (is_ram_file(file)) {
            m_streambuf = new ram_filebuf();
            success		= ((ram_filebuf*)m_streambuf)->open(m_file, mode | std::ios_base::out);
        } else {
            m_streambuf = new std::filebuf();
            success		= ((std::filebuf*)m_streambuf)->open(m_file, mode | std::ios_base::out);
        }
        if (success) {
            this->clear();
        } else {
            this->setstate(std::ios_base::failbit);
            delete m_streambuf;
            m_streambuf = nullptr;
        }
        this->rdbuf(m_streambuf);
        return m_streambuf;
    }

    //! Is the stream close?
    bool is_open()
    {
        if (nullptr == m_streambuf) return false;
        if (is_ram_file(m_file)) {
            return ((ram_filebuf*)m_streambuf)->is_open();
        } else {
            return ((std::filebuf*)m_streambuf)->is_open();
        }
    }

    //! Close the stream.
    void close()
    {
        bool fail = false;
        if (nullptr == m_streambuf) {
            fail = true;
        } else {
            if (is_ram_file(m_file)) {
                fail = !((ram_filebuf*)m_streambuf)->close();
            } else {
                fail = !((std::filebuf*)m_streambuf)->close();
            }
        }
        if (fail) this->setstate(std::ios::failbit);
    }

    //! Standard destructor
    ~osfstream()
    {
        delete m_streambuf; // streambuf closes the file on destruction
    }

    //! Cast to void*
    operator voidptr() const { return m_streambuf; }

    osfstream& seekp(pos_type pos)
    {
        ios_base::iostate err = std::ios_base::iostate(std::ios_base::goodbit);
        try {
            if (!this->fail()) {
                pos_type p = 0;
                if (is_ram_file(m_file)) {
                    p = ((ram_filebuf*)m_streambuf)->pubseekpos(pos, std::ios_base::out);
                } else {
                    p = ((std::filebuf*)m_streambuf)->pubseekpos(pos, std::ios_base::out);
                }
                if (p == pos_type(off_type(-1))) {
                    err |= ios_base::failbit;
                    this->setstate(err);
                }
            }
        } catch (...) {
            if (err) {
                this->setstate(err);
            }
        }
        return *this;
    }

    osfstream& seekp(off_type off, ios_base::seekdir way)
    {
        ios_base::iostate err = std::ios_base::iostate(ios_base::goodbit);
        try {
            if (!this->fail()) {
                pos_type p = 0;
                if (is_ram_file(m_file)) {
                    p = ((ram_filebuf*)m_streambuf)->pubseekoff(off, way, std::ios_base::out);

                } else {
                    p = ((std::filebuf*)m_streambuf)->pubseekoff(off, way, std::ios_base::out);
                }
                if (p == pos_type(off_type(-1))) {
                    err |= ios_base::failbit;
                    this->setstate(err);
                }
            }
        } catch (...) {
            if (err) {
                this->setstate(err);
            }
        }
        return *this;
    }

    std::streampos tellp();
};

class isfstream : public std::istream {
    typedef std::streambuf* buf_ptr_type;

private:
    buf_ptr_type m_streambuf = nullptr;
    std::string  m_file		 = "";

public:
    typedef void* voidptr;
    //! Standard constructor.
    isfstream() : std::istream(nullptr) { this->init(m_streambuf); }

    //! Constructor taking a file name and open mode.
    isfstream(const std::string& file, std::ios_base::openmode mode = std::ios_base::in)
        : std::istream(nullptr)
    {
        this->init(m_streambuf);
        open(file, mode);
    }

    //! Open the stream.
    buf_ptr_type open(const std::string& file, std::ios_base::openmode mode = std::ios_base::in)
    {
        delete m_streambuf;
        m_streambuf				= nullptr;
        m_file					= file;
        std::streambuf* success = nullptr;
        if (is_ram_file(file)) {
            m_streambuf = new ram_filebuf();
            success		= ((ram_filebuf*)m_streambuf)->open(m_file, mode | std::ios_base::in);
        } else {
            m_streambuf = new std::filebuf();
            success		= ((std::filebuf*)m_streambuf)->open(m_file, mode | std::ios_base::in);
        }
        if (success) {
            this->clear();
        } else {
            this->setstate(std::ios_base::failbit);
            delete m_streambuf;
            m_streambuf = nullptr;
        }
        this->rdbuf(m_streambuf);
        return m_streambuf;
    }

    //! Is the stream close?
    bool is_open()
    {
        if (nullptr == m_streambuf) return false;
        if (is_ram_file(m_file)) {
            return ((ram_filebuf*)m_streambuf)->is_open();
        } else {
            return ((std::filebuf*)m_streambuf)->is_open();
        }
    }

    //! Close the stream.
    void close()
    {
        bool fail = false;
        if (nullptr == m_streambuf) {
            fail = true;
        } else {
            if (is_ram_file(m_file)) {
                fail = !((ram_filebuf*)m_streambuf)->close();
            } else {
                fail = !((std::filebuf*)m_streambuf)->close();
            }
        }
        if (fail) this->setstate(std::ios::failbit);
    }

    //! Standard destructor
    ~isfstream() { delete m_streambuf; }

    //! Cast to void*
    operator voidptr() const
    {
        return m_streambuf; // streambuf closes the file on destruction
    }

    isfstream& seekg(pos_type pos)
    {
        ios_base::iostate err = std::ios_base::iostate(std::ios_base::goodbit);
        try {
            if (!this->fail()) {
                pos_type p = 0;
                if (is_ram_file(m_file)) {
                    p = ((ram_filebuf*)m_streambuf)->pubseekpos(pos, std::ios_base::in);

                } else {
                    p = ((std::filebuf*)m_streambuf)->pubseekpos(pos, std::ios_base::in);
                }
                if (p == pos_type(off_type(-1))) {
                    err |= ios_base::failbit;
                }
            }
        } catch (...) {
            if (err) {
                this->setstate(err);
            }
        }
        return *this;
    }

    isfstream& seekg(off_type off, ios_base::seekdir way)
    {
        ios_base::iostate err = std::ios_base::iostate(ios_base::goodbit);
        try {
            if (!this->fail()) {
                pos_type p = 0;
                if (is_ram_file(m_file)) {
                    p = ((ram_filebuf*)m_streambuf)->pubseekoff(off, way, std::ios_base::in);

                } else {
                    p = ((std::filebuf*)m_streambuf)->pubseekoff(off, way, std::ios_base::in);
                }
                if (p == pos_type(off_type(-1))) {
                    err |= ios_base::failbit;
                }
            }
        } catch (...) {
            if (err) {
                this->setstate(err);
            }
        }
        return *this;
    }

    std::streampos tellg()
    {
        ios_base::iostate err = std::ios_base::iostate(ios_base::goodbit);
        pos_type		  p   = pos_type(off_type(-1));
        try {
            if (!this->fail()) {
                if (is_ram_file(m_file)) {
                    p = ((ram_filebuf*)m_streambuf)->pubseekoff(0, std::ios_base::cur);

                } else {
                    p = ((std::filebuf*)m_streambuf)->pubseekoff(0, std::ios_base::cur);
                }
                if (p == pos_type(off_type(-1))) {
                    err |= ios_base::failbit;
                }
            }
        } catch (...) {
            if (err) {
                this->setstate(err);
            }
        }
        return p;
    }
};

} // end namespace

#endif

#include <iosfwd>	 // forward declaration of ostream
#include <stdint.h>   // for uint64_t uint32_t declaration
#include <cassert>
#include <ctime> // for rand initialization
#include <string>
#include <functional> // for class_to_hash
#include <string.h>   // for strlen and strdup
#include <cstdlib>
#include <sstream>   // for to_string method
#include <stdexcept> // for std::logic_error
#include <typeinfo>  // for typeid
#include <iomanip>
#include <numeric>
#include <random>
#include <chrono>
#include <atomic>
#include <mutex>
#include <algorithm>

// macros to transform a defined name to a string
#define SDSL_STR(x) #x
#define SDSL_XSTR(s) SDSL_STR(s)

#include <sys/types.h> // for file_size
#include <sys/stat.h>  // for file_size
#include <iomanip>
#include <vector>
#include <string>

#include <type_traits>
#include <typeinfo>

#ifndef MSVC_COMPILER
#include <cxxabi.h>
#endif

#ifndef _WIN32
#include <sys/time.h>	  // for struct timeval
#include <sys/resource.h> // for struct rusage
#include <libgen.h>		  // for basename
#include <unistd.h>		  // for getpid, file_size, clock_gettime
#else
#include <process.h>
#include <ciso646>
#endif

//! Namespace for the succinct data structure library.
namespace sdsl {

template <uint8_t>
class int_vector; // forward declaration

//! A namespace for helper functions
namespace util {

//============= Debug information =========================

SDSL_UNUSED static bool verbose = false;

inline void set_verbose() { verbose = true; }

//============ Manipulating int_vectors ===================

//! Sets all bits of the int_vector to pseudo-random bits.
/*! \param v The int_vector whose bits should be set to random bits
 *  \param seed If seed = 0, the time is used to initialize the
 *              pseudo random number generator, otherwise the seed
 *              parameter is used.
 */
template <class t_int_vec>
void set_random_bits(t_int_vec& v, int seed = 0);
//! Sets all bits of the int_vector to 0-bits.
template <class t_int_vec>
void _set_zero_bits(t_int_vec& v);
//! Sets all bits of the int_vector to 1-bits.
template <class t_int_vec>
void _set_one_bits(t_int_vec& v);

//! Bit compress the int_vector
/*! Determine the biggest value X and then set the
 *  int_width to the smallest possible so that we
 *  still can represent X
 */
template <class t_int_vec>
void bit_compress(t_int_vec& v);

//! Expands the integer width to new_width >= v.width()
template <class t_int_vec>
void expand_width(t_int_vec& v, uint8_t new_width);

//! All elements of v modulo m
template <class t_int_vec>
void mod(t_int_vec& v, typename t_int_vec::size_type m);

inline void cyclic_shifts(uint64_t* vec, uint8_t & n, uint64_t k, uint8_t int_width);

//! Set all entries of int_vector to value k
/*! \param  v The int_vector which should be set
 *  \param  k The value which should be inserted into v.
 *  \par Details
 *   This method pre-calculates the content of at most 64
 *   words and then repeatedly inserts these words into v.
 */
template <class t_int_vec>
void set_to_value(t_int_vec& v, uint64_t k);

//! Set all entries of int_vector starting from iterator it to value k
/*! \param  v The int_vector which should be set
 *  \param  k The value which should be inserted into v.
 *  \param  it The iterator from which on all elements are set to value k.
 *  \par Details
 *   This method pre-calculates the content of at most 64
 *   words and then repeatedly inserts these words into v.
 */
template <class t_int_vec, class t_int_vec_iterator>
void set_to_value(t_int_vec& v, uint64_t k, t_int_vec_iterator it);

//! Sets each entry of the numerical vector v at position \$fi\f$ to value \$fi\$f
template <class t_int_vec>
void set_to_id(t_int_vec& v);

//! Number of set bits in v.
/*! \param v  int_vector object.
      \return The number of 1-bits in v.
 */
template <class t_int_vec>
typename t_int_vec::size_type cnt_one_bits(const t_int_vec& v);

//! Number of occurrences of bit pattern `10` in v.
/*! \sa getOneBits, getOneZeroBits
 */
template <class t_int_vec>
typename t_int_vec::size_type cnt_onezero_bits(const t_int_vec& v);

//! Number of occurrences of bit pattern `01` in v.
/*! \sa getOneBits, getZeroOneBits
 */
template <class t_int_vec>
typename t_int_vec::size_type cnt_zeroone_bits(const t_int_vec& v);

//! Get the smallest position \f$i\geq idx\f$ where a bit is set
/*! \param v The int_vector in which the bit is searched
 *  \param idx The start position for the search \f$ 0\leq idx < v.bit_size()\f$
 *  \return The smallest position greater or equal to idx, where corresponding bit is 1 or v.bit_size() if no such position exists
 *  \par Time complexity
 *      \f$ \Order{n} \f$
 */
template <class t_int_vec>
typename t_int_vec::size_type next_bit(const t_int_vec& v, uint64_t idx);

//! Get the greatest position \f$i\leq idx\f$ where a bit is set
/*! \param v The int_vector in which the bit is searched
 *  \param idx The start position for the search \f$ 0\leq idx < v.bit_size()\f$
 *  \return The greatest position smaller or equal to idx, where corresponding bit is 1 or v.bit_size() if no such position exists
 *  \par Time complexity
 *     \f$ \Order{n} \f$
*/
template <class t_int_vec>
typename t_int_vec::size_type prev_bit(const t_int_vec& v, uint64_t idx);

//============= Handling files =============================

//! Get the size of a file in bytes
/*! \param file  Path to a file.
 *  \returns     Size of the specified file in bytes.
 */
inline size_t file_size(const std::string& file)
{
    if (is_ram_file(file)) {
        return ram_fs::file_size(file);
    } else {
        struct stat fs;
        stat(file.c_str(), &fs);
        return fs.st_size;
    }
}

//! Returns the basename of a file
/*! \param file  Path to a file.
 *  \returns     Basename of the specified file.
 */
inline std::string basename(std::string file)
{
    file = disk_file_name(file); // remove RAM-prefix
#ifdef _WIN32
    char* c						= _strdup((const char*)file.c_str());
	char  file_name[_MAX_FNAME] = {0};
#ifdef MSVC_COMPILER
	::_splitpath_s(c, NULL, 0, NULL, NULL, file_name, _MAX_FNAME, NULL, 0);
#else
	::_splitpath(c, NULL, NULL, file_name, NULL);
#endif
	std::string res(file_name);
#else
    char*		c   = strdup((const char*)file.c_str());
    std::string res = std::string(::basename(c));
#endif
    free(c);
    return res;
}

//! Returns the directory of a file. A trailing `/` will be removed.
/*! \param file  Path to a file.
 *  \returns     Directory name part of the specified path.
 */
inline std::string dirname(std::string file)
{
    bool ram_file = is_ram_file(file);
    file		  = disk_file_name(file); // remove RAM-prefix
#ifdef _WIN32
    char* c                  = _strdup((const char*)file.c_str());
	char  dir_name[_MAX_DIR] = {0};
	char  drive[_MAX_DRIVE]  = {0};
#ifdef MSVC_COMPILER
    ::_splitpath_s(c, drive, _MAX_DRIVE, dir_name, _MAX_DIR, NULL, 0, NULL, 0);
#else
    ::_splitpath  (c, drive, dir_name,NULL, NULL);
#endif
	std::string res = std::string(drive) + std::string(dir_name);
#else
    char*		c   = strdup((const char*)file.c_str());
    std::string res = std::string(::dirname(c));
    auto it = res.begin();
    auto next_it = res.begin()+1;
    while ( it != res.end() and next_it != res.end() ) {
        if ( *next_it != '/' or *it != '/' ) {
            *(++it) = *next_it;
        }
        ++next_it;
    }
    res.resize(it - res.begin() + 1);
#endif
    free(c);
    if (ram_file) {
        if ("." == res) {
            res = ram_file_name("");
        } else if ("/" == res) {
            res = ram_file_name(res);
        }
    }
    return res;
}

//! Demangle the class name of typeid(...).name()
/*!
 * \param name A pointer to the result of typeid(...).name()
 */
inline std::string demangle(const std::string& name)
{
#if 1
    char   buf[4096];
    size_t size   = 4096;
    int	status = 0;
    abi::__cxa_demangle(name.c_str(), buf, &size, &status);
    if (status == 0) return std::string(buf);
    return name;
#else
    return name;
#endif
}

//! Demangle the class name of typeid(...).name() and remove the "sdsl::"-prefix, "unsigned int",...
inline std::string demangle2(const std::string& name)
{
    std::string				 result = demangle(name);
    std::vector<std::string> words_to_delete;
    words_to_delete.push_back("sdsl::");
    words_to_delete.push_back("(unsigned char)");
    words_to_delete.push_back(", unsigned long");

    for (size_t k = 0; k < words_to_delete.size(); ++k) {
        std::string w = words_to_delete[k];
        for (size_t i = result.find(w); i != std::string::npos; i = result.find(w, i)) {
            result.erase(i, w.length());
            ++i;
        }
    }
    size_t		index	  = 0;
    std::string to_replace = "int_vector<1>";
    while ((index = result.find(to_replace, index)) != std::string::npos) {
        result.replace(index, to_replace.size(), "bit_vector");
    }
    return result;
}

//! Convert type to string
template <typename T>
std::string to_string(const T& t, int w);

//! Transforms the demangled class name of an object to a hash value.
template <class T>
uint64_t hashvalue_of_classname(const T&)
{
    std::hash<std::string> str_hash;
    return str_hash(sdsl::util::demangle2(typeid(T).name()));
}

//! Transforms the demangled class name of an object to a hash value.
template <class T>
std::string class_to_hash(const T& t)
{
    return to_string(hashvalue_of_classname(t));
}

template <class T>
std::string class_name(const T& t)
{
    std::string result		 = demangle2(typeid(t).name());
    size_t		template_pos = result.find("<");
    if (template_pos != std::string::npos) {
        result = result.erase(template_pos);
    }
    return result;
}

// convert an errno number to a readable msg
inline char* str_from_errno()
{
#ifdef MSVC_COMPILER
    #pragma warning(disable : 4996)
	return strerror(errno);
#pragma warning(default : 4996)
#else
    return strerror(errno);
#endif
}

struct _id_helper_struct {
    uint64_t id = 0;
};

extern inline uint64_t _id_helper() {
    static _id_helper_struct data;
    return data.id++;
}

//! Get the process id of the current process
inline uint64_t pid()
{
#ifdef MSVC_COMPILER
    return _getpid();
#else
    return getpid();
#endif
}

//! Get a unique id inside the process
inline uint64_t id() { return _id_helper(); }

template <typename T>
std::string to_latex_string(const T& t);

inline std::string to_latex_string(unsigned char c)
{
    if (c == '_')
        return "\\_";
    else if (c == '\0')
        return "\\$";
    else
        return to_string(c);
}

//! Delete all files of the file_map.
inline void delete_all_files(tMSS& file_map)
{
    for (auto file_pair : file_map) {
        sdsl::remove(file_pair.second);
    }
    file_map.clear();
}

//! clear the space used by x
/*!
 * \param x Reference to the data structure.
 */
template <class T>
void clear(T& x)
{
    T y;
    x = std::move(y);
}

//! Swap support data structure and assign to new vector
/*! \param s1 First support structure.
 *  \param s2 Second support structure.
 *  \param p1 First supported structure.
 *  \param p2 Second supported structure.
 *  s1 is swapped with s2 and after the execution s1 supports p1 and s2 supports
 *  p2. I.e. if p1 and p2 are members of a complex data structure, we have to
 *  swap p1 and p2 before we use this method.
 */
template <class S, class P>
void swap_support(S& s1, S& s2, const P* p1, const P* p2)
{
    std::swap(s1, s2);
    s1.set_vector(p1);
    s2.set_vector(p2);
}

//! Initialise support data structure with
/*! \param s Support structure which should be initialized
 *  \param x Pointer to the data structure which should be supported.
 */
template <class S, class X>
void init_support(S& s, const X* x)
{
    S temp(x);			 // generate a temporary support object
    s = std::move(temp); // swap its content with the target object
    s.set_vector(x);	 // set the support object's  pointer to x
}

//! Create 2^{log_s} random integers mod m with seed x
/*
 */
template <class t_int_vec>
t_int_vec rnd_positions(uint8_t log_s, uint64_t& mask, uint64_t mod = 0, uint64_t seed = 17)
{
    mask = (1 << log_s) - 1;
    t_int_vec rands(1 << log_s, 0);
    set_random_bits(rands, seed);
    if (mod > 0) {
        util::mod(rands, mod);
    }
    return rands;
}

//! Checks at compile time whether type is regular or not
/*  static_assert(is_regular<YOUR_TYPE>::value);
 *  Code is from a talk of Aerix Consulting
 */
template <typename T>
struct is_regular
    : std::integral_constant<bool,
                             std::is_default_constructible<T>::value &&
                                 std::is_copy_constructible<T>::value &&
                                 std::is_move_constructible<T>::value &&
                                 std::is_copy_assignable<T>::value && std::is_move_assignable<T>::value> {
};

} // end namespace util

//==================== Template functions ====================

template <class t_int_vec>
void util::set_random_bits(t_int_vec& v, int seed)
{
    std::mt19937_64 rng;
    if (0 == seed) {
        rng.seed(std::chrono::system_clock::now().time_since_epoch().count() + util::id());
    } else
        rng.seed(seed);

    uint64_t* data = v.data();
    if (v.empty()) return;
    *data = rng();
    for (typename t_int_vec::size_type i = 1; i < ((v.bit_size() + 63) >> 6); ++i) {
        *(++data) = rng();
    }
}

// all elements of vector v modulo m
template <class t_int_vec>
void util::mod(t_int_vec& v, typename t_int_vec::size_type m)
{
    for (typename t_int_vec::size_type i = 0; i < v.size(); ++i) {
        v[i] = v[i] % m;
    }
}

template <class t_int_vec>
void util::bit_compress(t_int_vec& v)
{
    auto	 max_elem = std::max_element(v.begin(), v.end());
    uint64_t max	  = 0;
    if (max_elem != v.end()) {
        max = *max_elem;
    }
    uint8_t min_width = bits::hi(max) + 1;
    uint8_t old_width = v.width();
    if (old_width > min_width) {
        const uint64_t* read_data	= v.data();
        uint64_t*		write_data   = v.data();
        uint8_t			read_offset  = 0;
        uint8_t			write_offset = 0;
        for (typename t_int_vec::size_type i = 0; i < v.size(); ++i) {
            uint64_t x = bits::read_int_and_move(read_data, read_offset, old_width);
            bits::write_int_and_move(write_data, x, write_offset, min_width);
        }
        v.bit_resize(v.size() * min_width);
        v.width(min_width);
        // v.shrink_to_fit(); TODO(cpockrandt): comment in once int_vector_mapper has the same interface
    }
}

template <class t_int_vec>
void util::expand_width(t_int_vec& v, uint8_t new_width)
{
    uint8_t						  old_width = v.width();
    typename t_int_vec::size_type n			= v.size();
    if (new_width > old_width) {
        if (n > 0) {
            typename t_int_vec::size_type i, old_pos, new_pos;
            new_pos = (n - 1) * new_width;
            old_pos = (n - 1) * old_width;
            v.bit_resize(v.size() * new_width);
            for (i = 0; i < n; ++i, new_pos -= new_width, old_pos -= old_width) {
                v.set_int(new_pos, v.get_int(old_pos, old_width), new_width);
            }
        }
        v.width(new_width);
    }
}

template <class t_int_vec>
void util::_set_zero_bits(t_int_vec& v)
{
    uint64_t* data = v.data();
    if (v.empty()) return;
    // TODO: replace by memset() but take care of size_t in the argument!
    *data = 0ULL;
    for (typename t_int_vec::size_type i = 1; i < ((v.bit_size() + 63) >> 6); ++i) {
        *(++data) = 0ULL;
    }
}

template <class t_int_vec>
void util::_set_one_bits(t_int_vec& v)
{
    uint64_t* data = v.data();
    if (v.empty()) return;
    *data = 0xFFFFFFFFFFFFFFFFULL;
    for (typename t_int_vec::size_type i = 1; i < ((v.bit_size() + 63) >> 6); ++i) {
        *(++data) = 0xFFFFFFFFFFFFFFFFULL;
    }
}

inline void util::cyclic_shifts(uint64_t* vec, uint8_t & n, uint64_t k, uint8_t int_width)
{
    n = 0;
    vec[0] = 0;
    uint8_t offset = 0;
    k &= 0xFFFFFFFFFFFFFFFFULL >> (64 - int_width);
    do { // loop terminates after at most 64 iterations
        vec[n] |= k << offset;
        offset += int_width;
        if (offset >= 64) {
            ++n;
            if (int_width == 64)
                return;
            assert(int_width - (offset - 64) < 64);
            vec[n] = k >> (int_width - (offset - 64));
            offset -= 64;
        }
    } while (offset != 0);
}

template <class t_int_vec>
void util::set_to_value(t_int_vec& v, uint64_t k)
{
    uint64_t* data = v.data();
    if (v.empty()) return;
    uint8_t int_width = v.width();
    if (int_width == 0) {
        throw std::logic_error("util::set_to_value can not be performed with int_width=0!");
    }
    if (0 == k) {
        _set_zero_bits(v);
        return;
    }
    if (bits::lo_set[int_width] == k) {
        _set_one_bits(v);
        return;
    }
    uint8_t n;
    uint64_t vec[65];
    util::cyclic_shifts(vec, n, k, int_width);

    typename t_int_vec::size_type n64 = (v.bit_size() + 63) >> 6;
    for (typename t_int_vec::size_type i = 0; i < n64;) {
        for (uint64_t ii = 0; ii < n and i < n64; ++ii, ++i) {
            *(data++) = vec[ii];
        }
    }
}

template <class t_int_vec, class t_int_vec_iterator>
void util::set_to_value(t_int_vec& v, uint64_t k, t_int_vec_iterator it)
{
    typedef typename t_int_vec::size_type size_type;

    if (v.empty()) return;
    uint8_t int_width = v.width();
    if (int_width == 0) {
        throw std::logic_error("util::set_to_value can not be performed with int_width=0!");
    }
    uint8_t n;
    uint64_t vec[65];
    util::cyclic_shifts(vec, n, k, int_width);

    size_type words        = (v.bit_size() + 63) >> 6;
    size_type word_pos     = ((it - v.begin()) * int_width) >> 6;
    uint8_t   pos_in_word  = ((it - v.begin()) * int_width) - (word_pos << 6); // ((it - v.begin()) * int_width) % 64
    uint8_t   cyclic_shift = word_pos % n;

    uint64_t* data = v.data() + word_pos;
    *(data) &= bits::lo_set[pos_in_word]; // unset first bits
    *(data) |= bits::lo_unset[pos_in_word] & vec[cyclic_shift++]; // set last bits
    ++word_pos;

    while (word_pos < words) {
        for (; cyclic_shift < n && word_pos < words; ++cyclic_shift, ++word_pos) {
            *(++data) = vec[cyclic_shift];
        }
        cyclic_shift = 0;
    }
}

//! Set v[i] = i for i=[0..v.size()-1]
template <class t_int_vec>
void util::set_to_id(t_int_vec& v)
{
    std::iota(v.begin(), v.end(), 0ULL);
}

template <class t_int_vec>
typename t_int_vec::size_type util::cnt_one_bits(const t_int_vec& v)
{
    const uint64_t* data = v.data();
    if (v.empty()) return 0;
    typename t_int_vec::size_type result = bits::cnt(*data);
    for (typename t_int_vec::size_type i = 1; i < ((v.bit_size() + 63) >> 6); ++i) {
        result += bits::cnt(*(++data));
    }
    if (v.bit_size() & 0x3F) {
        result -= bits::cnt((*data) & (~bits::lo_set[v.bit_size() & 0x3F]));
    }
    return result;
}

template <class t_int_vec>
typename t_int_vec::size_type util::cnt_onezero_bits(const t_int_vec& v)
{
    const uint64_t* data = v.data();
    if (v.empty()) return 0;
    uint64_t					  carry = 0, oldcarry = 0;
    typename t_int_vec::size_type result = bits::cnt10(*data, carry);
    for (typename t_int_vec::size_type i = 1; i < ((v.bit_size() + 63) >> 6); ++i) {
        oldcarry = carry;
        result += bits::cnt10(*(++data), carry);
    }
    if (v.bit_size() &
        0x3F) { // if bit_size is not a multiple of 64, subtract the counts of the additional bits
        result -= bits::cnt(bits::map10(*data, oldcarry) & bits::lo_unset[v.bit_size() & 0x3F]);
    }
    return result;
}

template <class t_int_vec>
typename t_int_vec::size_type util::cnt_zeroone_bits(const t_int_vec& v)
{
    const uint64_t* data = v.data();
    if (v.empty()) return 0;
    uint64_t					  carry = 1, oldcarry = 1;
    typename t_int_vec::size_type result = bits::cnt01(*data, carry);
    for (typename t_int_vec::size_type i = 1; i < ((v.bit_size() + 63) >> 6); ++i) {
        oldcarry = carry;
        result += bits::cnt01(*(++data), carry);
    }
    if (v.bit_size() &
        0x3F) { // if bit_size is not a multiple of 64, subtract the counts of the additional bits
        result -= bits::cnt(bits::map01(*data, oldcarry) & bits::lo_unset[v.bit_size() & 0x3F]);
    }
    return result;
}

template <class t_int_vec>
typename t_int_vec::size_type util::next_bit(const t_int_vec& v, uint64_t idx)
{
    uint64_t pos  = idx >> 6;
    uint64_t node = v.data()[pos];
    node >>= (idx & 0x3F);
    if (node) {
        return idx + bits::lo(node);
    } else {
        ++pos;
        while ((pos << 6) < v.bit_size()) {
            if (v.data()[pos]) {
                return (pos << 6) | bits::lo(v.data()[pos]);
            }
            ++pos;
        }
        return v.bit_size();
    }
}

template <class t_int_vec>
typename t_int_vec::size_type util::prev_bit(const t_int_vec& v, uint64_t idx)
{
    uint64_t pos  = idx >> 6;
    uint64_t node = v.data()[pos];
    node <<= 63 - (idx & 0x3F);
    if (node) {
        return bits::hi(node) + (pos << 6) - (63 - (idx & 0x3F));
    } else {
        --pos;
        while ((pos << 6) < v.bit_size()) {
            if (v.data()[pos]) {
                return (pos << 6) | bits::hi(v.data()[pos]);
            }
            --pos;
        }
        return v.bit_size();
    }
}

template <typename T>
std::string util::to_string(const T& t, int w)
{
    std::stringstream ss;
    ss << std::setw(w) << t;
    return ss.str();
}

template <typename T>
std::string util::to_latex_string(const T& t)
{
    return to_string(t);
}

} // end namespace sdsl
#endif

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file io.hpp
    \brief io.hpp contains some methods for reading/writing sdsl structures.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_IO
#define INCLUDED_SDSL_IO

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file platform.hpp
    \brief util.hpp contains platform dependend macros.
    \author Christopher Pockrandt
*/
#ifndef INCLUDED_SDSL_PLATFORM
#define INCLUDED_SDSL_PLATFORM

//! Namespace for the succinct data structure library.
namespace sdsl {

#if defined(__clang__)
#define COMPILER_CLANG
#endif

#if defined(__GNUC__) && !defined(COMPILER_CLANG)
#define COMPILER_GCC
#endif

// eliminate fallthrough warnings
#define SDSL_FALLTHROUGH
#if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(fallthrough)
#undef SDSL_FALLTHROUGH
        #if __cplusplus < 201500 && defined(COMPILER_GCC)
            #define SDSL_FALLTHROUGH [[gnu::fallthrough]];
        #elif __cplusplus < 201500 && defined(COMPILER_CLANG)
            #define SDSL_FALLTHROUGH [[clang::fallthrough]];
        #else
            #define SDSL_FALLTHROUGH [[fallthrough]];
        #endif
    #endif
#endif

} // end namespace sdsl

#endif

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file sdsl_concepts.hpp
    \brief Contains declarations and definitions of data structure concepts.
	\author Simon Gog
*/
#ifndef INCLUDED_SDSL_CONCEPTS
#define INCLUDED_SDSL_CONCEPTS

namespace sdsl {

struct bv_tag {
}; // bitvector tag
struct iv_tag {
}; // int_vector tag

struct csa_tag {
}; // compressed suffix array (CSAs) tag
struct cst_tag {
}; // compressed suffix tree (CST) tag
struct wt_tag {
}; // wavelet tree tag

struct psi_tag {
}; // tag for CSAs based on the psi function
struct lf_tag {
}; // tag for CSAs based on the LF function

struct csa_member_tag {
}; // tag for text, bwt, LF, \Psi members of CSA

struct lcp_tag {
};
struct lcp_plain_tag {
};
struct lcp_permuted_tag {
};
struct lcp_tree_compressed_tag {
};
struct lcp_tree_and_lf_compressed_tag {
};

struct alphabet_tag {
};
struct byte_alphabet_tag {
    static const uint8_t WIDTH = 8;
};
struct int_alphabet_tag {
    static const uint8_t WIDTH = 0;
};

struct sa_sampling_tag {
};
struct isa_sampling_tag {
};

template <class t_T, class t_r = void>
struct enable_if_type {
    typedef t_r type;
};

template <class t_idx, class t_enable = void>
struct index_tag {
    typedef t_enable type;
};

template <class t_idx>
struct index_tag<t_idx, typename enable_if_type<typename t_idx::index_category>::type> {
    using type = typename t_idx::index_category;
};

template <class t_sampling, class t_enable = void>
struct sampling_tag {
    typedef t_enable type;
};

template <class t_sampling>
struct sampling_tag<t_sampling,
                    typename enable_if_type<typename t_sampling::sampling_category>::type> {
    using type = typename t_sampling::sampling_category;
};

template <class t_enc_vec, class t_enable = void>
struct is_enc_vec {
    static constexpr bool value = false;
};

template <class t_enc_vec>
struct is_enc_vec<t_enc_vec, typename enable_if_type<typename t_enc_vec::enc_vec_type>::type> {
    static constexpr bool value = true;
};

template <class t_alphabet, class t_enable = void>
struct is_alphabet {
    static constexpr bool value = false;
};

template<class t_alphabet>
struct is_alphabet<t_alphabet, typename enable_if_type<typename t_alphabet::alphabet_category>::type> {
    static constexpr bool value = true;
};

} // end namespace sdsl

#endif

// Copyright (c) 2018, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file cereal.hpp
    \brief cereal.hpp offers cereal support
*/

#ifndef INCLUDED_SDSL_CEREAL
#define INCLUDED_SDSL_CEREAL

#include <type_traits>

#if defined(__has_include)
    #if __has_include(<cereal/cereal.hpp>)
#define SDSL_HAS_CEREAL 1
		#include <cereal/cereal.hpp>
		#include <cereal/details/traits.hpp>
		#include <cereal/archives/binary.hpp>
		#include <cereal/archives/json.hpp>
		#include <cereal/archives/portable_binary.hpp>
		#include <cereal/archives/xml.hpp>
		#include <cereal/types/memory.hpp>
		#include <cereal/types/vector.hpp>
    #endif
#endif

#ifndef SDSL_HAS_CEREAL
    #define SDSL_HAS_CEREAL 0

    #define CEREAL_NVP(X) X

    #define CEREAL_SERIALIZE_FUNCTION_NAME serialize
    #define CEREAL_LOAD_FUNCTION_NAME load
    #define CEREAL_SAVE_FUNCTION_NAME save
    #define CEREAL_LOAD_MINIMAL_FUNCTION_NAME load_minimal
    #define CEREAL_SAVE_MINIMAL_FUNCTION_NAME save_minimal

namespace cereal
{
namespace traits
{
template <typename t1, typename t2>
struct is_output_serializable
{
    using value = std::false_type;
};

template <typename t1, typename t2>
struct is_input_serializable
{
    using value = std::false_type;
};
}

template <typename t>
struct BinaryData {};

template <typename t1, typename t2>
void make_nvp(t1 const &, t2 const &) {}

template <typename t>
void make_size_tag(t const &) {}

template <typename t1, typename t2>
t1 binary_data(t1 const &, t2 const &) {}

}
#endif
#endif

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>

namespace sdsl {

int remove(const std::string&);

template <typename T>
void load_vector(std::vector<T>&, std::istream&);

template <typename T>
uint64_t serialize_vector(const std::vector<T>&,
                          std::ostream&,
                          sdsl::structure_tree_node* v = nullptr,
                          std::string				   = "");

// has_serialize<X>::value is true if class X has
// implement method serialize
// Adapted solution from jrok's proposal:
// http://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
template <typename X>
struct has_serialize {
    template <typename T>
    static constexpr auto check(T*) ->
    typename std::is_same<decltype(std::declval<T>().serialize(std::declval<std::ostream&>(),
                                                               std::declval<structure_tree_node*>(),
                                                               std::declval<std::string>())),
                          typename T::size_type>::type
    {
        return std::true_type();
    }
    template <typename>
    static constexpr std::false_type check(...)
    {
        return std::false_type();
    }
    typedef decltype(check<X>(nullptr)) type;
    static constexpr bool				value = type::value;
};

// has_load<X>::value is true if class X has
// implement method load
template <typename X>
struct has_load {
    template <typename T>
    static constexpr auto check(T*) ->
    typename std::is_same<decltype(std::declval<T>().load(std::declval<std::istream&>())),
                          void>::type
    {
        return std::true_type();
    }
    template <typename>
    static constexpr std::false_type check(...)
    {
        return std::false_type();
    }
    typedef decltype(check<X>(nullptr)) type;
    static constexpr bool				value = type::value;
};

// Writes primitive-typed variable t to stream out
template <typename T>
size_t write_member(const T&				   t,
                    std::ostream&			   out,
                    sdsl::structure_tree_node* v	= nullptr,
                    std::string				   name = "")
{
    sdsl::structure_tree_node* child =
        sdsl::structure_tree::add_child(v, name, util::class_name(t));
    out.write((char*)&t, sizeof(t));
    size_t written_bytes = sizeof(t);
    sdsl::structure_tree::add_size(child, written_bytes);
    return written_bytes;
}

// Specialization for std::string
template <>
inline size_t write_member<std::string>(const std::string&			t,
                                        std::ostream&				out,
                                        sdsl::structure_tree_node* v,
                                        std::string				name)
{
    structure_tree_node* child		   = structure_tree::add_child(v, name, util::class_name(t));
    size_t				 written_bytes = 0;
    written_bytes += write_member(t.size(), out, child, "length");
    out.write(t.c_str(), t.size());
    written_bytes += t.size();
    structure_tree::add_size(v, written_bytes);
    return written_bytes;
}

// Writes primitive-typed variable t to stream out
template <typename T>
void read_member(T& t, std::istream& in)
{
    in.read((char*)&t, sizeof(t));
}

// Specialization for std::string
template <>
inline void read_member<std::string>(std::string& t, std::istream& in)
{
    std::string::size_type size;
    read_member(size, in);
    char* buf = new char[size];
    in.read(buf, size);
    std::string temp(buf, size);
    delete[] buf;
    t = std::move(temp);
}

template <typename X>
typename std::enable_if<has_serialize<X>::value, typename X::size_type>::type
serialize(const X& x, std::ostream& out, structure_tree_node* v = nullptr, std::string name = "")
{
    return x.serialize(out, v, name);
}

template <typename X>
typename std::enable_if<std::is_pod<X>::value, uint64_t>::type
serialize(const X& x, std::ostream& out, structure_tree_node* v = nullptr, std::string name = "")
{
    return write_member(x, out, v, name);
}

template <typename X>
uint64_t serialize(const std::vector<X>& x,
                   std::ostream&		 out,
                   structure_tree_node*  v	= nullptr,
                   std::string			 name = "")
{

    return serialize(x.size(), out, v, name) + serialize_vector(x, out, v, name);
}

template <typename X>
typename std::enable_if<has_load<X>::value, void>::type load(X& x, std::istream& in)
{
    x.load(in);
}

template <typename X>
typename std::enable_if<std::is_pod<X>::value, void>::type load(X& x, std::istream& in)
{
    read_member(x, in);
}

template <typename X>
void load(std::vector<X>& x, std::istream& in)
{
    typename std::vector<X>::size_type size;
    load(size, in);
    x.resize(size);
    load_vector(x, in);
}

//! Load sdsl-object v from a file.
/*!
 * \param v sdsl-
 * \param file Name of the serialized file.
 */
template <typename T>
bool load_from_file(T& v, const std::string& file);

//! Load an int_vector from a plain array of `num_bytes`-byte integers with X in \{0, 1,2,4,8\} from disk.
// TODO: Remove ENDIAN dependency.
template <typename t_int_vec>
bool load_vector_from_file(t_int_vec&		  v,
                           const std::string& file,
                           uint8_t			  num_bytes		= 1,
                           uint8_t			  max_int_width = 64)
{
    if ((uint8_t)0 == num_bytes) { // if byte size is variable read int_vector<0> from file
        return load_from_file(v, file);
    } else if (num_bytes == 'd') {
        uint64_t  x = 0, max_x = 0;
        isfstream in(file, std::ios::in | std::ios::binary);
        if (!in) {
            return false;
        } else {
            std::vector<uint64_t> tmp;
            while (in >> x) {
                tmp.push_back(x);
                max_x = std::max(x, max_x);
            }
            v.width(bits::hi(max_x) + 1);
            v.resize(tmp.size());
            for (size_t i = 0; i < tmp.size(); ++i) {
                v[i] = tmp[i];
            }
            return true;
        }
    } else {
        off_t file_size = util::file_size(file);
        if (file_size == 0) {
            v.resize(0);
            return true;
        }
        if (file_size % num_bytes != 0) {
            throw std::logic_error("file size " + util::to_string(file_size) + " of \"" + file +
                "\" is not a multiple of " + util::to_string(num_bytes));
            return false;
        }
        isfstream in(file, std::ios::in | std::ios::binary);
        if (in) {
            v.width(std::min((int)8 * num_bytes, (int)max_int_width));
            v.resize(file_size / num_bytes);
            if (8 == t_int_vec::fixed_int_width and
                1 == num_bytes) { // if int_vector<8> is created from byte alphabet file
                in.read((char*)v.data(), file_size);
            } else {
                size_t				 idx		= 0;
                const size_t		 block_size = conf::SDSL_BLOCK_SIZE * num_bytes;
                std::vector<uint8_t> buf(block_size);
                // TODO: check for larger alphabets with num_bytes*8 = v::fixed_int_width

                uint64_t x		  = 0; // value
                uint8_t  cur_byte = 0;
                do {
                    in.read((char*)buf.data(), block_size);
                    size_t   read  = in.gcount();
                    uint8_t* begin = buf.data();
                    uint8_t* end   = begin + read;
                    while (begin < end) {
                        x |= ((uint64_t)(*begin)) << (cur_byte * 8);
                        ++cur_byte;
                        if (cur_byte == num_bytes) {
                            v[idx++] = x;
                            cur_byte = 0;
                            x		 = 0ULL;
                        }
                        ++begin;
                    }
                } while (idx < v.size());
                in.close();
            }
            return true;
        } else {
            return false;
        }
    }
}

//! Store a data structure to a file.
/*! The data structure has to provide a serialize function.
 *  \param v Data structure to store.
 *  \param file Name of the file where to store the data structure.
 *  \param Return if the data structure was stored successfully
 */
template <typename T>
bool store_to_file(const T& v, const std::string& file);

//! Specialization of store_to_file for a char array
inline bool store_to_file(const char* v, const std::string& file)
{
    osfstream out(file, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        if (util::verbose) {
            std::cerr << "ERROR: store_to_file(const char *v, const std::string&)" << std::endl;
            return false;
        }
    }
    uint64_t n = strlen((const char*)v);
    out.write(v, n);
    out.close();
    return true;
}

//! Specialization of store_to_file for int_vector
template <uint8_t t_width>
bool store_to_file(const int_vector<t_width>& v, const std::string& file);

//! Store an int_vector as plain int_type array to disk
template <typename int_type, typename t_int_vec>
bool store_to_plain_array(t_int_vec& v, const std::string& file)
{
    osfstream out(file, std::ios::out | std::ios::binary);
    if (out) {
        for (typename t_int_vec::size_type i = 0; i < v.size(); ++i) {
            int_type x = v[i];
            out.write((char*)&x, sizeof(int_type));
        }
        return true;
    } else {
        return false;
    }
}

template <typename T>
size_t serialize_empty_object(std::ostream&,
                              structure_tree_node* v	= nullptr,
                              std::string		   name = "",
                              const T*			   t	= nullptr)
{
    structure_tree_node* child		   = structure_tree::add_child(v, name, util::class_name(*t));
    size_t				 written_bytes = 0;
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}

//! Get the size of a data structure in bytes.
/*!
 *  \param v A reference to the data structure for which the size in bytes should be calculated.
 */
template <typename T>
typename T::size_type size_in_bytes(const T& t);

//! Get the size of a data structure in mega bytes (MiB).
/*!
 *  \param t A reference to the data structure for which the size in bytes should be calculated.
 */
template <typename T>
double size_in_mega_bytes(const T& t);

struct nullstream : std::ostream {
    struct nullbuf : std::streambuf {
        int overflow(int c) { return traits_type::not_eof(c); }
        int				 xputc(int) { return 0; }
        std::streamsize xsputn(char const*, std::streamsize n) { return n; }
        int sync() { return 0; }
    } m_sbuf;
    nullstream() : std::ios(&m_sbuf), std::ostream(&m_sbuf), m_sbuf() {}
};

//! Serialize each element of an std::vector
/*!
 * \param vec The vector which should be serialized.
 * \param out Output stream to which should be written.
 * \param v   Structure tree node. Note: If all elements have the same
 *            structure, then it is tried to combine all elements (i.e.
 *            make one node w with size set to the cumulative sum of all
 *           sizes of the children)
 */
template <typename T>
uint64_t serialize_vector(const std::vector<T>&		 vec,
                          std::ostream&				 out,
                          sdsl::structure_tree_node* v,
                          std::string				 name)
{
    if (vec.size() > 0) {
        sdsl::structure_tree_node* child =
            sdsl::structure_tree::add_child(v, name, "std::vector<" + util::class_name(vec[0]) + ">");
        size_t written_bytes = 0;
        for (const auto& x : vec) {
            written_bytes += serialize(x, out, child, "[]");
        }
        structure_tree::add_size(child, written_bytes);
        return written_bytes;
    } else {
        return 0;
    }
}

//! Load all elements of a vector from a input stream
/*! \param vec  Vector whose elements should be loaded.
 *  \param in   Input stream.
 *  \par Note
 *   The vector has to be resized prior the loading
 *   of its elements.
 */
template <typename T>
void load_vector(std::vector<T>& vec, std::istream& in)
{
    for (typename std::vector<T>::size_type i = 0; i < vec.size(); ++i) {
        load(vec[i], in);
    }
}

template <format_type F, typename X>
void write_structure(const X& x, std::ostream& out)
{
    std::unique_ptr<structure_tree_node> st_node(new structure_tree_node("name", "type"));
    nullstream							 ns;
    serialize(x, ns, st_node.get(), "");
    if (st_node.get()->children.size() > 0) {
        for (const auto& child : st_node.get()->children) {
            sdsl::write_structure_tree<F>(child.second.get(), out);
        }
    }
}

template <format_type F, typename X>
void write_structure(const X& x, std::string file)
{
    std::ofstream out(file);
    write_structure<F>(x, out);
}

template <format_type F, typename... Xs>
void write_structure(std::ostream& out, Xs... xs)
{
    typedef std::unique_ptr<structure_tree_node> up_stn_type;
    up_stn_type									 st_node(new structure_tree_node("name", "type"));
    _write_structure(st_node, xs...);
    sdsl::write_structure_tree<F>(st_node.get(), out);
}

template <typename X, typename... Xs>
void _write_structure(std::unique_ptr<structure_tree_node>& st_node, X x, Xs... xs)
{
    nullstream ns;
    serialize(x, ns, st_node.get(), "");
    _write_structure(st_node, xs...);
}

inline void _write_structure(std::unique_ptr<structure_tree_node>&) {}

//! Internal function used by csXprintf
inline uint64_t _parse_number(std::string::const_iterator& c, const std::string::const_iterator& end)
{
    std::string::const_iterator s = c;
    while (c != end and isdigit(*c))
        ++c;
    if (c > s) {
        return std::stoull(std::string(s, c));
    } else {
        return 0;
    }
}

//! Internal function used by csXprintf
template <typename t_csa>
const t_csa& _idx_csa(const t_csa& t, csa_tag)
{
    return t;
}

//! Internal function used by csXprintf
template <typename t_cst>
const typename t_cst::csa_type& _idx_csa(const t_cst& t, cst_tag)
{
    return t.csa;
}

//! Internal function used by csXprintf
template <typename t_csa>
std::string _idx_lcp_val(const t_csa&, uint64_t, uint64_t, csa_tag)
{
    return "";
}

//! Internal function used by csXprintf
template <typename t_cst>
std::string _idx_lcp_val(const t_cst& t, uint64_t i, uint64_t w, cst_tag)
{
    return util::to_string(t.lcp[i], w);
}

template <typename t_csx, typename t_alph = typename t_csx::alphabet_category>
struct default_sentinel {
    static const char value = '$';
};

template <typename t_csx>
struct default_sentinel<t_csx, byte_alphabet_tag> {
    static const char value = '$';
};

template <typename t_csx>
struct default_sentinel<t_csx, int_alphabet_tag> {
    static const char value = '0';
};

//! Prints members of CSAs and CSTs
/*! This is a printf like method to write members of CSAs and CSTs into an outstream.
 * \tparam t_idx   Type of the index. Class should be of concept csa_tag or cst_tag.
 * \param out      Output stream.
 * \param format   Format string. See explanation below.
 * \param idx      CSA or CST object.
 * \param sentinel Character which should replace the 0-symbol in BWT/ TEXT.
 *
 * \par Format string
 *   Each line of the output will be formatted according to the format string.
 *   All content, except tokens which start with `%` will be copied. Tokens
 *   which start with `%` will be replaced as follows (let w be a positive
 *    number. setw(w) is used to format single numbers):
 *
 *      Token      |  Replacement | Comment
 *      -----------------------------------------------------------------------
 *       %[w]I     | Row index i.                           |
 *       %[w]S     | SA[i]                                  |
 *       %[w]s     | ISA[i]                                 |
 *       %[w]P     | PSI[i]                                 |
 *       %[w]p     | LF[i]                                  |
 *       %[w]L     | LCP[i]                                 | only for CSTs
 *       %[w]B     | BWT[i]                                 |
 *       %[w[:W]]T | Print min(idx.size(),w) chars of each  |
 *                 | suffix, each char formatted by setw(W).|
 *       %%        | %                                      |
 */
template <typename t_idx>
void csXprintf(std::ostream&	  out,
               const std::string& format,
               const t_idx&		  idx,
               char				  sentinel = default_sentinel<t_idx>::value)
{
    typename t_idx::index_category  cat;
    const typename t_idx::csa_type& csa = _idx_csa(idx, cat);
    std::vector<std::string>		res(csa.size());
    bool truncate = false;
    for (std::string::const_iterator c = format.begin(), s = c; c != format.end(); s = c) {
        while (c != format.end() and *c != '%')
            ++c;	 // string before the next `%`
        if (c > s) { // copy format string part
            std::vector<std::string> to_copy(csa.size(), std::string(s, c));
            transform(
                res.begin(), res.end(), to_copy.begin(), res.begin(), std::plus<std::string>());
        }
        if (c == format.end()) break;
        ++c;										 // skip `%`
        uint64_t w = _parse_number(c, format.end()); // element width
        if (c == format.end()) break;
        uint64_t W = 0; // character width
        if (':' == *c) {
            ++c;
            W = _parse_number(c, format.end());
        }
        if (c == format.end()) break;
        for (uint64_t i = 0; i < csa.size(); ++i) {
            switch (*c) {
                case 'I':
                    res[i] += util::to_string(i, w);
                    break;
                case 'S':
                    res[i] += util::to_string(csa[i], w);
                    break;
                case 's':
                    res[i] += util::to_string(csa.isa[i], w);
                    break;
                case 'P':
                    res[i] += util::to_string(csa.psi[i], w);
                    break;
                case 'p':
                    res[i] += util::to_string(csa.lf[i], w);
                    break;
                case 'L':
                    res[i] += _idx_lcp_val(idx, i, w, cat);
                    break;
                case 'B':
                    if (0 == csa.bwt[i]) {
                        res[i] += util::to_string(sentinel, w);
                    } else {
                        res[i] += util::to_string(csa.bwt[i], w);
                    }
                    break;
                case 'U':
                    truncate = true;
                    SDSL_FALLTHROUGH
                case 'T':
                    for (uint64_t k = 0; (w > 0 and k < w) or (0 == w and k < csa.size()); ++k) {
                        if (0 == csa.text[(csa[i] + k) % csa.size()]) {
                            res[i] += util::to_string(sentinel, W);
                            if ( truncate ) {
                                truncate = false;
                                break;
                            }
                        } else {
                            res[i] += util::to_string(csa.text[(csa[i] + k) % csa.size()], W);
                        }
                    }
                    break;
                case 'u':
                    truncate = true;
                    SDSL_FALLTHROUGH
                case 't':
                    for (uint64_t k = 0; (w > 0 and k < w) or (0 == w and k < csa.size()); ++k) {
                        if (0 == csa.text[(i + k) % csa.size()]) {
                            res[i] += util::to_string(sentinel, W);
                            if ( truncate ) {
                                truncate = false;
                                break;
                            }
                        } else {
                            res[i] += util::to_string(csa.text[(i + k) % csa.size()], W);
                        }
                    }
                    break;
                case '%':
                    res[i] += "%";
                    break;
            }
        }
        ++c;
    }
    for (size_t i = 0; i < res.size(); ++i)
        out << res[i] << std::endl;
}

//! Returns the file name of the resource.
/*!
 * \param  key        Resource key.
 * \param  config    Cache configuration.
 * \return The file name of the resource.
 */
inline std::string cache_file_name(const std::string& key, const cache_config& config)
{
    if (config.file_map.count(key) != 0) {
        return config.file_map.at(key);
    }
    return config.dir + "/" + key + "_" + config.id + ".sdsl";
}

//! Returns the file name of the resource.
/*!
 * \param  key        Resource key.
 * \param  config    Cache configuration.
 * \return The file name of the resource.
 */
template <typename T>
std::string cache_file_name(const std::string& key, const cache_config& config)
{
    return cache_file_name(key + "_" + util::class_to_hash(T()), config);
}

//! Register the existing resource specified by the key to the cache
/*!
 *  \param key        Resource key.
 *  \param config    Cache configuration.
 *
 *  Note: If the resource does not exist under the given key,
 *  it will be not added to the cache configuration.
 */
inline void register_cache_file(const std::string& key, cache_config& config)
{
    std::string file_name = cache_file_name(key, config);
    isfstream   in(file_name);
    if (in) { // if file exists, register it.
        config.file_map[key] = file_name;
    }
}

//! Checks if the resource specified by the key exists in the cache.
/*!
  \param key    Resource key.
  \param config Cache configuration.
  \return True, if the file exists, false otherwise.
*/
inline bool cache_file_exists(const std::string& key, const cache_config& config)
{
    std::string file_name = cache_file_name(key, config);
    isfstream   in(file_name);
    if (in) {
        in.close();
        return true;
    }
    return false;
}

//! Checks if the resource specified by the key and type exists in the cache.
/*!
  \tparam T     Type.
  \param key    Resource key.
  \param config Cache configuration.
  \return True, if the file exists, false otherwise.
*/
template <typename T>
bool cache_file_exists(const std::string& key, const cache_config& config)
{
    return cache_file_exists(key + "_" + util::class_to_hash(T()), config);
}

//! Returns a name for a temporary file. I.e. the name was not used before.
inline std::string tmp_file(const cache_config& config, std::string name_part = "")
{
    return config.dir + "/" + util::to_string(util::pid()) + "_" + util::to_string(util::id()) +
        name_part + ".sdsl";
}

//! Returns a name for a temporary file. I.e. the name was not used before.
inline std::string tmp_file(const std::string& filename, std::string name_part = "")
{
    return util::dirname(filename) + "/" + util::to_string(util::pid()) + "_" +
        util::to_string(util::id()) + name_part + ".sdsl";
}

template <typename T>
bool load_from_cache(T&					 v,
                     const std::string&  key,
                     const cache_config& config,
                     bool				 add_type_hash = false)
{
    std::string file;
    if (add_type_hash) {
        file = cache_file_name<T>(key, config);
    } else {
        file = cache_file_name(key, config);
    }
    if (load_from_file(v, file)) {
        if (util::verbose) {
            std::cerr << "Load `" << file << std::endl;
        }
        return true;
    } else {
        std::cerr << "WARNING: Could not load file '";
        std::cerr << file << "'" << std::endl;
        return false;
    }
}

//! Stores the object v as a resource in the cache.
/*!
 *  \param
 */
template <typename T>
bool store_to_cache(const T&		   v,
                    const std::string& key,
                    cache_config&	  config,
                    bool			   add_type_hash = false)
{
    std::string file;
    if (add_type_hash) {
        file = cache_file_name<T>(key, config);
    } else {
        file = cache_file_name(key, config);
    }
    if (store_to_file(v, file)) {
        config.file_map[std::string(key)] = file;
        return true;
    } else {
        std::cerr << "WARNING: store_to_cache: could not store file `" << file << "`" << std::endl;
        return false;
    }
}

template <typename T>
bool remove_from_cache(const std::string& key, cache_config& config, bool add_type_hash = false)
{
    std::string file;
    if (add_type_hash) {
        file = cache_file_name<T>(key, config);
    } else {
        file = cache_file_name(key, config);
    }
    config.file_map.erase(key);
    if (sdsl::remove(file) == 0) {
        return true;
    } else {
        std::cerr << "WARNING: delete_from_cache: could not delete file `" << file << "`"
                  << std::endl;
        return false;
    }
}

//==================== Template functions ====================

template <typename T>
typename T::size_type size_in_bytes(const T& t)
{
    nullstream ns;
    return serialize(t, ns);
}

template <typename T>
double size_in_mega_bytes(const T& t)
{
    return size_in_bytes(t) / (1024.0 * 1024.0);
}

template <typename T>
void add_hash(const T& t, std::ostream& out)
{
    uint64_t hash_value = util::hashvalue_of_classname(t);
    write_member(hash_value, out);
}

template <typename T>
bool store_to_file(const T& t, const std::string& file)
{
    osfstream out(file, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        if (util::verbose) {
            std::cerr << "ERROR: store_to_file not successful for: `" << file << "`" << std::endl;
        }
        return false;
    }
    serialize(t, out);
    out.close();
    if (util::verbose) {
        std::cerr << "INFO: store_to_file: `" << file << "`" << std::endl;
    }
    return true;
}

template <typename T>
bool store_to_checked_file(const T& t, const std::string& file)
{
    std::string checkfile = file + "_check";
    osfstream   out(checkfile, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        if (util::verbose) {
            std::cerr << "ERROR: store_to_checked_file not successful for: `" << checkfile << "`"
                      << std::endl;
        }
        return false;
    }
    add_hash(t, out);
    out.close();
    return store_to_file(t, file);
}

inline bool store_to_checked_file(const char* v, const std::string& file)
{
    std::string checkfile = file + "_check";
    osfstream   out(checkfile, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        if (util::verbose) {
            std::cerr << "ERROR: store_to_checked_file(const char *v, const std::string&)"
                      << std::endl;
            return false;
        }
    }
    add_hash(v, out);
    out.close();
    return store_to_file(v, file);
}

inline bool store_to_file(const std::string& v, const std::string& file)
{
    osfstream out(file, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        if (util::verbose) {
            std::cerr << "ERROR: store_to_file(const std::string& v, const std::string&)"
                      << std::endl;
            return false;
        }
    }
    out.write(v.data(), v.size());
    out.close();
    return true;
}

template <uint8_t t_width>
bool store_to_file(const int_vector<t_width>& v, const std::string& file)
{
    osfstream out(file, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        std::cerr << "ERROR: util::store_to_file:: Could not open file `" << file << "`"
                  << std::endl;
        return false;
    } else {
        if (util::verbose) {
            std::cerr << "INFO: store_to_file: `" << file << "`" << std::endl;
        }
    }
    v.serialize(out, nullptr, "");
    out.close();
    return true;
}

template <uint8_t t_width>
bool store_to_checked_file(const int_vector<t_width>& v, const std::string& file)
{
    std::string checkfile = file + "_check";
    osfstream   out(checkfile, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!out) {
        std::cerr << "ERROR: util::store_to_checked_file: Could not open check file `" << checkfile
                  << "`" << std::endl;
        return false;
    } else {
        if (util::verbose) {
            std::cerr << "INFO: store_to_checked_file: `" << checkfile << "`" << std::endl;
        }
    }
    add_hash(v, out);
    out.close();
    return store_to_file(v, file);
}

template <typename T>
bool load_from_file(T& v, const std::string& file)
{
    isfstream in(file, std::ios::binary | std::ios::in);
    if (!in) {
        if (util::verbose) {
            std::cerr << "Could not load file `" << file << "`" << std::endl;
        }
        return false;
    }
    load(v, in);
    in.close();
    if (util::verbose) {
        std::cerr << "Load file `" << file << "`" << std::endl;
    }
    return true;
}

template <typename T>
bool load_from_checked_file(T& v, const std::string& file)
{
    isfstream in(file + "_check", std::ios::binary | std::ios::in);
    if (!in) {
        if (util::verbose) {
            std::cerr << "Could not load check file `" << file << "_check`" << std::endl;
        }
        return false;
    }
    uint64_t hash_value;
    read_member(hash_value, in);
    if (hash_value != util::hashvalue_of_classname(v)) {
        if (util::verbose) {
            std::cerr << "File `" << file << "` is not an instance of the class `"
                      << sdsl::util::demangle2(typeid(T).name()) << "`" << std::endl;
        }
        return false;
    }
    return load_from_file(v, file);
}

template <typename t_iv>
inline typename std::enable_if<std::is_same<typename t_iv::index_category, iv_tag>::value or
    std::is_same<typename t_iv::index_category, csa_tag>::value or
    std::is_same<typename t_iv::index_category, lcp_tag>::value,
                               std::ostream&>::type
operator<<(std::ostream& os, const t_iv& v)
{
    for (auto it = v.begin(), end = v.end(); it != end; ++it) {
        os << *it;
        if (it + 1 != end) os << " ";
    }
    return os;
}

template <typename t_iv>
inline typename std::enable_if<std::is_same<typename t_iv::index_category, wt_tag>::value,
                               std::ostream&>::type
operator<<(std::ostream& os, const t_iv& v)
{
    for (auto it = v.begin(), end = v.end(); it != end; ++it) {
        os << *it;
        if (it + 1 != end and
            std::is_same<typename t_iv::alphabet_category, int_alphabet_tag>::value)
            os << " ";
    }
    return os;
}

template <typename t_int>
inline typename std::enable_if<std::is_integral<t_int>::value, std::ostream&>::type
operator<<(std::ostream& os, const std::vector<t_int>& v)
{
    for (auto it = v.begin(), end = v.end(); it != end; ++it) {
        os << *it;
        if (it + 1 != end) os << " ";
    }
    return os;
}

template <typename t_iv>
inline typename std::enable_if<std::is_same<typename t_iv::category, csa_member_tag>::value,
                               std::ostream&>::type
operator<<(std::ostream& os, const t_iv& v)
{
    for (auto it = v.begin(), end = v.end(); it != end; ++it) {
        os << *it;
        if (it + 1 != end and
            std::is_same<typename t_iv::alphabet_category, int_alphabet_tag>::value)
            os << " ";
    }
    return os;
}
}
#endif

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*!\file memory_management.hpp
\brief memory_management.hpp contains two function for allocating and deallocating memory
\author Simon Gog
*/
#ifndef INCLUDED_SDSL_MEMORY_MANAGEMENT
#define INCLUDED_SDSL_MEMORY_MANAGEMENT

#include <chrono>
#include <algorithm>

namespace sdsl {

inline void output_event_json(std::ostream& out,const mm_event& ev,const tracker_storage& m)
{
    using namespace std::chrono;
    out << "\t\t" << "\"name\" : " << "\"" << ev.name << "\",\n";
    out << "\t\t" << "\"usage\" : [" << "\n";
    for (size_t j=0; j<ev.allocations.size(); j++)  {
        out << "\t\t\t[" << duration_cast<milliseconds>(ev.allocations[j].timestamp-m.start_log).count()
            << "," << ev.allocations[j].usage << "]";
        if (j+1<ev.allocations.size()) {
            out << ",\n";
        } else {
            out << "\n";
        }
    }
    out << "\t\t" << "]\n";
}

template<>
inline void write_mem_log<JSON_FORMAT>(std::ostream& out,const tracker_storage& m)
{
    auto events = m.completed_events;
    std::sort(events.begin(),events.end());

    // output
    out << "[\n";
    for (size_t i=0; i<events.size(); i++) {
        out << "\t{\n";
        output_event_json(out,events[i],m);
        if (i<events.size()-1) {
            out << "\t},\n";
        } else {
            out << "\t}\n";
        }
    }
    out << "]\n";
}

inline std::string create_mem_html_header()
{
    std::stringstream jsonheader;
    jsonheader << "<html>\n"
               << "<head>\n"
               << "<meta charset=\"utf-8\">\n"
               << "<style>\n"
               << "    body { font: 11px sans-serif; }\n"
               << "    .rule { height: 90%; position: absolute; border-right: 1px dotted #000; "
                  "text-align: right; }\n"
               << "</style>\n"
               << "<title>sdsl memory usage visualization</title>\n"
               << "<script src=\"http://d3js.org/d3.v3.js\"></script>\n"
               << "</head>\n"
               << "<body marginwidth=\"0\" marginheight=\"0\">\n"
               << "<button><a id=\"download\">Save as SVG</a></button>\n"
               << "<div class=\"chart\"><div id=\"visualization\"></div></div><script>\n";
    return jsonheader.str();
}

inline std::string create_mem_js_body(const std::string& jsonObject)
{
    std::stringstream jsonbody;
    jsonbody
        << "var events = " << jsonObject << ";\n"
        << "var w = window,d = document,e = d.documentElement,g = d.getElementsByTagName('body')[0],\n"
        << "  xw = w.innerWidth || e.clientWidth || g.clientWidth,\n"
        << "  yh = w.innerHeight || e.clientHeight || g.clientHeight;\n\n"
        << "var margin = {top: 20,right: 80,bottom: 120,left: 120},\n"
        << "  width = xw - margin.left - margin.right,height = yh - margin.top - margin.bottom;\n"
        << "var x = d3.scale.linear().range([0, width]);\n"
        << "var y = d3.scale.linear().range([height, 0]);\n"
        << "var xAxis = d3.svg.axis().scale(x).orient(\"bottom\");\n"
        << "var yAxis = d3.svg.axis().scale(y).orient(\"left\").ticks(5);\n"
        << "var color = d3.scale.category10();\n"
        << "var x_max = d3.max(events, function (d) { return d3.max(d.usage, function (u) { return u[0] / 1000;})})\n"
        << "var y_max = d3.max(events, function (d) { return d3.max(d.usage, function (u) { return 1.1 * u[1] / (1024 * 1024);})})\n"
        << "var peak = d3.max(events, function (d) { return d3.max(d.usage, function (u) { return u[1]; })})\n"
        << "var data = []\nevents.forEach(function (d) { data = data.concat(d.usage); });\n"
        << "var peakelem = data.filter(function (a) { return a[1] == peak; });\n"
        << "var peakelem = peakelem.splice(0,1);\n"
        << "x.domain([0, x_max]);\n y.domain([0, y_max]);\n"
        << "var svg = d3.select(\"#visualization\").append(\"svg\")\n"
        << "  .attr(\"width\", width + margin.left + margin.right)\n"
        << "  .attr(\"height\", height + margin.top + margin.bottom)\n"
        << "  .attr(\"xmlns\", \"http://www.w3.org/2000/svg\")\n"
        << "  .append(\"g\").attr(\"transform\",\"translate(\" + margin.left + \",\" + margin.top + \")\");\n\n"
        << "  svg.append(\"g\").attr(\"class\", \"xaxis\").attr(\"transform\", \"translate(0,\" + height + \")\")\n"
        << "  .call(xAxis).append(\"text\").attr(\"text-anchor\", \"end\")\n"
        << "  .attr(\"shape-rendering\", \"crispEdges\").attr(\"x\", width / 2 + 50).attr(\"y\", 70).attr(\"shape-rendering\", \"crispEdges\")\n"
        << "  .attr(\"font-family\", \"sans-serif\").attr(\"font-size\", \"20px\").text(\"Time (seconds)\");\n\n"
        << "svg.append(\"g\").attr(\"class\", \"yaxis\").call(yAxis).append(\"text\").attr(\"transform\", \"rotate(-90)\").attr(\"x\", -height / 2 + 50)\n"
        << "  .attr(\"y\", -80).attr(\"shape-rendering\", \"crispEdges\").attr(\"font-family\", \"sans-serif\").attr(\"font-size\", \"20px\").style(\"text-anchor\", \"end\")\n"
        << "  .text(\"Memory Usage (MiB)\");\n\n"
        << "svg.selectAll(\".tick text\").style(\"font-size\", \"20px\");\n"
        << "svg.selectAll(\".xaxis .tick text\").attr(\"dy\", 23);\nsvg.selectAll(\".yaxis .tick text\").attr(\"dx\", -10);\n"
        << "svg.selectAll(\"line\").attr(\"fill\", \"none\").attr(\"stroke\", \"black\")\nsvg.selectAll(\"path\").attr(\"fill\", \"none\").attr(\"stroke\", \"black\")\n\n"
        << "svg.selectAll(\"line.horizontalGrid\").data(y.ticks(5)).enter().append(\"line\")\n"
        << "  .attr({\"class\": \"horizontalGrid\",\"x1\": 0,\"x2\": width,\"y1\": function (d) { return y(d);},\n"
        << "     \"y2\": function (d) { return y(d); }, \"fill\": \"none\", \"shape-rendering\": \"crispEdges\",\n"
        << "     \"stroke\": \"lightgrey\",\"stroke-dasharray\": \"10,10\",\"stroke-width\": \"1.5px\"});\n\n"
        << "var area = d3.svg.area().x(function (d) { return x(d[0] / 1000);}).y0(height).y1(function (d) { return y(d[1] / (1024 * 1024))});\n\n"
        << "var ev = svg.selectAll(\".event\").data(events).enter().append(\"svg:path\").attr(\"class\", \"area\")\n"
        << "  .attr(\"fill\", function (d) { return d3.rgb(color(d.name)); })\n"
        << "  .attr(\"d\", function (d) { return area(d.usage) })\n"
        << "  .style(\"stroke\", function (d) { return d3.rgb(color(d.name)).darker(2);}).style(\"stroke-width\", \"2px\")\n\n"
        << "svg.selectAll(\".dot\").data(peakelem).enter().append(\"circle\").attr(\"r\", 3).attr(\"fill\", \"red\")\n"
        << "  .attr(\"cx\", function (d) {return x(d[0] / 1000)})\n"
        << "  .attr(\"cy\", function (d) {return y(d[1] / (1024 * 1024))})\n"
        << "  .attr(\"fill\", \"red\").attr(\"stroke-width\", 2).attr(\"stroke\", \"#cc0000\")\n\n"
        << "svg.selectAll(\".dot\").data(peakelem).enter().append(\"svg:text\")\n"
        << "  .attr(\"x\", function (d) {return x(d[0] / 1000)}).attr(\"y\", function (d) {return y(d[1] / (1024 * 1024) * 1.025)})\n"
        << "  .text(function (d) {return \"Peak Usage: \" + Math.round(d[1] / (1024 * 1024)) + \" MB\"})\n"
        << "  .attr(\"font-size\", 12).attr(\"fill\", \"red\");\n\n"
        << "svg.selectAll(\".dot\").data(peakelem).enter().append(\"circle\")\n"
        << "  .attr(\"r\", 5).attr(\"fill\", \"red\")\n"
        << "  .attr(\"cx\", function (d) {return x(d[0] / 1000)})\n"
        << "  .attr(\"cy\", function (d) {return y(d[1] / (1024 * 1024))})\n"
        << "  .attr(\"fill\", \"none\").attr(\"stroke-width\", 2).attr(\"stroke\", \"#cc0000\").each(pulsepeak());\n\n"
        << "function pulsepeak() { return function (d, i, j) {\n"
        << "  d3.select(this).attr(\"r\", 5).style(\"stroke-opacity\", 1.0).transition()\n"
        << "    .ease(\"linear\").duration(1000).attr(\"r\", 10).style(\"stroke-opacity\", 0.0).each(\"end\", pulsepeak());};}\n\n"
        << "var vertical = d3.select(\".chart\").append(\"div\").attr(\"class\", \"remove\")\n"
        << "  .style(\"position\", \"absolute\").style(\"z-index\", \"19\").style(\"width\", \"1px\")\n"
        << "  .style(\"height\", height - margin).style(\"top\", \"30px\").style(\"bottom\", \"50px\")\n"
        << "  .style(\"left\", \"0px\").style(\"opacity\", \"0.4\").style(\"background\", \"black\");\n\n"
        << "var tooltip = d3.select(\".chart\").append(\"div\").attr(\"class\", \"remove\")\n"
        << "  .style(\"position\", \"absolute\").style(\"z-index\", \"20\").style(\"visibility\", \"hidden\").style(\"top\", \"10px\");\n\n"
        << "var circle = svg.append(\"circle\").attr(\"cx\", 100).attr(\"cy\", 350).attr(\"r\", 3).attr(\"fill\", \"black\").style(\"opacity\", \"0\")\n\n"
        << "d3.select(\"svg\").on(\"mousemove\", function () {\n"
        << "  mousex = d3.mouse(this);\n"
        << "  if (mousex[0] < margin.left + 3 || mousex[0] >= xw - margin.right) {\n"
        << "    vertical.style(\"opacity\", \"0\"); tooltip.style(\"opacity\", \"0\"); circle.style(\"opacity\", \"0\")\n"
        << "  } else {\n"
        << "    var xvalue = x.invert(mousex[0] - margin.left); var pos = findPosition(xvalue)\n"
        << "    vertical.style(\"opacity\", \"0.4\"); tooltip.style(\"opacity\", \"1\"); circle.style(\"opacity\", \"1\")\n"
        << "    circle.attr(\"cx\", pos.x).attr(\"cy\", pos.y); vertical.style(\"left\", mousex[0] + \"px\");tooltip.style(\"left\", mousex[0] + 15 + \"px\")\n"
        << "    tooltip.html(\"<p>\" + xvalue.toFixed(2) + \" Seconds <br>\" + Math.round(pos.mem) + \" MiB <br> \" + pos.name + "
        << "  \"<br> Phase Time: \" + pos.ptime + \" Seconds </p>\").style(\"visibility\", \"visible\");\n"
        << "  }\n})"
        << ".on(\"mouseover\", function () {\n"
        << "  mousex = d3.mouse(this);\n  if (mousex[0] < margin.left + 3 || mousex[0] > xw - margin.right) {\n"
        << "    vertical.style(\"opacity\", \"0\")\n  } else {\n    vertical.style(\"opacity\", \"0.4\");vertical.style(\"left\", mousex[0] + 7 + \"px\")\n}})\n"
        << "d3.select(\"#download\").on(\"click\", function () {\n"
        << "d3.select(this).attr(\"href\", 'data:application/octet-stream;base64,' + btoa(d3.select(\"#visualization\").html())).attr(\"download\", \"viz.svg\")})\n\n"
        << "function findPosition(e){correctArea=d3.selectAll(\".area\").filter(function(t){if(t.usage[0][0]<=e*1e3&&t.usage[t.usage.length-1][0]>=e*1e3){return true}"
        << "return false});if(correctArea.empty()){return 0}var t=new Array;correctArea[0].forEach(function(n){t.push(findYValueinArea(n,e))});"
        << "max_elem=d3.max(t,function(e){return e.mem});var n=t.filter(function(e){return e.mem==max_elem});return n[0]}"
        << "function findYValueinArea(e,t){len=e.getTotalLength();var n=0;var r=len;for(var i=0;i<=len;i+=50){var s=e.getPointAtLength(i);"
        << "var o=x.invert(s.x);var u=y.invert(s.y);if(u>0&&o>t){n=Math.max(0,i-50);r=i;break}}var a=e.getPointAtLength(0);"
        << "var f=1;while(n<r){var l=(r+n)/2;a=e.getPointAtLength(l);target_x=x.invert(a.x);if((l==n||l==r)&&Math.abs(target_x-t)>.01){break}if(target_x>t)r=l;"
        << "else if(target_x<t)n=l;else{break}if(f>50){break}f++}var c=new function(){this.mem=y.invert(a.y);this.name=e.__data__.name;"
        << "this.min=d3.min(e.__data__.usage,function(e){return e[0]/1e3});this.max=d3.max(e.__data__.usage,function(e){return e[0]/1e3});"
        << "this.ptime=Math.round(this.max-this.min);this.x=a.x;this.y=a.y};return c}\n</script></body></html>";
    return jsonbody.str();
}

template<>
inline void write_mem_log<HTML_FORMAT>(std::ostream& out,const tracker_storage& m)
{
    std::stringstream json_data;
    write_mem_log<JSON_FORMAT>(json_data,m);

    out << create_mem_html_header();
    out << create_mem_js_body(json_data.str());
}

#pragma pack(push, 1)
typedef struct mm_block {
    size_t			 size;
    struct mm_block* next;
    struct mm_block* prev;
} mm_block_t;

typedef struct bfoot {
    size_t size;
} mm_block_foot_t;
#pragma pack(pop)

#define ALIGNMENT             sizeof(uint64_t)
#define ALIGNSPLIT(size)      (((size)) & ~0x7)
#define ALIGN(size)           (((size) + (ALIGNMENT-1)) & ~0x7)
#define MM_BLOCK_OVERHEAD     (sizeof(size_t)+sizeof(size_t))
#define MIN_BLOCKSIZE         (ALIGN(sizeof(mm_block_t)+sizeof(mm_block_foot_t)))
#define UNMASK_SIZE(size)     ((size)&~1)
#define ISFREE(size)          ((size)&1)
#define SETFREE(size)         ((size)|1)
#define SPLIT_THRESHOLD       (MIN_BLOCKSIZE)

inline mm_block_t* block_cur(void* ptr)
{
    mm_block_t* bptr = (mm_block_t*)((uint8_t*)ptr - sizeof(size_t));
    return bptr;
}

/* given a block retrieve the previous block if any. nullptr otherwise */
inline mm_block_t* block_prev(mm_block_t* cur_bptr, mm_block_t* first)
{
    /* start of the heap? */
    if (cur_bptr == first) return nullptr;
    mm_block_foot_t* prev_foot = (mm_block_foot_t*)((uint8_t*)cur_bptr - sizeof(mm_block_foot_t));
    mm_block_t*		 prev_bptr = (mm_block_t*)((uint8_t*)cur_bptr - UNMASK_SIZE(prev_foot->size));
    return prev_bptr;
}

/* given a block retrieve the next block if any. nullptr otherwise */
inline mm_block_t* block_next(mm_block_t* cur_bptr, uint8_t* top)
{
    /* end of the heap? */
    if ((uint8_t*)((uint8_t*)cur_bptr + UNMASK_SIZE(cur_bptr->size)) >= top) return nullptr;

    mm_block_t* next_bptr = (mm_block_t*)((uint8_t*)cur_bptr + UNMASK_SIZE(cur_bptr->size));
    return next_bptr;
}

/* calculate the size of a memory block */
inline size_t block_size(void* ptr)
{
    mm_block_t* bptr = block_cur(ptr);
    return UNMASK_SIZE(bptr->size);
}

inline bool block_isfree(mm_block_t* ptr)
{
    return ((ptr->size) & 1ULL);
}

/* is the next block free */
inline bool block_nextfree(mm_block_t* ptr, uint8_t* top)
{
    mm_block_t* next = block_next(ptr, top);
    if (next && block_isfree(next)) return true;
    return false;
}

/* is the prev block free */
inline bool block_prevfree(mm_block_t* ptr, mm_block_t* begin)
{
    mm_block_t* prev = block_prev(ptr, begin);
    if (prev && block_isfree(prev)) return 1;
    return 0;
}

/* update the footer with a new size */
inline void foot_update(mm_block_t* ptr, size_t size)
{
    mm_block_foot_t* fptr =
        (mm_block_foot_t*)((uint8_t*)ptr + UNMASK_SIZE(size) - sizeof(mm_block_foot_t));
    fptr->size = size;
}

/* update the block with a new size */
inline void block_update(mm_block_t* ptr, size_t size)
{
    ptr->size = size;
    foot_update(ptr, size);
}

/* return the pointer to the "data" */
inline void* block_data(mm_block_t* ptr) { return (void*)((uint8_t*)ptr + sizeof(size_t)); }

/* return size of the data that can be stored in the block */
inline size_t block_getdatasize(mm_block_t* ptr)
{
    size_t blocksize = UNMASK_SIZE(ptr->size);
    return blocksize - sizeof(size_t) - sizeof(mm_block_foot_t);
}

/* mark the block as free */
inline void block_markfree(mm_block_t* ptr) { block_update(ptr, SETFREE(ptr->size)); }

/* mark the block as used */
inline void block_markused(mm_block_t* ptr) { block_update(ptr, UNMASK_SIZE(ptr->size)); }

#ifndef _WIN32

class hugepage_allocator {
private:
    uint8_t*	m_base		  = nullptr;
    mm_block_t* m_first_block = nullptr;
    uint8_t*	m_top		  = nullptr;
    size_t		m_total_size  = 0;
    std::multimap<size_t, mm_block_t*> m_free_large;

private:

    inline void block_print(int id, mm_block_t* bptr)
    {
        fprintf(stdout,
                "%d addr=%p size=%lu (%lu) free=%d\n",
                id,
                ((void*)bptr),
                UNMASK_SIZE(bptr->size),
                bptr->size,
                block_isfree(bptr));
        fflush(stdout);
    }

    inline uint64_t extract_number(std::string& line)
    {
        std::string num_str;
        for (size_t i = line.size() - 1; i + 1 >= 1; i--) {
            if (isdigit(line[i])) {
                num_str.insert(num_str.begin(), line[i]);
            } else {
                if (num_str.size() > 0) {
                    break;
                }
            }
        }
        return std::strtoull(num_str.c_str(), nullptr, 10);
    }

    inline uint64_t extract_multiplier(std::string& line)
    {
        uint64_t num = 1;
        if (line[line.size() - 2] == 'k' || line[line.size() - 2] == 'K') {
            num = 1024;
        }
        if (line[line.size() - 2] == 'm' || line[line.size() - 2] == 'M') {
            num = 1024 * 1024;
        }
        if (line[line.size() - 2] == 'g' || line[line.size() - 2] == 'G') {
            num = 1024 * 1024 * 1024;
        }
        return num;
    }

    size_t determine_available_hugepage_memory()
    {
        size_t			  size_in_bytes		 = 0;
        size_t			  page_size_in_bytes = 0;
        size_t			  num_free_pages	 = 0;
        const std::string meminfo_file		 = "/proc/meminfo";
        const std::string ps_str			 = "Hugepagesize:";
        const std::string pf_str			 = "HugePages_Free:";
        std::ifstream	 mifs(meminfo_file);
        if (mifs.is_open()) {
            // find size of one page
            std::string line;
            while (std::getline(mifs, line)) {
                auto ps = std::mismatch(ps_str.begin(), ps_str.end(), line.begin());
                if (ps.first == ps_str.end()) {
                    page_size_in_bytes = extract_number(line) * extract_multiplier(line);
                }
                auto pf = std::mismatch(pf_str.begin(), pf_str.end(), line.begin());
                if (pf.first == pf_str.end()) {
                    num_free_pages = extract_number(line);
                }
            }
            size_in_bytes = page_size_in_bytes * num_free_pages;
        } else {
            throw std::system_error(
                ENOMEM,
                std::system_category(),
                "hugepage_allocator could not automatically determine available hugepages");
        }
        return size_in_bytes;
    }

    void coalesce_block(mm_block_t* block)
    {
        //std::cout << "coalesce_block()" << std::endl;
        mm_block_t* newblock = block;
        if (block_nextfree(block, m_top)) {
            mm_block_t* next = block_next(block, m_top);
            /* remove the "next" block from the free list */
            remove_from_free_set(next);
            /* add the size of our block */
            block_update(block, UNMASK_SIZE(block->size) + UNMASK_SIZE(next->size));
        }
        if (block_prevfree(block, m_first_block)) {
            mm_block_t* prev = block_prev(block, m_first_block);
            /* we remove the old prev block and readd it to the correct
               size list if necessary */
            remove_from_free_set(prev);
            newblock = prev;
            block_update(prev, UNMASK_SIZE(prev->size) + UNMASK_SIZE(block->size));
        }
        if (newblock) {
            block_markfree(newblock);
            insert_into_free_set(newblock);
        }
    }

    void split_block(mm_block_t* bptr, size_t size)
    {
        //std::cout << "split_block("<< (void*)bptr << ")" << std::endl;
        size_t blocksize = UNMASK_SIZE(bptr->size);
        //std::cout << "cur_block_size = " << blocksize << std::endl;
        /* only split if we get at least a small block
           out of it */
        int64_t newblocksize = ALIGNSPLIT(blocksize - ALIGN(size + MM_BLOCK_OVERHEAD));
        //std::cout << "new_block_size = " << newblocksize << std::endl;
        if (newblocksize >= (int64_t)SPLIT_THRESHOLD) {
            /* update blocksize of old block */
            //std::cout << "block_update = " << blocksize-newblocksize << std::endl;
            block_update(bptr, blocksize - newblocksize);
            mm_block_t* newblock = (mm_block_t*)((char*)bptr + (blocksize - newblocksize));
            //std::cout << "new block ptr = " << (void*)newblock << std::endl;
            block_update(newblock, newblocksize);
            coalesce_block(newblock);
        }
    }

    uint8_t* hsbrk(size_t size)
    {
        ptrdiff_t left = (ptrdiff_t)m_total_size - (m_top - m_base);
        if (left < (ptrdiff_t)size) { // enough space left?
            throw std::system_error(
                ENOMEM, std::system_category(), "hugepage_allocator: not enough hugepage memory available");
        }
        uint8_t* new_mem = m_top;
        m_top += size;
        return new_mem;
    }

    mm_block_t* new_block(size_t size)
    {
        //std::cout << "new_block(" << size << ")" << std::endl;
        size						   = ALIGN(size + MM_BLOCK_OVERHEAD);
        if (size < MIN_BLOCKSIZE) size = MIN_BLOCKSIZE;
        mm_block_t* ptr				   = (mm_block_t*)hsbrk(size);
        block_update(ptr, size);
        return ptr;
    }

    void remove_from_free_set(mm_block_t* block)
    {
        //std::cout << "remove_from_free_set()" << std::endl;
        auto eq_range = m_free_large.equal_range(block->size);
        // find the block amoung the blocks with equal size
        auto itr   = eq_range.first;
        auto last  = eq_range.second;
        auto found = m_free_large.end();
        while (itr != last) {
            if (itr->second == block) {
                found = itr;
            }
            ++itr;
        }
        if (found == m_free_large.end()) {
            found = last;
        }
        m_free_large.erase(found);
    }

    void insert_into_free_set(mm_block_t* block)
    {
        //std::cout << "insert_into_free_set("<< (void*)block << "," << UNMASK_SIZE(block->size) << ")" << std::endl;
        //std::cout << "insert_into_free_set("<< (void*)block << "," << block->size << ")" << std::endl;
        m_free_large.insert({block->size, block});
    }

    mm_block_t* find_free_block(size_t size_in_bytes)
    {
        //std::cout << "find_free_block(" << size_in_bytes << ")" << std::endl;

        mm_block_t* bptr	   = nullptr;
        auto		free_block = m_free_large.lower_bound(size_in_bytes);
        if (free_block != m_free_large.end()) {
            bptr = free_block->second;
            m_free_large.erase(free_block);
        }
        return bptr;
    }

    mm_block_t* last_block()
    {
        mm_block_t* last = nullptr;
        //std::cout << "m_top = " << (void*)m_top << std::endl;
        //std::cout << "m_base = " << (void*)m_base << std::endl;
        if (m_top != m_base) {
            mm_block_foot_t* fptr = (mm_block_foot_t*)(m_top - sizeof(size_t));
            //std::cout << "foot of last = " << (void*)fptr << std::endl;
            //std::cout << "size of last = " << UNMASK_SIZE(fptr->size) << std::endl;
            last = (mm_block_t*)(((uint8_t*)fptr) - UNMASK_SIZE(fptr->size) + sizeof(size_t));
            //std::cout << "last = " << (void*)last << std::endl;
        }
        return last;
    }


    void print_heap()
    {
        mm_block_t* bptr = m_first_block;
        size_t		id   = 0;
        while (bptr) {
            block_print(id, bptr);
            id++;
            bptr = block_next(bptr, m_top);
        }
    }

public:
    void init(SDSL_UNUSED size_t size_in_bytes = 0)
    {
#ifdef MAP_HUGETLB
        if (size_in_bytes == 0) {
            size_in_bytes = determine_available_hugepage_memory();
        }

        m_total_size = size_in_bytes;
        m_base		 = (uint8_t*)mmap(nullptr,
                                        m_total_size,
                                        (PROT_READ | PROT_WRITE),
                                        (MAP_HUGETLB | MAP_ANONYMOUS | MAP_PRIVATE),
                                        0,
                                        0);
        if (m_base == MAP_FAILED) {
            throw std::system_error(
                ENOMEM, std::system_category(), "hugepage_allocator could not allocate hugepages");
        } else {
            // init the allocator
            m_top		  = m_base;
            m_first_block = (mm_block_t*)m_base;
        }
#else
        throw std::system_error(ENOMEM,
								std::system_category(),
								"hugepage_allocator: MAP_HUGETLB / hugepage support not available");
#endif
    }

    void* mm_realloc(void* ptr, size_t size)
    {
        //print_heap();
        //std::cout << "REALLOC(" << ptr << "," << size << ")" << std::endl;
        /* handle special cases first */
        if (nullptr == ptr) return mm_alloc(size);
        if (size == 0) {
            mm_free(ptr);
            return nullptr;
        }
        mm_block_t* bptr = block_cur(ptr);

        bool   need_malloc   = 0;
        size_t blockdatasize = block_getdatasize(bptr);
        /* we do nothing if the size is equal to the block */
        if (size == blockdatasize) {
            //std::cout << "return ptr = " << ptr << std::endl;
            return ptr; /* do nothing if size fits already */
        }
        if (size < blockdatasize) {
            /* we shrink */
            /* do we shrink enough to perform a split? */
            //std::cout << "shrink!" << std::endl;
            split_block(bptr, size);
        } else {
            //std::cout << "expand!" << std::endl;
            /* we expand */
            /* if the next block is free we could use it! */
            mm_block_t* next = block_next(bptr, m_top);
            if (!next) {
                //std::cout << "no next! -> expand!" << std::endl;
                // we are the last block so we just expand
                blockdatasize = block_getdatasize(bptr);
                size_t needed = ALIGN(size - blockdatasize);
                hsbrk(needed);
                block_update(bptr, UNMASK_SIZE(bptr->size) + needed);
                return block_data(bptr);
            } else {
                // we are not the last block
                //std::cout << "try combine next" << std::endl;
                if (next && block_isfree(next)) {
                    /* do we have enough space if we use the next block */
                    if (blockdatasize + UNMASK_SIZE(next->size) >= size) {
                        /* the next block is enough! */
                        /* remove the "next" block from the free list */
                        remove_from_free_set(next);
                        /* add the size of our block */
                        block_update(bptr, UNMASK_SIZE(bptr->size) + UNMASK_SIZE(next->size));
                    } else {
                        /* the next block is not enough. we allocate a new one instead */
                        need_malloc = true;
                    }
                } else {
                    /* try combing the previous block if free */
                    //std::cout << "try combine prev" << std::endl;
                    mm_block_t* prev = block_prev(bptr, m_first_block);
                    if (prev && block_isfree(prev)) {
                        if (blockdatasize + UNMASK_SIZE(prev->size) >= size) {
                            remove_from_free_set(prev);
                            size_t newsize = UNMASK_SIZE(prev->size) + UNMASK_SIZE(bptr->size);
                            block_update(prev, newsize);
                            block_markused(prev);
                            /* move the data into the previous block */
                            ptr = memmove(block_data(prev), ptr, blockdatasize);
                        } else {
                            /* not enough in the prev block */
                            need_malloc = true;
                        }
                    } else {
                        /* prev block not free. get more memory */
                        need_malloc = true;
                    }
                }
            }
        }
        if (need_malloc) {
            //std::cout << "need_alloc in REALLOC!" << std::endl;
            void* newptr = mm_alloc(size);
            memcpy(newptr, ptr, size);
            mm_free(ptr);
            ptr = newptr;
        }
        //print_heap();
        //std::cout << "return ptr = " << ptr << std::endl;
        return ptr;
    }

    void* mm_alloc(size_t size_in_bytes)
    {
        //std::cout << "ALLOC(" << size_in_bytes << ")" << std::endl;
        mm_block_t* bptr = nullptr;
        if ((bptr = find_free_block(size_in_bytes + MM_BLOCK_OVERHEAD)) != nullptr) {
            //std::cout << "found free block = " << (void*)bptr << std::endl;
            block_markused(bptr);
            /* split if we have a block too large for us? */
            split_block(bptr, size_in_bytes);
        } else {
            //std::cout << "no free block found that is big enough!" << std::endl;
            // check if last block is free
            //std::cout << "check last block" << std::endl;
            bptr = last_block();
            if (bptr && block_isfree(bptr)) {
                //std::cout << "last block is free. -> extend!" << std::endl;
                // extent last block as it is free
                size_t blockdatasize = block_getdatasize(bptr);
                size_t needed		 = ALIGN(size_in_bytes - blockdatasize);
                hsbrk(needed);
                remove_from_free_set(bptr);
                block_update(bptr, blockdatasize + needed + sizeof(size_t) + sizeof(mm_block_foot_t));
                //insert_into_free_set(bptr);
                block_markused(bptr);
            } else {
                bptr = new_block(size_in_bytes);
            }
        }
        //print_heap();
        //void* ptr = block_data(bptr);
        //std::cout << "return ptr = " << ptr << std::endl;
        return block_data(bptr);
    }

    void mm_free(void* ptr)
    {
        //print_heap();
        //std::cout << "FREE(" << ptr << ")" << std::endl;
        if (ptr) {
            mm_block_t* bptr = block_cur(ptr);
            block_markfree(bptr);
            /* coalesce if needed. otherwise just add */
            coalesce_block(bptr);
        }
        //print_heap();
    }

    bool in_address_space(void* ptr)
    {
        // check if ptr is in the hugepage address space
        if (ptr == nullptr) {
            return true;
        }
        if (ptr >= m_base && ptr < m_top) {
            return true;
        }
        return false;
    }
    static hugepage_allocator& the_allocator()
    {
        static hugepage_allocator a;
        return a;
    }
};
#endif

class memory_manager {
private:
    bool hugepages = false;

private:
    static memory_manager& the_manager()
    {
        static memory_manager m;
        return m;
    }

public:
    static uint64_t* alloc_mem(size_t size_in_bytes)
    {
#ifndef _WIN32
        auto& m = the_manager();
        if (m.hugepages) {
            return (uint64_t*)hugepage_allocator::the_allocator().mm_alloc(size_in_bytes);
        }
#endif
        return (uint64_t*)calloc(size_in_bytes, 1);
    }
    static void free_mem(uint64_t* ptr)
    {
#ifndef _WIN32
        auto& m = the_manager();
        if (m.hugepages and hugepage_allocator::the_allocator().in_address_space(ptr)) {
            hugepage_allocator::the_allocator().mm_free(ptr);
            return;
        }
#endif
        std::free(ptr);
    }
    static uint64_t* realloc_mem(uint64_t* ptr, size_t size)
    {
#ifndef _WIN32
        auto& m = the_manager();
        if (m.hugepages and hugepage_allocator::the_allocator().in_address_space(ptr)) {
            return (uint64_t*)hugepage_allocator::the_allocator().mm_realloc(ptr, size);
        }
#endif
        return (uint64_t*)realloc(ptr, size);
    }

public:
    static void use_hugepages(size_t bytes = 0)
    {
#ifndef _WIN32
        auto& m = the_manager();
        hugepage_allocator::the_allocator().init(bytes);
        m.hugepages = true;
#else
        throw std::runtime_error(std::string("hugepages not supported on Windows"));
        // avoid error: unused parameter 'bytes' [-Werror=unused-parameter]
        (void) bytes;
#endif
    }
    template <class t_vec>
    static void resize(t_vec& v, const typename t_vec::size_type capacity)
    {
        uint64_t old_capacity_in_bytes = ((v.m_capacity + 63) >> 6) << 3;
        uint64_t new_capacity_in_bytes = ((capacity + 63) >> 6) << 3;
        bool	 do_realloc		   = old_capacity_in_bytes != new_capacity_in_bytes;
        v.m_capacity      		   = ((capacity + 63) >> 6) << 6; // set new_capacity to a multiple of 64

        if (do_realloc || v.m_data == nullptr) {
            // Note that we allocate 8 additional bytes if m_capacity % 64 == 0.
            // We need this padding since rank data structures do a memory
            // access to this padding to answer rank(size()) if capacity()%64 ==0.
            // Note that this padding is not counted in the serialize method!
            size_t allocated_bytes = (size_t)(((v.m_capacity + 64) >> 6) << 3);
            v.m_data			   = memory_manager::realloc_mem(v.m_data, allocated_bytes);
            if (allocated_bytes != 0 && v.m_data == nullptr) {
                throw std::bad_alloc();
            }

            // update stats
            if (do_realloc) {
                memory_monitor::record((int64_t)new_capacity_in_bytes - (int64_t)old_capacity_in_bytes);
            }
        }
    }
    template <class t_vec>
    static void clear(t_vec& v)
    {
        int64_t size_in_bytes = ((v.m_size + 63) >> 6) << 3;
        // remove mem
        memory_manager::free_mem(v.m_data);
        v.m_data = nullptr;

        // update stats
        if (size_in_bytes) {
            memory_monitor::record(size_in_bytes * -1);
        }
    }

    static int open_file_for_mmap(std::string& filename, std::ios_base::openmode mode)
    {
        if (is_ram_file(filename)) {
            return ram_fs::open(filename);
        }
#ifdef MSVC_COMPILER
        int fd = -1;
		if (!(mode & std::ios_base::out))
			_sopen_s(&fd, filename.c_str(), _O_BINARY | _O_RDONLY, _SH_DENYNO, _S_IREAD);
		else
			_sopen_s(&fd, filename.c_str(), _O_BINARY | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		return fd;
#else
        if (!(mode & std::ios_base::out))
            return open(filename.c_str(), O_RDONLY);
        else
            return open(filename.c_str(), O_RDWR);
#endif
        return -1;
    }

    static void* mmap_file(int fd, uint64_t file_size, std::ios_base::openmode mode)
    {
        if (file_size == 0) {
            std::cout << "file_size=0" << std::endl;
            return nullptr;
        }
        if (is_ram_file(fd)) {
            if (ram_fs::file_size(fd) < file_size) return nullptr;
            auto& file_content = ram_fs::content(fd);
            return file_content.data();
        }
        memory_monitor::record(file_size);
#ifdef _WIN32
        HANDLE fh = (HANDLE)_get_osfhandle(fd);
		if (fh == INVALID_HANDLE_VALUE) {
			return nullptr;
		}
		HANDLE fm;
		if (!(mode & std::ios_base::out)) { // read only?
			fm = CreateFileMapping(fh, NULL, PAGE_READONLY, 0, 0, NULL);
		} else
			fm = CreateFileMapping(fh, NULL, PAGE_READWRITE, 0, 0, NULL);
		if (fm == NULL) {
			return nullptr;
		}
		void* map = nullptr;
		if (!(mode & std::ios_base::out)) { // read only?
			map = MapViewOfFile(fm, FILE_MAP_READ, 0, 0, file_size);
		} else
			map = MapViewOfFile(fm, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, file_size);
		// we can close the file handle before we unmap the view: (see UnmapViewOfFile Doc)
		// Although an application may close the file handle used to create a file mapping object,
		// the system holds the corresponding file open until the last view of the file is unmapped.
		// Files for which the last view has not yet been unmapped are held open with no sharing restrictions.
		CloseHandle(fm);
		return map;
#else
        void* map = nullptr;
        if (!(mode & std::ios_base::out))
            map = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        else
            map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) map = nullptr; // unify windows and unix error behaviour
        return map;
#endif
        return nullptr;
    }

    static int mem_unmap(int fd, void* addr, const uint64_t size)
    {
        if (addr == nullptr) {
            return 0;
        }
        if (is_ram_file(fd)) {
            return 0;
        }
        memory_monitor::record(-((int64_t)size));
#ifdef _WIN32
        if (UnmapViewOfFile(addr)) return 0;
		return -1;
#else
        return munmap(addr, size);
#endif
        return -1;
    }

    static int close_file_for_mmap(int fd)
    {
        if (is_ram_file(fd)) {
            return ram_fs::close(fd);
        }
#ifdef MSVC_COMPILER
        return _close(fd);
#else
        return close(fd);
#endif
        return -1;
    }

    static int truncate_file_mmap(int fd, const uint64_t new_size)
    {
        if (is_ram_file(fd)) {
            return ram_fs::truncate(fd, new_size);
        }
#ifdef _WIN32
        auto ret		  = _chsize_s(fd, new_size);
		if (ret != 0) ret = -1;
		return ret;
#else
        return ftruncate(fd, new_size);
#endif
        return -1;
    }
};

#undef ALIGNMENT
#undef ALIGNSPLIT
#undef ALIGN
#undef MM_BLOCK_OVERHEAD
#undef MIN_BLOCKSIZE
#undef UNMASK_SIZE
#undef ISFREE
#undef SETFREE
#undef SPLIT_THRESHOLD

} // end namespace

#endif

#include <iosfwd>	// forward declaration of ostream
#include <stdexcept> // for exceptions
#include <iostream>  // for cerr
#include <typeinfo>
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cstddef>
#include <ctime>   // for rand initialization
#include <cstring> // for memcpy
#include <ostream>
#include <istream>
#include <string>
#include <initializer_list>
#include <type_traits>
#include <vector>
#include <ios>

//! Namespace for the succinct data structure library.
namespace sdsl {

typedef uint64_t std_size_type_for_int_vector;

template <uint8_t t_width = 0>
class int_vector;

template <class int_vector_type>
class mm_item;

//! bit_vector is a specialization of the int_vector.
typedef int_vector<1> bit_vector;

template <class t_int_vector>
class int_vector_reference;

template <class t_int_vector>
class int_vector_iterator_base;

template <class t_int_vector>
class int_vector_iterator;

template <class t_int_vector>
class int_vector_const_iterator;

template <uint8_t t_width, std::ios_base::openmode t_mode>
class int_vector_mapper;

template <uint8_t b, uint8_t t_patter_len> // forward declaration
class rank_support_v;

class rank_support;

class select_support;

template <uint8_t t_bit_pattern, uint8_t t_pattern_len>
class select_support_mcl;

namespace coder {
template <typename T>
class fibonacci;
template <typename T>
class elias_delta;
template <typename T>
class elias_gamma;
template <uint8_t t_width>
class comma;
}

template <uint8_t t_width>
struct int_vec_category_trait {
    typedef iv_tag type;
};

template <>
struct int_vec_category_trait<1> {
    typedef bv_tag type;
};

template <uint8_t t_width>
struct int_vector_trait {
    typedef uint64_t								   value_type;
    typedef int_vector<t_width>						   int_vector_type;
    typedef int_vector_reference<int_vector_type>	  reference;
    typedef uint64_t							   const_reference;
    typedef uint8_t									   int_width_type;
    typedef int_vector_iterator<int_vector_type>	   iterator;
    typedef int_vector_const_iterator<int_vector_type> const_iterator;

    static iterator begin(int_vector_type* v, uint64_t*) noexcept { return iterator(v, 0); }
    static iterator end(int_vector_type* v, uint64_t*, int_vector_size_type) noexcept
    {
        return iterator(v, v->size() * v->width());
    }
    static const_iterator begin(const int_vector_type* v, const uint64_t*) noexcept
    {
        return const_iterator(v, 0);
    }
    static const_iterator end(const int_vector_type* v, const uint64_t*, int_vector_size_type) noexcept
    {
        return const_iterator(v, v->size() * v->width());
    }

    static void set_width(uint8_t new_width, int_width_type& width) noexcept
    {
        if (t_width == 0) {
            if (0 < new_width and new_width <= 64)
                width = new_width;
            else
                width = 64;
        }
    }
};

template <>
struct int_vector_trait<64> {
    typedef uint64_t		value_type;
    typedef int_vector<64>  int_vector_type;
    typedef uint64_t&		reference;
    typedef uint64_t  const_reference;
    typedef uint8_t			int_width_type;
    typedef uint64_t*		iterator;
    typedef const uint64_t* const_iterator;

    static iterator begin(int_vector_type*, uint64_t* begin) noexcept { return begin; }
    static iterator end(int_vector_type*, uint64_t* begin, int_vector_size_type size) noexcept
    {
        return begin + size;
    }
    static const_iterator begin(const int_vector_type*, const uint64_t* begin) noexcept { return begin; }
    static const_iterator
    end(const int_vector_type*, const uint64_t* begin, int_vector_size_type size) noexcept
    {
        return begin + size;
    }

    static void set_width(uint8_t, int_width_type) noexcept {}
};

template <>
struct int_vector_trait<32> {
    typedef uint32_t		value_type;
    typedef int_vector<32>  int_vector_type;
    typedef uint32_t&		reference;
    typedef uint32_t  const_reference;
    typedef uint8_t			int_width_type;
    typedef uint32_t*		iterator;
    typedef const uint32_t* const_iterator;

    static iterator begin(int_vector_type*, uint64_t* begin) noexcept { return (uint32_t*)begin; }
    static iterator end(int_vector_type*, uint64_t* begin, int_vector_size_type size) noexcept
    {
        return ((uint32_t*)begin) + size;
    }
    static const_iterator begin(const int_vector_type*, const uint64_t* begin) noexcept
    {
        return (uint32_t*)begin;
    }
    static const_iterator
    end(const int_vector_type*, const uint64_t* begin, int_vector_size_type size) noexcept
    {
        return ((uint32_t*)begin) + size;
    }

    static void set_width(uint8_t, int_width_type) noexcept {}
};

template <>
struct int_vector_trait<16> {
    typedef uint16_t		value_type;
    typedef int_vector<16>  int_vector_type;
    typedef uint16_t&		reference;
    typedef uint16_t  const_reference;
    typedef uint8_t			int_width_type;
    typedef uint16_t*		iterator;
    typedef const uint16_t* const_iterator;

    static iterator begin(int_vector_type*, uint64_t* begin) noexcept { return (uint16_t*)begin; }
    static iterator end(int_vector_type*, uint64_t* begin, int_vector_size_type size) noexcept
    {
        return ((uint16_t*)begin) + size;
    }
    static const_iterator begin(const int_vector_type*, const uint64_t* begin) noexcept
    {
        return (uint16_t*)begin;
    }
    static const_iterator
    end(const int_vector_type*, const uint64_t* begin, int_vector_size_type size) noexcept
    {
        return ((uint16_t*)begin) + size;
    }

    static void set_width(uint8_t, int_width_type) noexcept {}
};

template <>
struct int_vector_trait<8> {
    typedef uint8_t		   value_type;
    typedef int_vector<8>  int_vector_type;
    typedef uint8_t&	   reference;
    typedef uint8_t  const_reference;
    typedef uint8_t		   int_width_type;
    typedef uint8_t*	   iterator;
    typedef const uint8_t* const_iterator;

    static iterator begin(int_vector_type*, uint64_t* begin) noexcept { return (uint8_t*)begin; }
    static iterator end(int_vector_type*, uint64_t* begin, int_vector_size_type size) noexcept
    {
        return ((uint8_t*)begin) + size;
    }
    static const_iterator begin(const int_vector_type*, const uint64_t* begin) noexcept
    {
        return (uint8_t*)begin;
    }
    static const_iterator
    end(const int_vector_type*, const uint64_t* begin, int_vector_size_type size) noexcept
    {
        return ((uint8_t*)begin) + size;
    }

    static void set_width(uint8_t, int_width_type) noexcept {}
};

//! A generic vector class for integers of width \f$w\in [1..64]\f$.
/*! \author Simon Gog
 *
 *    This generic vector class could be used to generate a vector
 *    that contains integers of fixed width \f$w\in [1..64]\f$.
 *    It has a growth factor of 1.5 to achieve amortized running time.
 *    Note that resize() does not reserve more space than necessary.
 *
 *  \tparam t_width Width of the integer. If set to `0` it is variable
 *          during runtime, otherwise fixed at compile time.
 *  @ingroup int_vector
 */
template <uint8_t t_width>
class int_vector {
private:
    static_assert(t_width <= 64, "int_vector: width of must be at most 64bits.");

public:
    typedef typename int_vector_trait<t_width>::value_type		value_type;
    typedef typename int_vector_trait<t_width>::iterator		iterator;
    typedef typename int_vector_trait<t_width>::const_iterator  const_iterator;
    typedef typename int_vector_trait<t_width>::reference		reference;
    typedef typename int_vector_trait<t_width>::const_reference const_reference;
    typedef int_vector_reference<int_vector>*					pointer;
    typedef const value_type*									const_pointer;
    typedef ptrdiff_t											difference_type;
    typedef int_vector_size_type								size_type;
    typedef typename int_vector_trait<t_width>::int_width_type  int_width_type;
    typedef rank_support_v<1, 1>	 rank_1_type;
    typedef rank_support_v<0, 1>	 rank_0_type;
    typedef select_support_mcl<1, 1> select_1_type;
    typedef select_support_mcl<0, 1> select_0_type;
    typedef typename int_vec_category_trait<t_width>::type index_category;

    friend struct int_vector_trait<t_width>;
    friend class int_vector_iterator_base<int_vector>;
    friend class int_vector_iterator<int_vector>;
    friend class int_vector_const_iterator<int_vector>;
    template <uint8_t, std::ios_base::openmode>
    friend class int_vector_mapper;
    template <typename T>
    friend class coder::elias_delta;
    template <typename T>
    friend class coder::elias_gamma;
    template <typename T>
    friend class coder::fibonacci;
    template <uint8_t>
    friend class coder::comma;
    friend class memory_manager;
    static constexpr uint8_t fixed_int_width = t_width;
    float growth_factor = 1.5; //!< Growth factor for amortized constant time operations
    // see the explanation in the documentation of FBVector on different growth factors
    // https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md#memory-handling

private:
    size_type	  m_size;  //!< Number of bits needed to store int_vector.
    size_type	  m_capacity; //!< Number of bits reserved by int_vector.
    uint64_t*	  m_data;  //!< Pointer to the memory for the bits.
    int_width_type m_width; //!< Width of the integers.

    // Hidden, since number of bits (size) does not go well together with int value.
    void bit_resize(const size_type size, const value_type value);

    void amortized_resize(const size_type size)
    {
        assert(growth_factor > 1.0);
        size_type bit_size = size * m_width;
        if (bit_size > m_capacity || m_data == nullptr) {
            // start with 64 bit if vector has no capacity
            size_type tmp_capacity = m_capacity == 0 ? 64 : m_capacity;
            // find smallest x s.t. m_capacity * 1.5 ** x >= size
            auto resize_factor = pow(growth_factor, std::ceil(std::log((bit_size + tmp_capacity - 1) / tmp_capacity) / std::log(growth_factor)));
            size_type new_capacity = std::ceil(tmp_capacity * resize_factor);
            memory_manager::resize(*this, new_capacity);
        }
        m_size = size * m_width;
    }

    // The number of 64-bit words used by the int_vector.
    size_type bit_data_size() const { return (m_size + 63) >> 6; }

public:
    //! Constructor for int_vector.
    /*! \param size          Number of elements. Default value is 0.
            \param default_value Initialize all value to `default value`.
            \param int_width     The width of each integer.
            \sa resize, width
         */

    int_vector(size_type size, value_type default_value, uint8_t int_width = t_width);

    //! Constructor to fix possible comparison with integeres issue.
    explicit int_vector(size_type size = 0) : int_vector(size, static_cast<value_type>(0), t_width)
    {
    }
    //! Constructor for initializer_list.
    int_vector(std::initializer_list<value_type> il) : int_vector(0, 0)
    {
        assign(il);
    }

    //! Constructor for iterator range
    /*! \param first Iterator pointing to first element to be copied.
	    \param last  Iterator pointing to the element behind the last one to be copied.
	 */
    template <typename input_iterator_t>
    int_vector(
        typename std::enable_if<
            std::is_base_of<
                std::input_iterator_tag, typename std::iterator_traits<input_iterator_t>::iterator_category
            >::value,
            input_iterator_t
        >::type first,
        input_iterator_t last) : int_vector(0, 0)
    {
        assign(first, last);
    }

    //! Clearing the int_vector. Allocated memory will not be released.
    /*! \sa resize
	 */
    void clear() noexcept { m_size = 0; }

    //! Remove element that iterator is pointing to.
    /*! \param it Iterator pointing to an element in int_vector
	 */
    iterator erase(const_iterator it)
    {
        iterator it_nonconst = begin() + (it - cbegin());
        std::copy(it_nonconst + 1, end(), it_nonconst);
        resize(size() - 1);
        return it_nonconst;
    }

    //! Remove elements in given iterator range.
    /*! \param first Iterator pointing to first element to be deleted.
           \param last  Iterator pointing to the elemnt after the one to be deleted.
	 */
    iterator erase(const_iterator first, const_iterator last)
    {
        iterator first_nonconst = begin() + (first - cbegin());
        iterator last_nonconst = begin() + (last - cbegin());
        std::copy(last_nonconst, end(), first_nonconst);
        resize(size() - (last - first));
        return first_nonconst;
    }

    //! Insert an element constructed with std::forward<Args>(args) before the element that the iterator is pointing to.
    /*! \param it   Iterator pointing to an element in int_vector.
           \param args Function parameter pack.
	 */
    template <class... Args>
    iterator emplace(const_iterator it, Args&&... args)
    {
        return insert(it, 1, value_type(std::forward<Args>(args) ...));
    }

    //! Insert an element before the element that the iterator is pointing to.
    /*! \param it    Iterator pointing to an element in int_vector.
           \param value Element to be inserted.
	 */
    iterator insert(const_iterator it, value_type value) { return insert(it, 1, value); }

    //! Insert n copies of an element before the element that the iterator is pointing to.
    /*! \param it    Iterator pointing to an element in int_vector.
           \param n     Number of copies.
           \param value Element to be inserted.
	 */
    iterator insert(const_iterator it, size_type n, value_type value)
    {
        size_type pos = it - cbegin();
        amortized_resize(size() + n);
        iterator it_new = begin() + pos;
        std::copy_backward(it_new, end() - n, end());
        std::fill_n(it_new, n, value);
        return it_new;
    }

    //! Insert elements from intializer_list before the element that the iterator is pointing to.
    /*! \param it Iterator pointing to an element in int_vector.
           \param il Elements to be inserted.
	 */
    iterator insert(const_iterator it, std::initializer_list<value_type> il)
    {
        return insert(it, il.begin(), il.end());
    }

    //! Insert elements from an iterator pair before the element that the iterator `it` is pointing to.
    /*! \param it    Iterator pointing to an element in int_vector.
           \param first Iterator pointing to first element to be inserted.
           \param last  Iterator pointing to the elemnt after the one to be inserted.
	 */
    template <typename input_iterator_t>
    typename std::enable_if<
        std::is_base_of<
            std::input_iterator_tag, typename std::iterator_traits<input_iterator_t>::iterator_category
        >::value,
        iterator
    >::type insert(const_iterator it,
                   input_iterator_t first, input_iterator_t last)
    {
        size_type pos = it - cbegin();
        amortized_resize(size() + last - first);
        iterator it_new = begin() + pos;
        std::copy_backward(it_new, end() - (last - first), end());
        std::copy(first, last, it_new);
        return it_new;
    }

    //! Returns first element.
    reference front() noexcept { return *begin(); }

    //! Returns first element.
    const_reference front() const noexcept { return *cbegin(); }

    //! Returns last element.
    reference back() noexcept { return *(end()-1); }

    //! Returns last element.
    const_reference back() const noexcept { return *(cend() - 1); }

    //! Insert an element constructed with std::forward<Args>(args) at the end.
    /*! \param args Function parameter pack.
	 */
    template <class... Args>
    void emplace_back(Args&&... args)
    {
        push_back(value_type(std::forward<Args>(args) ...));
    }

    //! Insert element at the end.
    /*! \param value Element to be inserted.
	 */
    void push_back(value_type value)
    {
        amortized_resize(size() + 1);
        *(end() - 1) = value;
    }

    //! Remove element at the end.
    void pop_back() { resize(size() - 1); }

    //! Move constructor.
    int_vector(int_vector&& v);

    //! Copy constructor.
    int_vector(const int_vector& v);

    //! Destructor.
    ~int_vector();

    //! Assign. Resize int_vector to `size` and fill elements with `default_value`.
    /*! \param size Number of elements.
           \param default_value Elements to be inserted.
	 */
    void assign(size_type size, value_type default_value)
    {
        bit_resize(size * m_width);
        util::set_to_value(*this, default_value); // new initialization
    }

    //! Assign. Resize int_vector and initialize with initializer_list.
    /*! \param il Initializer_list.
	 */
    void assign(std::initializer_list<value_type> il)
    {
        bit_resize(il.size() * m_width);
        size_type idx = 0;
        for (auto x : il) {
            (*this)[idx++] = x;
        }
    }

    //! Assign. Resize int_vector and initialize by copying from an iterator range.
    /*! \param first Iterator pointing to first element to be inserted.
           \param last  Iterator pointing to the elemnt after the one to be inserted.
	 */
    template <typename input_iterator_t>
    void assign(input_iterator_t first, input_iterator_t last)
    {
        assert(first <= last);
        bit_resize((last - first) * m_width);
        size_type idx = 0;
        while (first < last) {
            (*this)[idx++] = *(first++);
        }
    }

    //! Equivalent to size() == 0.
    bool empty() const noexcept { return 0 == m_size; }

    //! Swap method for int_vector.
    void swap(int_vector& v) noexcept { std::swap(v, *this); }

    //! Free unused allocated memory.
    void shrink_to_fit() { memory_manager::resize(*this, m_size); }

    //! Reserve storage. If the new capacity is smaller than the current, this method does nothing.
    /*! \param capacity New capacity in bits
	 */
    void reserve(size_type capacity)
    {
        if (capacity * m_width > m_capacity || m_data == nullptr) {
            memory_manager::resize(*this, capacity * m_width);
        }
    }

    //! Resize the int_vector in terms of elements. If the current size is smaller than `size`, the additional elements are initialized with 0.
    /*! Only as much space as necessary is being allocated.
	    \param size Number of elements.
	 */
    void resize(const size_type size) { resize(size, 0); }

    //! Resize the int_vector in terms of elements. Only as much space as necessary is allocated.
    /*! \param size The size to resize the int_vector in terms of elements.
        \param value If the current size is smaller than `size`, the additional elements are initialized with value.
	 */
    void resize(const size_type size, const value_type value) { bit_resize(size * m_width, value); }

    //! Resize the int_vector in terms of bits. Only as much space as necessary is allocated.
    /*! \param size The size to resize the int_vector in terms of bits.
         */
    void bit_resize(const size_type size);

    //! The number of elements in the int_vector.
    /*! \sa max_size, bit_size, capacity, bit_capacity
         */
    inline size_type size() const noexcept;

    //! Maximum size of the int_vector.
    /*! \sa size, bit_size, capacity, bit_capacity
        */
    static size_type max_size() noexcept { return ((size_type)1) << (sizeof(size_type) * 8 - 6); }

    //! The number of bits in the int_vector.
    /*!  \sa size, max_size, bit_size, capacity
         */
    size_type bit_size() const noexcept { return m_size; }

    //! Returns the size of the occupied bits of the int_vector.
    /*! The capacity of a int_vector is greater or equal to the
            size of the vector: capacity() >= size().
            \sa size, bit_size, max_size, capacity, bit_capacity
         */
    inline size_type capacity() const noexcept;

    //! Returns the size of the occupied bits of the int_vector.
    /*! The bit_capacity of a int_vector is greater or equal to the
            bit_size of the vector: bit_capacity() >= bit_size().
            \sa size, bit_size, max_size, capacity, bit_capacity
         */
    size_type bit_capacity() const noexcept { return m_capacity; }

    //! Pointer to the raw data of the int_vector
    /*! \returns Const pointer to the raw data of the int_vector
         */
    const uint64_t* data() const noexcept { return m_data; }

    //! Pointer to the raw data of the int_vector
    /*! \returns pointer to the raw data of the int_vector
         */
    uint64_t* data() noexcept { return m_data; }

    //! Get the integer value of the binary string of length len starting at position idx in the int_vector.
    /*! \param idx Starting index of the binary representation of the integer.
            \param len Length of the binary representation of the integer. Default value is 64.
            \returns The integer value of the binary string of length len starting at position idx.
            \sa setInt, getBit, setBit
        */
    value_type get_int(size_type idx, const uint8_t len = 64) const;

    //! Set the bits from position idx to idx+len-1 to the binary representation of integer x.
    /*! The bit at position idx represents the least significant bit(lsb), and the bit at
            position idx+len-1 the most significant bit (msb) of x.
            \param idx Starting index of the binary representation of x.
            \param x   The integer to store in the int_vector.
            \param len The length used to store x in the int_vector. Default value is 64.
            \sa getInt, getBit, setBit
        */
    void set_int(size_type idx, value_type x, const uint8_t len = 64);

    //! Returns the width of the integers which are accessed via the [] operator.
    /*! \returns The width of the integers which are accessed via the [] operator.
            \sa width
        */
    uint8_t width() const noexcept { return m_width; }

    //! Sets the width of the integers which are accessed via the [] operator, if t_width equals 0.
    /*! \param new_width New width of the integers accessed via the [] operator.
            \note This method has no effect if t_width is in the range [1..64].
              \sa width
        */
    void width(uint8_t new_width) noexcept { int_vector_trait<t_width>::set_width(new_width, m_width); }

    // Write data (without header) to a stream.
    size_type write_data(std::ostream& out) const;

    //! Serializes the int_vector to a stream.
    /*! \return The number of bytes written to out.
         *  \sa load
         */
    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const;

    //! Load the int_vector for a stream.
    void load(std::istream& in);

    /* For cereal we need to define different versions of the load and save function depending on whether we want
	 * binary data or text (XML/JSON) data.
	 * See https://github.com/USCiLab/cereal/blob/master/include/cereal/types/vector.hpp for an example.
	 */

    //!\brief Serialise (save) via cereal if archive is not binary
    template <typename archive_t> inline
    typename std::enable_if<!cereal::traits::is_output_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
    CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const;

    //!\brief Serialise (save) via cereal if archive is binary
    template <typename archive_t> inline
    typename std::enable_if<cereal::traits::is_output_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
    CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const;

    //!\brief Serialise (load) via cereal if archive is not binary
    template <typename archive_t> inline
    typename std::enable_if<!cereal::traits::is_input_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
    CEREAL_LOAD_FUNCTION_NAME(archive_t & ar);

    //!\brief Serialise (save) via cereal if archive is binary
    template <typename archive_t> inline
    typename std::enable_if<cereal::traits::is_input_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
    CEREAL_LOAD_FUNCTION_NAME(archive_t & ar);

    //! non const version of [] operator
    /*! \param i Index the i-th integer of length width().
         *  \return A reference to the i-th integer of length width().
         */
    inline reference operator[](const size_type& i) noexcept;

    //! const version of [] operator
    /*! \param i Index the i-th integer of length width().
         *  \return The value of the i-th integer of length width().
         */
    inline const_reference operator[](const size_type& i) const noexcept;

    //! non const version of at() function
    /*! \param i Index the i-th integer of length width().
 	 *  \return A reference to the i-th integer of length width().
 	 */
    reference at(const size_type& i) { return (*this)[i]; }

    //! const version of at() function
    /*! \param i Index the i-th integer of length width().
 	 *  \return The value of the i-th integer of length width().
 	 */
    const_reference at(const size_type& i) const { return (*this)[i]; }

    //! Assignment operator.
    /*! \param v The vector v which should be assigned
         */
    int_vector& operator=(const int_vector& v);

    //! Move assignment operator.
    int_vector& operator=(int_vector&& v);

    //! Equality operator for two int_vectors.
    /*! Two int_vectors are equal if
         *    - sizes are equal and
         *    - its elements are equal.
         */
    bool operator==(const int_vector<t_width> & v) const noexcept
    {
        if (bit_size() != v.bit_size()) return false;
        if (empty()) return true;
        const uint64_t* data1 = v.data();
        const uint64_t* data2 = data();
        for (size_type i = 0; i < bit_data_size() - 1; ++i) {
            if (*(data1++) != *(data2++)) return false;
        }
        uint8_t l = 64 - ((bit_data_size() << 6) - m_size);
        return ((*data1) & bits::lo_set[l]) == ((*data2) & bits::lo_set[l]);
    }

    //! Equality operator for an arbitrary container.
    /*! Note that this function is slow since it compares element by element
	    and cannot compare the bit representations of the containers.
	    Two containers are equal if
         *    - sizes are equal and
         *    - its elements are equal.
         */
    template <class container>
    bool operator==(const container& v) const noexcept
    {
        return std::equal(begin(), end(), v.begin());
    }

    //! Inequality operator for two int_vectors.
    /*! Two int_vectors are not equal if
         *    - sizes are not equal or
         *    - its elements are not equal.
		Note that comparing two int_vectors of different widths is slow
 		since it compares element by element and not the bit representations of the int_vectors.
         */
    template <uint8_t t_width2>
    bool operator!=(const int_vector<t_width2> & v) const noexcept { return !(*this == v); }

    //! Less operator for two int_vectors
    /*! int_vector w is less than v if
         *    - w[i]==v[i] for i<j and w[j]<v[j] with j in [0, min(w.size(), v.size()) )
         *    - or w[i]==v[i] for all i < min(w.size(), v.size()) and w.size()<v.size().
         *  \sa operator>
        */
    bool operator<(const int_vector& v) const noexcept;

    //! Greater operator for two int_vectors
    /*! int_vector w is greater than v if
         *    - w[i]==v[i] for i<j and w[j]>v[j] with j in [0, min(w.size(), v.size()) )
         *    - or w[i]==v[i] for all i < min(w.size(), v.size()) and w.size()>v.size().
        */
    bool operator>(const int_vector& v) const noexcept;

    //! Less or equal operator
    bool operator<=(const int_vector& v) const noexcept;

    //! Greater of equal operator
    bool operator>=(const int_vector& v) const noexcept;

    //! bitwise-and-update operator
    int_vector& operator&=(const int_vector& v);

    //! bitwise-or-update equal operator
    int_vector& operator|=(const int_vector& v);

    //! bitwise-xor-update operator
    int_vector& operator^=(const int_vector& v);

    //! Iterator that points to the first element of the int_vector.
    /*!  Time complexity guaranty is O(1).
         */
    iterator begin() noexcept { return int_vector_trait<t_width>::begin(this, m_data); }

    //! Iterator that points to the element after the last element of int_vector.
    /*! Time complexity guaranty is O(1).
         */
    iterator end() noexcept { return int_vector_trait<t_width>::end(this, m_data, (m_size / m_width)); }

    //! Const iterator that points to the first element of the int_vector.
    const_iterator begin() const noexcept { return int_vector_trait<t_width>::begin(this, m_data); }

    //! Const iterator that points to the element after the last element of int_vector.
    const_iterator end() const noexcept { return int_vector_trait<t_width>::end(this, m_data, (m_size / m_width)); }

    //! Const iterator that points to the first element of the int_vector.
    const_iterator cbegin() const noexcept { return int_vector_trait<t_width>::begin(this, m_data); }

    //! Const iterator that points to the element after the last element of int_vector.
    const_iterator cend() const noexcept { return int_vector_trait<t_width>::end(this, m_data, (m_size / m_width)); }

    //! Flip all bits of bit_vector
    void flip()
    {
        static_assert(1 == t_width, "int_vector: flip() is available only for bit_vector.");
        if (!empty()) {
            for (uint64_t i = 0; i < bit_data_size(); ++i) {
                m_data[i] = ~m_data[i];
            }
        }
    }

    //! Read the size and int_width of a int_vector
    static size_t
    read_header(int_vector_size_type& size, int_width_type& int_width, std::istream& in)
    {
        uint64_t width_and_size = 0;
        read_member(width_and_size, in);
        size				   = width_and_size & bits::lo_set[56];
        uint8_t read_int_width = (uint8_t)(width_and_size >> 56);
        if (t_width == 0) {
            int_width = read_int_width;
        }
        if (t_width > 0 and t_width != read_int_width) {
            std::cerr << "Warning: Width of int_vector<" << (size_t)t_width << ">";
            std::cerr << " was specified as " << (size_type)read_int_width << std::endl;
            std::cerr << "Length is " << size << " bits" << std::endl;
        }
        return sizeof(width_and_size);
    }

    //! Write the size and int_width of a int_vector
    static uint64_t write_header(uint64_t size, uint8_t int_width, std::ostream& out)
    {
        if (t_width > 0) {
            if (t_width != int_width) {
                std::cout << "Warning: writing width=" << (size_type)int_width << " != fixed "
                          << (size_type)t_width << std::endl;
            }
        }
        uint64_t width_and_size = (((uint64_t)int_width) << 56) | size;
        return write_member(width_and_size, out);
    }

    struct raw_wrapper {
        const int_vector& vec;
        raw_wrapper() = delete;
        raw_wrapper(const int_vector& _vec) : vec(_vec) {}

        size_type
        serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
        {
            structure_tree_node* child =
                structure_tree::add_child(v, name, util::class_name(*this));
            auto written_bytes = vec.write_data(out);
            structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }
    };

    const raw_wrapper raw = raw_wrapper(*this);
};

//! A proxy class that acts as a reference to an integer of length \p len bits in a int_vector.
/*! \tparam t_int_vector The specific int_vector class.
 */
template <class t_int_vector>
class int_vector_reference {
public:
    typedef typename t_int_vector::value_type value_type;

private:
    typename t_int_vector::value_type* const m_word;
    const uint8_t							 m_offset;
    const uint8_t							 m_len; //!< Length of the integer referred to in bits.
public:

    //! Default constructor explicitly deleted.
    int_vector_reference() = delete;
    //! Copy and move explicitly defaulted.
    constexpr int_vector_reference(int_vector_reference const &) noexcept = default;
    constexpr int_vector_reference(int_vector_reference &&) noexcept = default;

    //! Constructor for the reference class
    /*! \param word Pointer to the corresponding 64bit word in the int_vector.
            \param offset Offset to the starting bit (offset in [0..63])
            \param len length of the integer, should be v->width()!!!
        */
    int_vector_reference(value_type* word, uint8_t offset, uint8_t len) noexcept
        : m_word(word), m_offset(offset), m_len(len){};

    //! Assignment operator for the proxy class
    /*!
            The integer x is assign to the referenced
            position in the t_int_vector with the specified width
            of the int_vector
            \param x 64bit integer to assign
            \return A const_reference to the assigned reference
         */
    int_vector_reference& operator=(value_type x) noexcept
    {
        bits::write_int(m_word, x, m_offset, m_len);
        return *this;
    };

    int_vector_reference& operator=(const int_vector_reference& x) noexcept
    {
        return *this = value_type(x);
    };

    int_vector_reference& operator=(int_vector_reference&& x) noexcept
    {
        return *this = value_type(std::move(x));
    };

    //! Cast the reference to a int_vector<>::value_type
    operator value_type() const noexcept { return bits::read_int(m_word, m_offset, m_len); }

    //! Prefix increment of the proxy object
    int_vector_reference& operator++() noexcept
    {
        value_type x = bits::read_int(m_word, m_offset, m_len);
        bits::write_int(m_word, x + 1, m_offset, m_len);
        return *this;
    }

    //! Postfix increment of the proxy object
    value_type operator++(int) noexcept
    {
        value_type val = (typename t_int_vector::value_type) * this;
        ++(*this);
        return val;
    }

    //! Prefix decrement of the proxy object
    int_vector_reference& operator--() noexcept
    {
        value_type x = bits::read_int(m_word, m_offset, m_len);
        bits::write_int(m_word, x - 1, m_offset, m_len);
        return *this;
    }

    //! Postfix decrement of the proxy object
    value_type operator--(int) noexcept
    {
        value_type val = (value_type) * this;
        --(*this);
        return val;
    }

    //! Add assign from the proxy object
    int_vector_reference& operator+=(const value_type x) noexcept
    {
        value_type w = bits::read_int(m_word, m_offset, m_len);
        bits::write_int(m_word, w + x, m_offset, m_len);
        return *this;
    }

    //! Subtract assign from the proxy object
    int_vector_reference& operator-=(const value_type x) noexcept
    {
        value_type w = bits::read_int(m_word, m_offset, m_len);
        bits::write_int(m_word, w - x, m_offset, m_len);
        return *this;
    }

    bool operator==(const int_vector_reference& x) const noexcept
    {
        return value_type(*this) == value_type(x);
    }

    bool operator<(const int_vector_reference& x) const noexcept
    {
        return value_type(*this) < value_type(x);
    }
};

// For C++11
template <class t_int_vector>
inline void swap(int_vector_reference<t_int_vector> x, int_vector_reference<t_int_vector> y) noexcept
{
    // TODO: more efficient solution?
    typename int_vector_reference<t_int_vector>::value_type tmp = x;
    x															= y;
    y															= tmp;
}

// For C++11
template <class t_int_vector>
inline void swap(typename int_vector_reference<t_int_vector>::value_type& x,
                 int_vector_reference<t_int_vector>						  y) noexcept
{
    // TODO: more efficient solution?
    typename int_vector_reference<t_int_vector>::value_type tmp = x;
    x															= y;
    y															= tmp;
}

// For C++11
template <class t_int_vector>
inline void swap(int_vector_reference<t_int_vector>						  x,
                 typename int_vector_reference<t_int_vector>::value_type& y) noexcept
{
    // TODO: more efficient solution?
    typename int_vector_reference<t_int_vector>::value_type tmp = x;
    x															= y;
    y															= tmp;
}

// specialization for int_vector_reference for int_vector == bit_vector
// special thanks to Timo Beller, who pointed out that the specialization is missing
// Same implementation as in stl_bvector.h.
template <>
class int_vector_reference<bit_vector> {
public:
    typedef bool value_type;

private:
    uint64_t* const m_word;
    uint64_t		m_mask;

public:

    //! Default constructor explicitly deleted.
    int_vector_reference() = delete;
    //! Copy and move explicitly defaulted.
    constexpr int_vector_reference(int_vector_reference const &) noexcept = default;
    constexpr int_vector_reference(int_vector_reference &&) noexcept = default;

    //! Constructor for the reference class
    /*! \param word Pointer to the corresponding 64bit word in the int_vector.
            \param offset Offset to the starting bit (offset in [0..63])
        */
    int_vector_reference(uint64_t* word, uint8_t offset, uint8_t) noexcept
        : m_word(word), m_mask(1ULL << offset){};

    //! Assignment operator for the proxy class
    int_vector_reference& operator=(bool x) noexcept
    {
        if (x)
            *m_word |= m_mask;
        else
            *m_word &= ~m_mask;
        return *this;
    };

    int_vector_reference& operator=(const int_vector_reference& x) noexcept { return *this = bool(x); };
    int_vector_reference& operator=(int_vector_reference && x) noexcept { return *this = bool(x); };

    //! Cast the reference to a bool
    operator bool() const noexcept { return !!(*m_word & m_mask); }

    bool operator==(const int_vector_reference& x) const noexcept { return bool(*this) == bool(x); }

    bool operator<(const int_vector_reference& x) const noexcept { return !bool(*this) && bool(x); }
};

// For C++11
template <>
inline void swap(int_vector_reference<bit_vector> x, int_vector_reference<bit_vector> y) noexcept
{
    // TODO: more efficient solution?
    bool tmp = x;
    x		 = y;
    y		 = tmp;
}

// For C++11
template <>
inline void swap(bool& x, int_vector_reference<bit_vector> y) noexcept
{
    // TODO: more efficient solution?
    bool tmp = x;
    x		 = y;
    y		 = tmp;
}

// For C++11
template <>
inline void swap(int_vector_reference<bit_vector> x, bool& y) noexcept
{
    // TODO: more efficient solution?
    bool tmp = x;
    x		 = y;
    y		 = tmp;
}

template <class t_int_vector>
class int_vector_iterator_base : public std::iterator<std::random_access_iterator_tag,
                                                      typename t_int_vector::value_type,
                                                      typename t_int_vector::difference_type> {
public:
    typedef uint64_t size_type;

protected:
    uint8_t m_offset;
    uint8_t m_len;

public:
    int_vector_iterator_base(uint8_t offset, uint8_t len) : m_offset(offset), m_len(len) {}

    int_vector_iterator_base(const t_int_vector* v = nullptr, size_type idx = 0)
        : m_offset(idx & 0x3F), m_len(v == nullptr ? 0 : v->m_width)
    {
    }
};

template <class t_int_vector>
class int_vector_iterator : public int_vector_iterator_base<t_int_vector> {
public:
    typedef int_vector_reference<t_int_vector>	 reference;
    typedef uint64_t							   value_type;
    typedef int_vector_iterator					   iterator;
    typedef reference*							   pointer;
    typedef typename t_int_vector::size_type	   size_type;
    typedef typename t_int_vector::difference_type difference_type;

    friend class int_vector_const_iterator<t_int_vector>;

private:
    using int_vector_iterator_base<t_int_vector>::m_offset; // make m_offset easy usable
    using int_vector_iterator_base<t_int_vector>::m_len;	// make m_len easy usable

    typename t_int_vector::value_type* m_word;

public:
    int_vector_iterator(t_int_vector* v = nullptr, size_type idx = 0)
        : int_vector_iterator_base<t_int_vector>(v, idx)
        , m_word((v != nullptr) ? v->m_data + (idx >> 6) : nullptr)
    {
    }

    int_vector_iterator(const int_vector_iterator<t_int_vector>& it)
        : int_vector_iterator_base<t_int_vector>(it), m_word(it.m_word)
    {
        m_offset = it.m_offset;
        m_len	= it.m_len;
    }

    reference operator*() const { return reference(m_word, m_offset, m_len); }

    //! Prefix increment of the Iterator
    iterator& operator++()
    {
        m_offset += m_len;
        if (m_offset >= 64) {
            m_offset &= 0x3F;
            ++m_word;
        }
        return *this;
    }

    //! Postfix increment of the Iterator
    iterator operator++(int)
    {
        int_vector_iterator it = *this;
        ++(*this);
        return it;
    }

    //! Prefix decrement of the Iterator
    iterator& operator--()
    {
        m_offset -= m_len;
        if (m_offset >= 64) {
            m_offset &= 0x3F;
            --m_word;
        }
        return *this;
    }

    //! Postfix decrement of the Iterator
    iterator operator--(int)
    {
        int_vector_iterator it = *this;
        --(*this);
        return it;
    }

    iterator& operator+=(difference_type i)
    {
        if (i < 0) return *this -= (-i);
        difference_type t = i * m_len;
        m_word += (t >> 6);
        if ((m_offset += (t & 0x3F)) & ~0x3F) { // if new offset is >= 64
            ++m_word;							// add one to the word
            m_offset &= 0x3F;					// offset = offset mod 64
        }
        return *this;
    }

    iterator& operator-=(difference_type i)
    {
        if (i < 0) return *this += (-i);
        difference_type t = i * m_len;
        m_word -= (t >> 6);
        if ((m_offset -= (t & 0x3F)) & ~0x3F) { // if new offset is < 0
            --m_word;
            m_offset &= 0x3F;
        }
        return *this;
    }

    iterator& operator=(const int_vector_iterator<t_int_vector>& it)
    {
        if (this != &it) {
            m_word   = it.m_word;
            m_offset = it.m_offset;
            m_len	= it.m_len;
        }
        return *this;
    }

    iterator operator+(difference_type i) const
    {
        iterator it = *this;
        return it += i;
    }

    iterator operator-(difference_type i) const
    {
        iterator it = *this;
        return it -= i;
    }

    reference operator[](difference_type i) const { return *(*this + i); }

    bool operator==(const int_vector_iterator& it) const noexcept
    {
        return it.m_word == m_word && it.m_offset == m_offset;
    }

    bool operator!=(const int_vector_iterator& it) const  noexcept{ return !(*this == it); }

    bool operator<(const int_vector_iterator& it) const noexcept
    {
        if (m_word == it.m_word) return m_offset < it.m_offset;
        return m_word < it.m_word;
    }

    bool operator>(const int_vector_iterator& it) const noexcept
    {
        if (m_word == it.m_word) return m_offset > it.m_offset;
        return m_word > it.m_word;
    }

    bool operator>=(const int_vector_iterator& it) const noexcept { return !(*this < it); }

    bool operator<=(const int_vector_iterator& it) const noexcept { return !(*this > it); }
    inline difference_type operator-(const int_vector_iterator& it) const noexcept
    {
        return (((m_word - it.m_word) << 6) + m_offset - it.m_offset) / m_len;
    }
};

//template<class t_int_vector>
//void swap(const int_vector_iterator<t_int_vector> &x, const int_vector_iterator<t_int_vector> &y){
//  x->swap(*y);
//}

template <class t_int_vector>
inline int_vector_iterator<t_int_vector>
operator+(typename int_vector_iterator<t_int_vector>::difference_type n,
          const int_vector_iterator<t_int_vector>&					  it)
{
    return it + n;
}

template <class t_int_vector>
class int_vector_const_iterator : public int_vector_iterator_base<t_int_vector> {
public:
    typedef typename t_int_vector::value_type		 const_reference;
    typedef const typename t_int_vector::value_type* pointer;
    typedef int_vector_const_iterator				 const_iterator;
    typedef typename t_int_vector::size_type		 size_type;
    typedef typename t_int_vector::difference_type   difference_type;

    template <class X>
    friend typename int_vector_const_iterator<X>::difference_type
    operator-(const int_vector_const_iterator<X>& x, const int_vector_const_iterator<X>& y);
    friend class int_vector_iterator<t_int_vector>;
    friend class int_vector_iterator_base<t_int_vector>;

private:
    using int_vector_iterator_base<t_int_vector>::m_offset; // make m_offset easy usable
    using int_vector_iterator_base<t_int_vector>::m_len;	// make m_len easy usable

    const typename t_int_vector::value_type* m_word;

public:
    int_vector_const_iterator(const t_int_vector* v = nullptr, size_type idx = 0)
        : int_vector_iterator_base<t_int_vector>(v, idx)
        , m_word((v != nullptr) ? v->m_data + (idx >> 6) : nullptr)
    {
    }

    int_vector_const_iterator(const int_vector_const_iterator& it)
        : int_vector_iterator_base<t_int_vector>(it), m_word(it.m_word)
    {
        m_offset = it.m_offset;
        m_len	= it.m_len;
    }

    int_vector_const_iterator(const int_vector_iterator<t_int_vector>& it) : m_word(it.m_word)
    {
        m_offset = it.m_offset;
        m_len	= it.m_len;
    }

    const_reference operator*() const
    {
        if (m_offset + m_len <= 64) {
            return ((*m_word) >> m_offset) & bits::lo_set[m_len];
        }
        return ((*m_word) >> m_offset) |
            ((*(m_word + 1) & bits::lo_set[(m_offset + m_len) & 0x3F]) << (64 - m_offset));
    }

    //! Prefix increment of the Iterator
    const_iterator& operator++()
    {
        m_offset += m_len;
        if (m_offset >= 64) {
            m_offset &= 0x3F;
            ++m_word;
        }
        return *this;
    }

    //! Postfix increment of the Iterator
    const_iterator operator++(int)
    {
        int_vector_const_iterator it = *this;
        ++(*this);
        return it;
    }

    //! Prefix decrement of the Iterator
    const_iterator& operator--()
    {
        m_offset -= m_len;
        if (m_offset >= 64) {
            m_offset &= 0x3F;
            --m_word;
        }
        return *this;
    }

    //! Postfix decrement of the Iterator
    const_iterator operator--(int)
    {
        int_vector_const_iterator it = *this;
        --(*this);
        return it;
    }

    const_iterator& operator+=(difference_type i)
    {
        if (i < 0) return *this -= (-i);
        difference_type t = i * m_len;
        m_word += (t >> 6);
        if ((m_offset += (t & 0x3F)) & ~0x3F) { // if new offset >= 64
            ++m_word;							// add one to the word
            m_offset &= 0x3F;					// offset = offset mod 64
        }
        return *this;
    }

    const_iterator& operator-=(difference_type i)
    {
        if (i < 0) return *this += (-i);
        difference_type t = i * m_len;
        m_word -= (t >> 6);
        if ((m_offset -= (t & 0x3F)) & ~0x3F) { // if new offset is < 0
            --m_word;
            m_offset &= 0x3F;
        }
        return *this;
    }

    const_iterator operator+(difference_type i) const
    {
        const_iterator it = *this;
        return it += i;
    }

    const_iterator operator-(difference_type i) const
    {
        const_iterator it = *this;
        return it -= i;
    }

    const_reference operator[](difference_type i) const { return *(*this + i); }

    bool operator==(const int_vector_const_iterator& it) const noexcept
    {
        return it.m_word == m_word && it.m_offset == m_offset;
    }

    bool operator!=(const int_vector_const_iterator& it) const noexcept { return !(*this == it); }

    bool operator<(const int_vector_const_iterator& it) const noexcept
    {
        if (m_word == it.m_word) return m_offset < it.m_offset;
        return m_word < it.m_word;
    }

    bool operator>(const int_vector_const_iterator& it) const noexcept
    {
        if (m_word == it.m_word) return m_offset > it.m_offset;
        return m_word > it.m_word;
    }

    bool operator>=(const int_vector_const_iterator& it) const noexcept { return !(*this < it); }

    bool operator<=(const int_vector_const_iterator& it) const noexcept { return !(*this > it); }
};

template <class t_int_vector>
inline typename int_vector_const_iterator<t_int_vector>::difference_type
operator-(const int_vector_const_iterator<t_int_vector>& x,
          const int_vector_const_iterator<t_int_vector>& y)
{
    return (((x.m_word - y.m_word) << 6) + x.m_offset - y.m_offset) / x.m_len;
}

template <class t_int_vector>
inline int_vector_const_iterator<t_int_vector>
operator+(typename int_vector_const_iterator<t_int_vector>::difference_type n,
          const int_vector_const_iterator<t_int_vector>&					it)
{
    return it + n;
}

template <class t_bv>
inline typename std::enable_if<std::is_same<typename t_bv::index_category, bv_tag>::value,
                               std::ostream&>::type
operator<<(std::ostream& os, const t_bv& bv)
{
    for (auto b : bv) {
        os << b;
    }
    return os;
}

// ==== int_vector implementation  ====

template <uint8_t t_width>
inline int_vector<t_width>::int_vector(size_type size, value_type default_value, uint8_t int_width)
    : m_size(0), m_capacity(0), m_data(nullptr), m_width(t_width)
{
    width(int_width);
    assign(size, default_value);
}

template <uint8_t t_width>
inline int_vector<t_width>::int_vector(int_vector&& v)
    : m_size(v.m_size), m_capacity(v.m_capacity), m_data(v.m_data), m_width(v.m_width)
{
    v.m_data = nullptr; // ownership of v.m_data now transfered
    v.m_size = 0;
    v.m_capacity = 0;
}

template <uint8_t t_width>
inline int_vector<t_width>::int_vector(const int_vector& v)
    : m_size(0), m_capacity(0), m_data(nullptr), m_width(v.m_width)
{
    width(v.m_width);
    resize(v.size());
    if (v.m_size > 0) {
        if (memcpy(m_data, v.data(), bit_data_size() << 3) == nullptr) {
            throw std::bad_alloc(); // LCOV_EXCL_LINE
        }
    }
}

template <uint8_t	t_width>
int_vector<t_width>& int_vector<t_width>::operator=(const int_vector& v)
{
    if (this != &v) { // if v is not the same object
        int_vector<t_width> tmp(v);
        *this = std::move(tmp);
    }
    return *this;
}

template <uint8_t	t_width>
int_vector<t_width>& int_vector<t_width>::operator=(int_vector&& v)
{
    if (this != &v) { // if v is not the same object
        memory_manager::clear(*this); // clear allocated memory
        m_size     = v.m_size;
        m_data     = v.m_data; // assign new memory
        m_width    = v.m_width;
        m_capacity = v.m_capacity;
        v.m_data     = nullptr;
        v.m_size     = 0;
        v.m_capacity = 0;
    }
    return *this;
}

// Destructor
template <uint8_t t_width>
int_vector<t_width>::~int_vector()
{
    memory_manager::clear(*this);
}

// sdsl::swap (to fulfill the container concept)
template <uint8_t t_width>
void swap(int_vector<t_width>& v1, int_vector<t_width>& v2) noexcept { std::swap(v1, v2); }

template <uint8_t t_width>
void int_vector<t_width>::bit_resize(const size_type size)
{
    if (size > m_capacity || m_data == nullptr) {
        memory_manager::resize(*this, size);
    }
    m_size = size;
}

template <uint8_t t_width>
void int_vector<t_width>::bit_resize(const size_type size, const value_type value)
{
    size_type old_size = m_size;
    bit_resize(size);
    auto it = begin() + old_size / m_width;
    util::set_to_value(*this, value, it);
}

template <uint8_t t_width>
auto int_vector<t_width>::get_int(size_type idx, const uint8_t len) const -> value_type
{
#ifdef SDSL_DEBUG
    if (idx + len > m_size) {
		throw std::out_of_range(
		"OUT_OF_RANGE_ERROR: int_vector::get_int(size_type, uint8_t); idx+len > size()!");
	}
	if (len > 64) {
		throw std::out_of_range(
		"OUT_OF_RANGE_ERROR: int_vector::get_int(size_type, uint8_t); len>64!");
	}
#endif
    return bits::read_int(m_data + (idx >> 6), idx & 0x3F, len);
}

template <uint8_t t_width>
inline void int_vector<t_width>::set_int(size_type idx, value_type x, const uint8_t len)
{
#ifdef SDSL_DEBUG
    if (idx + len > m_size) {
		throw std::out_of_range(
		"OUT_OF_RANGE_ERROR: int_vector::set_int(size_type, uint8_t); idx+len > size()!");
	}
	if (len > 64) {
		throw std::out_of_range(
		"OUT_OF_RANGE_ERROR: int_vector::set_int(size_type, uint8_t); len>64!");
	}
#endif
    bits::write_int(m_data + (idx >> 6), x, idx & 0x3F, len);
}

template <uint8_t t_width>
inline typename int_vector<t_width>::size_type int_vector<t_width>::size() const noexcept {
    return m_size / m_width;
}

// specialized size method for 64-bit integer vector
template <>
inline typename int_vector<64>::size_type int_vector<64>::size() const noexcept {
    return m_size>>6;
}

// specialized size method for 32-bit integer vector
template <>
inline typename int_vector<32>::size_type int_vector<32>::size() const noexcept {
    return m_size>>5;
}

// specialized size method for 64-bit integer vector
template <>
inline typename int_vector<16>::size_type int_vector<16>::size() const noexcept {
    return m_size>>4;
}

// specialized size method for 64-bit integer vector
template <>
inline typename int_vector<8>::size_type int_vector<8>::size() const noexcept {
    return m_size>>3;
}

// specialized size method for bit_vector
template <>
inline typename int_vector<1>::size_type int_vector<1>::size() const noexcept {
    return m_size;
}

template <uint8_t t_width>
inline typename int_vector<t_width>::size_type int_vector<t_width>::capacity() const noexcept {
    return m_capacity / m_width;
}

// specialized capacity method for 64-bit integer vector
template <>
inline typename int_vector<64>::size_type int_vector<64>::capacity() const noexcept {
    return m_capacity>>6;
}

// specialized capacity method for 32-bit integer vector
template <>
inline typename int_vector<32>::size_type int_vector<32>::capacity() const noexcept {
    return m_capacity>>5;
}

// specialized capacity method for 64-bit integer vector
template <>
inline typename int_vector<16>::size_type int_vector<16>::capacity() const noexcept {
    return m_capacity>>4;
}

// specialized capacity method for 64-bit integer vector
template <>
inline typename int_vector<8>::size_type int_vector<8>::capacity() const noexcept {
    return m_capacity>>3;
}

// specialized capacity method for bit_vector
template <>
inline typename int_vector<1>::size_type int_vector<1>::capacity() const noexcept {
    return m_capacity;
}

template <uint8_t t_width>
inline auto int_vector<t_width>::operator[](const size_type& idx) noexcept -> reference
{
    assert(idx < this->size());
    size_type i = idx * m_width;
    return reference(this->m_data + (i >> 6), i & 0x3F, m_width);
}

// specialized [] operator for 64 bit access.
template <>
inline auto int_vector<64>::operator[](const size_type& idx) noexcept -> reference
{
    assert(idx < this->size());
    return *(this->m_data + idx);
}

// specialized [] operator for 32 bit access.
template <>
inline auto int_vector<32>::operator[](const size_type& idx) noexcept -> reference
{
    assert(idx < this->size());
    return *(((uint32_t*)(this->m_data)) + idx);
}

// specialized [] operator for 16 bit access.
template <>
inline auto int_vector<16>::operator[](const size_type& idx) noexcept -> reference
{
    assert(idx < this->size());
    return *(((uint16_t*)(this->m_data)) + idx);
}

// specialized [] operator for 8 bit access.
template <>
inline auto int_vector<8>::operator[](const size_type& idx) noexcept -> reference
{
    assert(idx < this->size());
    return *(((uint8_t*)(this->m_data)) + idx);
}

template <uint8_t t_width>
inline auto int_vector<t_width>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return get_int(idx * t_width, t_width);
}

template <>
inline auto int_vector<0>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return get_int(idx * m_width, m_width);
}

template <>
inline auto int_vector<64>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return *(this->m_data + idx);
}

template <>
inline auto int_vector<32>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return *(((uint32_t*)this->m_data) + idx);
}

template <>
inline auto int_vector<16>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return *(((uint16_t*)this->m_data) + idx);
}

template <>
inline auto int_vector<8>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return *(((uint8_t*)this->m_data) + idx);
}

template <>
inline auto int_vector<1>::operator[](const size_type& idx) const noexcept -> const_reference
{
    assert(idx < this->size());
    return ((*(m_data + (idx >> 6))) >> (idx & 0x3F)) & 1;
}

template <uint8_t t_width>
bool int_vector<t_width>::operator<(const int_vector& v) const noexcept
{
    size_type min_size				  = size();
    if (min_size > v.size()) min_size = v.size();
    for (auto it = begin(), end = begin() + min_size, it_v = v.begin(); it != end; ++it, ++it_v) {
        if (*it == *it_v)
            continue;
        else
            return *it < *it_v;
    }
    return size() < v.size();
}

template <uint8_t t_width>
bool int_vector<t_width>::operator>(const int_vector& v) const noexcept
{
    size_type min_size				  = size();
    if (min_size > v.size()) min_size = v.size();
    for (auto it = begin(), end = begin() + min_size, it_v = v.begin(); it != end; ++it, ++it_v) {
        if (*it == *it_v)
            continue;
        else
            return *it > *it_v;
    }
    return size() > v.size();
}

template <uint8_t t_width>
bool int_vector<t_width>::operator<=(const int_vector& v) const noexcept
{
    return *this == v or *this < v;
}

template <uint8_t t_width>
bool int_vector<t_width>::operator>=(const int_vector& v) const noexcept
{
    return *this == v or *this > v;
}

template <uint8_t	t_width>
int_vector<t_width>& int_vector<t_width>::operator&=(const int_vector& v)
{
    assert(v.bit_size() == bit_size());
    for (uint64_t i = 0; i < bit_data_size(); ++i)
        m_data[i] &= v.m_data[i];
    return *this;
}

template <uint8_t	t_width>
int_vector<t_width>& int_vector<t_width>::operator|=(const int_vector& v)
{
    assert(bit_size() == v.bit_size());
    for (uint64_t i = 0; i < bit_data_size(); ++i)
        m_data[i] |= v.m_data[i];
    return *this;
}

template <uint8_t	t_width>
int_vector<t_width>& int_vector<t_width>::operator^=(const int_vector& v)
{
    assert(bit_size() == v.bit_size());
    for (uint64_t i = 0; i < bit_data_size(); ++i)
        m_data[i] ^= v.m_data[i];
    return *this;
}

template <uint8_t						t_width>
typename int_vector<t_width>::size_type int_vector<t_width>::write_data(std::ostream& out) const
{
    size_type written_bytes = 0;
    uint64_t* p				= m_data;
    size_type idx			= 0;
    while (idx + conf::SDSL_BLOCK_SIZE < bit_data_size()) {
        out.write((char*)p, conf::SDSL_BLOCK_SIZE * sizeof(uint64_t));
        written_bytes += conf::SDSL_BLOCK_SIZE * sizeof(uint64_t);
        p += conf::SDSL_BLOCK_SIZE;
        idx += conf::SDSL_BLOCK_SIZE;
    }
    out.write((char*)p, (bit_data_size() - idx) * sizeof(uint64_t));
    written_bytes += (bit_data_size() - idx) * sizeof(uint64_t);
    return written_bytes;
}

template <uint8_t t_width>
typename int_vector<t_width>::size_type
int_vector<t_width>::serialize(std::ostream& out, structure_tree_node* v, std::string name) const
{
    structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
    size_type			 written_bytes = int_vector<t_width>::write_header(m_size, m_width, out);
    written_bytes += write_data(out);
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}

template <uint8_t t_width>
void int_vector<t_width>::load(std::istream& in)
{
    size_type size;
    int_vector<t_width>::read_header(size, m_width, in);

    bit_resize(size);
    uint64_t* p   = m_data;
    size_type idx = 0;
    while (idx + conf::SDSL_BLOCK_SIZE < bit_data_size()) {
        in.read((char*)p, conf::SDSL_BLOCK_SIZE * sizeof(uint64_t));
        p += conf::SDSL_BLOCK_SIZE;
        idx += conf::SDSL_BLOCK_SIZE;
    }
    in.read((char*)p, (bit_data_size() - idx) * sizeof(uint64_t));
}

template <uint8_t t_width>
template <typename archive_t> inline
typename std::enable_if<cereal::traits::is_output_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
int_vector<t_width>::CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
{
    ar(CEREAL_NVP(cereal::make_size_tag(static_cast<int_width_type>(m_width))));
    ar(CEREAL_NVP(growth_factor));
    ar(CEREAL_NVP(cereal::make_size_tag(static_cast<size_type>(m_size))));
    ar(cereal::make_nvp("data", cereal::binary_data(m_data, bit_data_size() * sizeof(uint64_t))));
}

template <uint8_t t_width>
template <typename archive_t> inline
typename std::enable_if<!cereal::traits::is_output_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
int_vector<t_width>::CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
{
    ar(CEREAL_NVP(m_width));
    ar(CEREAL_NVP(growth_factor));
    ar(CEREAL_NVP(m_size));
    for(value_type const & v : *this)
        ar(v);
}

template <uint8_t t_width>
template <typename archive_t> inline
typename std::enable_if<cereal::traits::is_input_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
int_vector<t_width>::CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
{
    ar(CEREAL_NVP(cereal::make_size_tag(m_width)));
    ar(CEREAL_NVP(growth_factor));
    ar(CEREAL_NVP(cereal::make_size_tag(m_size)));
    resize(size());
    ar(cereal::make_nvp("data", cereal::binary_data(m_data, bit_data_size() * sizeof(uint64_t))));
}

template <uint8_t t_width>
template <typename archive_t> inline
typename std::enable_if<!cereal::traits::is_input_serializable<cereal::BinaryData<int_vector<t_width>>, archive_t>::value, void>::type
int_vector<t_width>::CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
{
    ar(CEREAL_NVP(m_width));
    width(width());
    ar(CEREAL_NVP(growth_factor));
    ar(CEREAL_NVP(m_size));
    resize(size());

    for (size_t i = 0; i<size(); ++i)
    {
        value_type tmp;
        ar(tmp);
        operator[](i) = tmp;
    }
}

} // end namespace sdsl

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file int_vector_buffer.hpp
    \brief int_vector_buffer.hpp contains the sdsl::int_vector_buffer class.
    \author Maike Zwerger, Timo Beller and Simon Gog
*/
#ifndef INCLUDED_INT_VECTOR_BUFFER
#define INCLUDED_INT_VECTOR_BUFFER

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file iterators.hpp
    \brief iterators.hpp contains an generic iterator for random access containers.
	\author Simon Gog
*/
#ifndef INCLUDED_SDSL_ITERATORS
#define INCLUDED_SDSL_ITERATORS

#include <iterator>

namespace sdsl {

//! Generic iterator for a random access container
/*! \tparam t_rac Type of random access container.
 */
template <class t_rac>
class random_access_const_iterator : public std::iterator<std::random_access_iterator_tag,
                                                          typename t_rac::value_type,
                                                          typename t_rac::difference_type> {
public:
    typedef const typename t_rac::value_type	const_reference;
    typedef typename t_rac::size_type			size_type;
    typedef random_access_const_iterator<t_rac> iterator;
    typedef typename t_rac::difference_type		difference_type;

private:
    const t_rac*			  m_rac; // pointer to the random access container
    typename t_rac::size_type m_idx;

    template <class t_RAC>
    friend typename random_access_const_iterator<t_RAC>::difference_type
    operator-(const random_access_const_iterator<t_RAC>& x,
              const random_access_const_iterator<t_RAC>& y);

public:
    //! Constructor
    random_access_const_iterator(const t_rac* rac, size_type idx = 0) : m_rac(rac), m_idx(idx) {}

    //! Dereference operator for the Iterator.
    const_reference operator*() const { return (*m_rac)[m_idx]; }

    //! Prefix increment of the Iterator.
    iterator& operator++()
    {
        ++m_idx;
        return *this;
    }

    //! Postfix increment of the Iterator.
    iterator operator++(int)
    {
        random_access_const_iterator it = *this;
        ++(*this);
        return it;
    }

    //! Prefix decrement of the Iterator.
    iterator& operator--()
    {
        --m_idx;
        return *this;
    }

    //! Postfix decrement of the Iterator.
    iterator operator--(int)
    {
        random_access_const_iterator it = *this;
        --(*this);
        return it;
    }

    iterator& operator+=(difference_type i)
    {
        if (i < 0) return *this -= (-i);
        m_idx += i;
        return *this;
    }

    iterator& operator-=(difference_type i)
    {
        if (i < 0) return *this += (-i);
        m_idx -= i;
        return *this;
    }

    iterator operator+(difference_type i) const
    {
        iterator it = *this;
        return it += i;
    }

    iterator operator-(difference_type i) const
    {
        iterator it = *this;
        return it -= i;
    }

    const_reference operator[](difference_type i) const { return *(*this + i); }

    bool operator==(const iterator& it) const { return it.m_rac == m_rac && it.m_idx == m_idx; }

    bool operator!=(const iterator& it) const { return !(*this == it); }

    bool operator<(const iterator& it) const { return m_idx < it.m_idx; }

    bool operator>(const iterator& it) const { return m_idx > it.m_idx; }

    bool operator>=(const iterator& it) const { return !(*this < it); }

    bool operator<=(const iterator& it) const { return !(*this > it); }
};

template <class t_rac>
inline typename random_access_const_iterator<t_rac>::difference_type
operator-(const random_access_const_iterator<t_rac>& x,
          const random_access_const_iterator<t_rac>& y)
{
    return (typename random_access_const_iterator<t_rac>::difference_type)x.m_idx -
        (typename random_access_const_iterator<t_rac>::difference_type)y.m_idx;
}

template <class t_rac>
inline random_access_const_iterator<t_rac>
operator+(typename random_access_const_iterator<t_rac>::difference_type n,
          const random_access_const_iterator<t_rac>&					it)
{
    return it + n;
}

template <typename t_F>
struct random_access_container {
    typedef int_vector<>::size_type								  size_type;
    typedef int_vector<>::difference_type						  difference_type;
    typedef typename std::result_of<t_F(size_type)>::type		  value_type;
    typedef random_access_const_iterator<random_access_container> iterator_type;

    t_F		  f;
    size_type m_size;

    random_access_container(){};
    random_access_container(t_F ff, size_type size) : f(ff), m_size(size) {}

    value_type operator[](size_type i) const { return f(i); }

    size_type size() const { return m_size; }

    iterator_type begin() const { return iterator_type(this, 0); }

    iterator_type end() const { return iterator_type(this, size()); }
};

} // end namespace sdsl
#endif

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

namespace sdsl {

template <uint8_t t_width = 0>
class int_vector_buffer {
public:
    class iterator;
    typedef typename int_vector<t_width>::difference_type difference_type;
    typedef typename int_vector<t_width>::value_type	  value_type;

private:
    static_assert(t_width <= 64, "int_vector_buffer: width must be at most 64 bits.");
    sdsl::isfstream		m_ifile;
    sdsl::osfstream		m_ofile;
    std::string			m_filename;
    int_vector<t_width> m_buffer;
    bool				m_need_to_write = false;
    // length of int_vector header in bytes: 0 for plain, 8 for int_vector<t_width> (0 < t_width), 9 for int_vector<0>
    uint64_t m_offset	 = 0;
    uint64_t m_buffersize = 8; // in elements! m_buffersize*width() must be a multiple of 8!
    uint64_t m_size		  = 0; // size of int_vector_buffer
    uint64_t m_begin	  = 0; // number in elements

    //! Read block containing element at index idx.
    void read_block(const uint64_t idx)
    {
        m_begin = (idx / m_buffersize) * m_buffersize;
        if (m_begin >= m_size) {
            util::set_to_value(m_buffer, 0);
        } else {
            m_ifile.seekg(m_offset + (m_begin * width()) / 8);
            assert(m_ifile.good());
            m_ifile.read((char*)m_buffer.data(), (m_buffersize * width()) / 8);
            if ((uint64_t)m_ifile.gcount() < (m_buffersize * width()) / 8) {
                m_ifile.clear();
            }
            assert(m_ifile.good());
            for (uint64_t i = m_size - m_begin; i < m_buffersize; ++i) {
                m_buffer[i] = 0;
            }
        }
    }

    //! Write current block to file.
    void write_block()
    {
        if (m_need_to_write) {
            m_ofile.seekp(m_offset + (m_begin * width()) / 8);
            assert(m_ofile.good());
            if (m_begin + m_buffersize >= m_size) {
                //last block in file
                uint64_t wb = ((m_size - m_begin) * width() + 7) / 8;
                m_ofile.write((char*)m_buffer.data(), wb);
            } else {
                m_ofile.write((char*)m_buffer.data(), (m_buffersize * width()) / 8);
            }
            m_ofile.flush();
            assert(m_ofile.good());
            m_need_to_write = false;
        }
    }

    //! Read value from idx.
    uint64_t read(const uint64_t idx)
    {
        assert(is_open());
        assert(idx < m_size);
        if (idx < m_begin or m_begin + m_buffersize <= idx) {
            write_block();
            read_block(idx);
        }
        return m_buffer[idx - m_begin];
    }

    //! Write value to idx.
    void write(const uint64_t idx, const uint64_t value)
    {
        assert(is_open());
        // If idx is not in current block, write current block and load needed block
        if (idx < m_begin or m_begin + m_buffersize <= idx) {
            write_block();
            read_block(idx);
        }
        if (m_size <= idx) {
            m_size = idx + 1;
        }
        m_need_to_write			= true;
        m_buffer[idx - m_begin] = value;
    }

public:
    //! Constructor.
    int_vector_buffer() { m_buffer = int_vector<t_width>(); }

    //! Constructor for int_vector_buffer.
    /*! \param filename   File that contains the data read from / written to.
         *  \param mode       Openmode:
         *                    std::ios::in opens an existing file (that must exist already),
         *                    std::ios::out creates a new file (that may exist already).
         *  \param buffersize Buffersize in bytes. This has to be a multiple of 8, if not the next multiple of 8 will be taken
         *  \param int_width  The width of each integer.
         *  \param is_plain   If false (default) the file will be interpreted as int_vector.
         *                    If true the file will be interpreted as plain array with t_width bits per integer.
         *                    In second case (is_plain==true), t_width must be 8, 16, 32 or 64.
         */
    int_vector_buffer(const std::string  filename,
                      std::ios::openmode mode		 = std::ios::in,
                      const uint64_t	 buffer_size = 1024 * 1024,
                      const uint8_t		 int_width   = t_width,
                      const bool		 is_plain	= false)
    {
        m_filename = filename;
        assert(!(mode & std::ios::app));
        mode &= ~std::ios::app;
        m_buffer.width(int_width);
        if (is_plain) {
            m_offset = 0;
            // is_plain is only allowed with width() in {8, 16, 32, 64}
            assert(8 == width() or 16 == width() or 32 == width() or 64 == width());
        } else {
            m_offset = 8; // TODO: make this dependent on header size of int_vector<t_width>
        }

        // Open file for IO
        m_ofile.open(m_filename, mode | std::ios::out | std::ios::binary);
        assert(m_ofile.good());
        m_ifile.open(m_filename, std::ios::in | std::ios::binary);
        assert(m_ifile.good());
        if (mode & std::ios::in) {
            uint64_t size = 0;
            if (is_plain) {
                m_ifile.seekg(0, std::ios_base::end);
                size = m_ifile.tellg() * 8;
            } else {
                uint8_t width = 0;
                int_vector<t_width>::read_header(size, width, m_ifile);
                m_buffer.width(width);
            }
            assert(m_ifile.good());
            m_size = size / width();
        }
        buffersize(buffer_size);
    }

    //! Move constructor.
    int_vector_buffer(int_vector_buffer&& ivb)
        : m_filename(std::move(ivb.m_filename))
        , m_buffer(std::move(ivb.m_buffer))
        , m_need_to_write(ivb.m_need_to_write)
        , m_offset(ivb.m_offset)
        , m_buffersize(ivb.m_buffersize)
        , m_size(ivb.m_size)
        , m_begin(ivb.m_begin)
    {
        ivb.m_ifile.close();
        ivb.m_ofile.close();
        m_ifile.open(m_filename, std::ios::in | std::ios::binary);
        m_ofile.open(m_filename, std::ios::in | std::ios::out | std::ios::binary);
        assert(m_ifile.good());
        assert(m_ofile.good());
        // set ivb to default-constructor state
        ivb.m_filename		= "";
        ivb.m_buffer		= int_vector<t_width>();
        ivb.m_need_to_write = false;
        ivb.m_offset		= 0;
        ivb.m_buffersize	= 8;
        ivb.m_size			= 0;
        ivb.m_begin			= 0;
    }

    //! Destructor.
    ~int_vector_buffer() { close(); }

    //! Move assignment operator.
    int_vector_buffer<t_width>& operator=(int_vector_buffer&& ivb)
    {
        close();
        ivb.m_ifile.close();
        ivb.m_ofile.close();
        m_filename = ivb.m_filename;
        m_ifile.open(m_filename, std::ios::in | std::ios::binary);
        m_ofile.open(m_filename, std::ios::in | std::ios::out | std::ios::binary);
        assert(m_ifile.good());
        assert(m_ofile.good());
        // assign the values of ivb to this
        m_buffer		= (int_vector<t_width> &&)ivb.m_buffer;
        m_need_to_write = ivb.m_need_to_write;
        m_offset		= ivb.m_offset;
        m_buffersize	= ivb.m_buffersize;
        m_size			= ivb.m_size;
        m_begin			= ivb.m_begin;
        // set ivb to default-constructor state
        ivb.m_filename		= "";
        ivb.m_buffer		= int_vector<t_width>();
        ivb.m_need_to_write = false;
        ivb.m_offset		= 0;
        ivb.m_buffersize	= 8;
        ivb.m_size			= 0;
        ivb.m_begin			= 0;
        return *this;
    }

    //! Returns the width of the integers which are accessed via the [] operator.
    uint8_t width() const { return m_buffer.width(); }

    //! Returns the number of elements currently stored.
    uint64_t size() const { return m_size; }

    //! Returns the filename.
    std::string filename() const { return m_filename; }

    //! Returns the buffersize in bytes
    uint64_t buffersize() const
    {
        assert(m_buffersize * width() % 8 == 0);
        return (m_buffersize * width()) / 8;
    }

    //! Set the buffersize in bytes
    void buffersize(uint64_t buffersize)
    {
        if (0ULL == buffersize) buffersize = 8;
        write_block();
        if (0 == (buffersize * 8) % width()) {
            m_buffersize =
                buffersize * 8 /
                    width(); // m_buffersize might not be multiple of 8, but m_buffersize*width() is.
        } else {
            uint64_t element_buffersize =
                (buffersize * 8) / width() +
                    1; // one more element than fits into given buffersize in byte
            m_buffersize =
                element_buffersize + 7 - (element_buffersize + 7) % 8; // take next multiple of 8
        }
        m_buffer = int_vector<t_width>(m_buffersize, 0, width());
        if (0 != m_buffersize) read_block(0);
    }

    //! Returns whether state of underlying streams are good
    bool good() { return m_ifile.good() and m_ofile.good(); }

    //! Returns whether underlying streams are currently associated to a file
    bool is_open()
    {
        return m_ifile.is_open() and m_ofile.is_open();
        ;
    }

    //! Delete all content and set size to 0
    void reset()
    {
        // reset file
        assert(m_ifile.good());
        assert(m_ofile.good());
        m_ifile.close();
        m_ofile.close();
        m_ofile.open(m_filename, std::ios::out | std::ios::binary);
        assert(m_ofile.good());
        m_ifile.open(m_filename, std::ios::in | std::ios::binary);
        assert(m_ifile.good());
        assert(m_ofile.good());
        // reset member variables
        m_need_to_write = false;
        m_size			= 0;
        // reset buffer
        read_block(0);
    }

    // Forward declaration
    class reference;

    //! [] operator
    /*! \param i Index the i-th integer of length width().
         *  \return A reference to the i-th integer of length width().
         */
    reference operator[](uint64_t idx) { return reference(this, idx); }

    //! Appends the given element value to the end of the int_vector_buffer
    void push_back(const uint64_t value) { write(m_size, value); }

    //! Close the int_vector_buffer.
    /*! It is not possible to read from / write into the int_vector_buffer after calling this method
         *  \param remove_file If true, the underlying file will be removed on closing.
         */
    void close(bool remove_file = false)
    {
        if (is_open()) {
            if (!remove_file) {
                write_block();
                if (0 < m_offset) { // in case of int_vector, write header and trailing zeros
                    uint64_t size = m_size * width();
                    m_ofile.seekp(0, std::ios::beg);
                    int_vector<t_width>::write_header(size, width(), m_ofile);
                    assert(m_ofile.good());
                    uint64_t wb = (size + 7) / 8;
                    if (wb % 8) {
                        m_ofile.seekp(m_offset + wb);
                        assert(m_ofile.good());
                        m_ofile.write("\0\0\0\0\0\0\0\0", 8 - wb % 8);
                        assert(m_ofile.good());
                    }
                }
            }
            m_ifile.close();
            assert(m_ifile.good());
            m_ofile.close();
            assert(m_ofile.good());
            if (remove_file) {
                sdsl::remove(m_filename);
            }
        }
    }

    iterator begin() { return iterator(*this, 0); }

    iterator end() { return iterator(*this, size()); }

    class reference {
        friend class int_vector_buffer<t_width>;

    private:
        int_vector_buffer<t_width>* const m_int_vector_buffer = nullptr;
        uint64_t						  m_idx				  = 0;

        reference() {}

        reference(int_vector_buffer<t_width>* _int_vector_buffer, uint64_t _idx)
            : m_int_vector_buffer(_int_vector_buffer), m_idx(_idx)
        {
        }

    public:
        //! Conversion to int for read operations
        operator uint64_t() const { return m_int_vector_buffer->read(m_idx); }

        //! Assignment operator for write operations
        reference& operator=(const uint64_t& val)
        {
            m_int_vector_buffer->write(m_idx, val);
            return *this;
        }

        //! Assignment operator
        reference& operator=(reference& x) { return *this = (uint64_t)(x); };

        //! Prefix increment of the proxy object
        reference& operator++()
        {
            uint64_t x = m_int_vector_buffer->read(m_idx);
            m_int_vector_buffer->write(m_idx, x + 1);
            return *this;
        }

        //! Postfix increment of the proxy object
        uint64_t operator++(int)
        {
            uint64_t val = (uint64_t) * this;
            ++(*this);
            return val;
        }

        //! Prefix decrement of the proxy object
        reference& operator--()
        {
            uint64_t x = m_int_vector_buffer->read(m_idx);
            m_int_vector_buffer->write(m_idx, x - 1);
            return *this;
        }

        //! Postfix decrement of the proxy object
        uint64_t operator--(int)
        {
            uint64_t val = (uint64_t) * this;
            --(*this);
            return val;
        }

        //! Add assign from the proxy object
        reference& operator+=(const uint64_t x)
        {
            uint64_t w = m_int_vector_buffer->read(m_idx);
            m_int_vector_buffer->write(m_idx, w + x);
            return *this;
        }

        //! Subtract assign from the proxy object
        reference& operator-=(const uint64_t x)
        {
            uint64_t w = m_int_vector_buffer->read(m_idx);
            m_int_vector_buffer->write(m_idx, w - x);
            return *this;
        }

        bool operator==(const reference& x) const { return (uint64_t) * this == (uint64_t)x; }

        bool operator<(const reference& x) const { return (uint64_t) * this < (uint64_t)x; }
    };

    class iterator : public std::iterator<std::random_access_iterator_tag,
                                          value_type,
                                          difference_type,
                                          value_type*,
                                          reference> {
    private:
        int_vector_buffer<t_width>& m_ivb;
        uint64_t					m_idx = 0;

    public:
        iterator() = delete;
        iterator(int_vector_buffer<t_width>& ivb, uint64_t idx = 0) : m_ivb(ivb), m_idx(idx) {}

        iterator& operator++()
        {
            ++m_idx;
            return *this;
        }

        iterator operator++(int)
        {
            iterator it = *this;
            ++(*this);
            return it;
        }

        iterator& operator--()
        {
            --m_idx;
            return *this;
        }

        iterator operator--(int)
        {
            iterator it = *this;
            --(*this);
            return it;
        }

        reference operator*() const { return m_ivb[m_idx]; }

        iterator& operator+=(difference_type i)
        {
            if (i < 0) return *this -= (-i);
            m_idx += i;
            return *this;
        }

        iterator& operator-=(difference_type i)
        {
            if (i < 0) return *this += (-i);
            m_idx -= i;
            return *this;
        }

        iterator operator+(difference_type i) const
        {
            iterator it = *this;
            return it += i;
        }

        iterator operator-(difference_type i) const
        {
            iterator it = *this;
            return it -= i;
        }

        bool operator==(const iterator& it) const
        {
            return &m_ivb == &(it.m_ivb) and m_idx == it.m_idx;
        }

        bool operator!=(const iterator& it) const { return !(*this == it); }
        inline difference_type operator-(const iterator& it) { return (m_idx - it.m_idx); }
    };
};

} // end of namespace

#endif // include guard

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
#ifndef SDSL_INT_VECTOR_MAPPER
#define SDSL_INT_VECTOR_MAPPER

#include <inttypes.h>
#include <cstdio>
#include <ios>

namespace sdsl {

template <uint8_t				  t_width = 0,
    std::ios_base::openmode t_mode  = std::ios_base::out | std::ios_base::in>
class int_vector_mapper {
    static_assert(t_width <= 64, "int_vector_mapper: width must be at most 64 bits.");

public:
    typedef typename int_vector<t_width>::difference_type difference_type;
    typedef typename int_vector<t_width>::value_type	  value_type;
    typedef typename int_vector<t_width>::size_type		  size_type;
    typedef typename int_vector<t_width>::int_width_type  width_type;
    static constexpr uint8_t							  fixed_int_width = t_width;

public:
    const size_type append_block_size = 1000000;

private:
    uint8_t*			m_mapped_data	 = nullptr;
    uint64_t			m_file_size_bytes = 0;
    off_t				m_data_offset	 = 0;
    int					m_fd			  = -1;
    int_vector<t_width> m_wrapper;
    std::string			m_file_name;
    bool				m_delete_on_close;

public:
    int_vector_mapper()							= delete;
    int_vector_mapper(const int_vector_mapper&) = delete;
    int_vector_mapper& operator=(const int_vector_mapper&) = delete;

public:
    ~int_vector_mapper()
    {
        if (m_mapped_data) {
            auto ret = memory_manager::mem_unmap(m_fd, m_mapped_data, m_file_size_bytes);
            if (ret != 0) {
                std::cerr << "int_vector_mapper: error unmapping file mapping'" << m_file_name
                          << "': " << ret << std::endl;
            }

            if (t_mode & std::ios_base::out) { // write was possible
                if (m_data_offset) {		   // if the file is not a plain file
                    // set std::ios::in to not truncate the file
                    osfstream out(m_file_name, std::ios::in);
                    if (out) {
                        out.seekp(0, std::ios::beg);
                        int_vector<t_width>::write_header(m_wrapper.m_size, m_wrapper.m_width, out);

                        //    out.seekp(0, std::ios::end);
                    } else {
                        std::cerr << "int_vector_mapper: could not open file for header update" << std::endl;
/*
						throw std::runtime_error("int_vector_mapper: \
                                    could not open file for header update");
*/
                    }
                }
            }

            if (t_mode & std::ios_base::out) {
                // do we have to truncate?
                size_type current_bit_size   = m_wrapper.m_size;
                size_type data_size_in_bytes = ((current_bit_size + 63) >> 6) << 3;
                if (m_file_size_bytes != data_size_in_bytes + m_data_offset) {
                    int tret =
                        memory_manager::truncate_file_mmap(m_fd, data_size_in_bytes + m_data_offset);
                    if (tret == -1) {
                        std::string truncate_error =
                            std::string("int_vector_mapper: truncate error. ") +
                                std::string(util::str_from_errno());
                        std::cerr << truncate_error;
                    }
                }
            }
        }
        if (m_fd != -1) {
            auto ret = memory_manager::close_file_for_mmap(m_fd);
            if (ret != 0) {
                std::cerr << "int_vector_mapper: error closing file mapping'" << m_file_name
                          << "': " << ret << std::endl;
            }
            if (m_delete_on_close) {
                int ret_code = sdsl::remove(m_file_name);
                if (ret_code != 0) {
                    std::cerr << "int_vector_mapper: error deleting file '" << m_file_name
                              << "': " << ret_code << std::endl;
                }
            }
        }
        m_wrapper.m_data = nullptr;
        m_wrapper.m_size = 0;
    }

    int_vector_mapper(int_vector_mapper&& ivm)
    {
        m_wrapper.m_data = ivm.m_wrapper.m_data;
        m_wrapper.m_size = ivm.m_wrapper.m_size;
        m_wrapper.width(ivm.m_wrapper.width());
        m_file_name			 = ivm.m_file_name;
        m_delete_on_close	= ivm.m_delete_on_close;
        ivm.m_wrapper.m_data = nullptr;
        ivm.m_wrapper.m_size = 0;
        ivm.m_mapped_data	= nullptr;
        ivm.m_fd			 = -1;
    }

    int_vector_mapper& operator=(int_vector_mapper&& ivm)
    {
        m_wrapper.m_data = ivm.m_wrapper.m_data;
        m_wrapper.m_size = ivm.m_wrapper.m_size;
        m_wrapper.width(ivm.m_wrapper.width());
        m_file_name			 = ivm.m_file_name;
        m_delete_on_close	= ivm.m_delete_on_close;
        ivm.m_wrapper.m_data = nullptr;
        ivm.m_wrapper.m_size = 0;
        ivm.m_mapped_data	= nullptr;
        ivm.m_fd			 = -1;
        return (*this);
    }

    int_vector_mapper(const std::string& key, const cache_config& config)
        : int_vector_mapper(cache_file_name(key, config))
    {
    }

    int_vector_mapper(const std::string filename,
                      bool				is_plain		= false,
                      bool				delete_on_close = false)
        : m_data_offset(0), m_file_name(filename), m_delete_on_close(delete_on_close)
    {
        size_type size_in_bits = 0;
        uint8_t   int_width	= t_width;
        {
            isfstream f(filename, std::ifstream::binary);
            if (!f.is_open()) {
                throw std::runtime_error("int_vector_mapper: file " + m_file_name +
                    " does not exist.");
            }
            if (!is_plain) {
                m_data_offset = int_vector<t_width>::read_header(size_in_bits, int_width, f);
            }
        }

        m_file_size_bytes = util::file_size(m_file_name);

        if (is_plain) {
            if (8 != t_width and 16 != t_width and 32 != t_width and 64 != t_width) {
                throw std::runtime_error("int_vector_mapper: plain vector can "
                                         "only be of width 8, 16, 32, 64.");
            } else {
                uint8_t byte_width = t_width / 8;
                // if( m_file_size_bytes % (t_width/8) != 0)
                if ((m_file_size_bytes & bits::lo_set[bits::cnt(byte_width - 1)]) != 0) {
                    throw std::runtime_error(
                        "int_vector_mapper: plain vector not a multiple of byte: " +
                            std::to_string(m_file_size_bytes) + " mod " + std::to_string(byte_width) +
                            " != 0");
                }
            }
            size_in_bits = m_file_size_bytes * 8;
        }

        // open backend file depending on mode
        m_fd = memory_manager::open_file_for_mmap(m_file_name, t_mode);
        if (m_fd == -1) {
            std::string open_error = std::string("int_vector_mapper: open file error.") +
                std::string(util::str_from_errno());
            throw std::runtime_error(open_error);
        }

        // prepare for mmap
        m_wrapper.width(int_width);
        // mmap data
        m_mapped_data = (uint8_t*)memory_manager::mmap_file(m_fd, m_file_size_bytes, t_mode);
        if (m_mapped_data == nullptr) {
            std::string mmap_error =
                std::string("int_vector_mapper: mmap error. ") + std::string(util::str_from_errno());
            throw std::runtime_error(mmap_error);
        }

        m_wrapper.m_size = size_in_bits;
        free(m_wrapper.m_data);
        m_wrapper.m_data = (uint64_t*)(m_mapped_data + m_data_offset);
    }

    std::string file_name() const { return m_file_name; }
    width_type  width() const { return m_wrapper.width(); }
    void width(const uint8_t new_int_width)
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'width'");
        m_wrapper.width(new_int_width);
    }
    size_type size() const { return m_wrapper.size(); }
    void bit_resize(const size_type bit_size)
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'bit_resize'");
        size_type new_size_in_bytes = ((bit_size + 63) >> 6) << 3;
        if (m_file_size_bytes != new_size_in_bytes + m_data_offset) {
            if (m_mapped_data) {
                auto ret = memory_manager::mem_unmap(m_fd, m_mapped_data, m_file_size_bytes);
                if (ret != 0) {
                    std::cerr << "int_vector_mapper: error unmapping file mapping'" << m_file_name
                              << "': " << ret << std::endl;
                }
            }
            int tret = memory_manager::truncate_file_mmap(m_fd, new_size_in_bytes + m_data_offset);
            if (tret == -1) {
                std::string truncate_error = std::string("int_vector_mapper: truncate error. ") +
                    std::string(util::str_from_errno());
                throw std::runtime_error(truncate_error);
            }
            m_file_size_bytes = new_size_in_bytes + m_data_offset;

            // perform the actual mapping
            m_mapped_data = (uint8_t*)memory_manager::mmap_file(m_fd, m_file_size_bytes, t_mode);
            if (m_mapped_data == nullptr) {
                std::string mmap_error = std::string("int_vector_mapper: mmap error. ") +
                    std::string(util::str_from_errno());
                throw std::runtime_error(mmap_error);
            }

            // update wrapper
            m_wrapper.m_data = (uint64_t*)(m_mapped_data + m_data_offset);
        }
        m_wrapper.m_size = bit_size;
    }

    void resize(const size_type size)
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'resize'");
        size_type size_in_bits = size * width();
        bit_resize(size_in_bits);
    }

    auto begin() -> typename int_vector<t_width>::iterator
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'begin'");
        return m_wrapper.begin();
    }
    auto end() -> typename int_vector<t_width>::iterator
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'end'");
        return m_wrapper.end();
    }
    auto begin() const -> typename int_vector<t_width>::const_iterator { return m_wrapper.begin(); }
    auto end() const -> typename int_vector<t_width>::const_iterator { return m_wrapper.end(); }
    auto cbegin() const -> typename int_vector<t_width>::const_iterator
    {
        return m_wrapper.begin();
    }
    auto cend() const -> typename int_vector<t_width>::const_iterator { return m_wrapper.end(); }
    auto operator[](const size_type& idx) const -> typename int_vector<t_width>::const_reference
    {
        return m_wrapper[idx];
    }
    auto operator[](const size_type& idx) -> typename int_vector<t_width>::reference
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'operator[]'");
        return m_wrapper[idx];
    }
    const uint64_t* data() const { return m_wrapper.data(); }
    uint64_t*		data()
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'data'");
        return m_wrapper.data();
    }
    value_type get_int(size_type idx, const uint8_t len = 64) const
    {
        return m_wrapper.get_int(idx, len);
    }
    void set_int(size_type idx, value_type x, const uint8_t len = 64)
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'set_int'");
        m_wrapper.set_int(idx, x, len);
    }
    void push_back(value_type x)
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'push_back'");
        if (capacity() < size() + 1) {
            size_type old_size	 = m_wrapper.m_size;
            size_type size_in_bits = (size() + append_block_size) * width();
            bit_resize(size_in_bits);
            m_wrapper.m_size = old_size;
        }
        // update size in wrapper only
        m_wrapper.m_size += width();
        m_wrapper[size() - 1] = x;
    }
    size_type capacity() const
    {
        size_t data_size_in_bits = 8 * (m_file_size_bytes - m_data_offset);
        return data_size_in_bits / width();
    }
    size_type bit_size() const { return m_wrapper.bit_size(); }
    template <class container>
    bool operator==(const container& v) const
    {
        return std::equal(begin(), end(), v.begin());
    }
    bool operator==(const int_vector<t_width>& v) const { return m_wrapper == v; }
    bool operator==(const int_vector_mapper& v) const { return m_wrapper == v.m_wrapper; }
    template <class container>
    bool operator!=(const container& v) const
    {
        return !(*this == v);
    }
    void flip()
    {
        static_assert(t_mode & std::ios_base::out,
                      "int_vector_mapper: must be opened in in+out mode for 'flip'");
        m_wrapper.flip();
    }
    bool empty() const { return m_wrapper.empty(); }
};

template <uint8_t t_width = 0>
class temp_file_buffer {
private:
    static std::string tmp_file(const std::string& dir)
    {
        char tmp_file_name[1024] = {0};
#ifdef _WIN32
        auto ret = GetTempFileName(dir.c_str(), "tmp_mapper_file_", 0, tmp_file_name);
		if (ret == 0) {
			throw std::runtime_error("could not create temporary file.");
		}
#else
        sprintf(tmp_file_name, "%s/tmp_mapper_file_%" PRIu64 "_XXXXXX.sdsl", dir.c_str(), util::pid());
        int fd = mkstemps(tmp_file_name, 5);
        if (fd == -1) {
            throw std::runtime_error("could not create temporary file.");
        }
        close(fd);
#endif
        return std::string(tmp_file_name, strlen(tmp_file_name));
    }

public:
    static int_vector_mapper<t_width> create()
    {
#ifdef MSVC_COMPILER
        char tmp_dir_name[1024] = {0};
		auto tmp_dir			= GetTempPath(1024, tmp_dir_name);
		auto file_name			= tmp_file(tmp_dir_name);
#else
        auto file_name = tmp_file("/tmp");
#endif
        return create(file_name);
    }
    static int_vector_mapper<t_width> create(const cache_config& config)
    {
        auto file_name = tmp_file(config.dir);
        return create(file_name);
    }
    static int_vector_mapper<t_width> create(const std::string& file_name)
    {
        //write empty int_vector to init the file
        int_vector<t_width> tmp_vector;
        store_to_file(tmp_vector, file_name);
        return int_vector_mapper<t_width, std::ios_base::out | std::ios_base::in>(
            file_name, false, true);
    }
};

// creates emtpy int_vector<> that will not be deleted
template <uint8_t t_width = 0>
class write_out_mapper {
public:
    static int_vector_mapper<t_width> create(const std::string& key, cache_config& config)
    {
        auto file_name = cache_file_name(key, config);
        auto tmp	   = create(file_name);
        register_cache_file(key, config);
        return std::move(tmp);
    }
    static int_vector_mapper<t_width> create(const std::string& file_name)
    {
        //write empty int_vector to init the file
        int_vector<t_width> tmp_vector;
        store_to_file(tmp_vector, file_name);
        return int_vector_mapper<t_width, std::ios_base::out | std::ios_base::in>(
            file_name, false, false);
    }
    static int_vector_mapper<t_width>
    create(const std::string& file_name, size_t size, uint8_t int_width = t_width)
    {
        //write empty int_vector to init the file
        int_vector<t_width> tmp_vector(0, 0, int_width);
        store_to_file(tmp_vector, file_name);
        int_vector_mapper<t_width, std::ios_base::out | std::ios_base::in> mapper(
            file_name, false, false);
        mapper.resize(size);
        return mapper;
    }
};

template <std::ios_base::openmode t_mode = std::ios_base::out | std::ios_base::in>
using bit_vector_mapper					 = int_vector_mapper<1, t_mode>;

template <uint8_t t_width = 0>
using read_only_mapper	= const int_vector_mapper<t_width, std::ios_base::in>;

} // end of namespace

#endif

#endif

// Copyright (c) 2016, the SDSL Project Authors and Jouni Siren.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*!\file sd_vector.hpp
   \brief sd_vector.hpp contains the sdsl::sd_vector class, and
          classes which support rank and select for sd_vector.
   \author Simon Gog, Jouni Siren
*/
#ifndef INCLUDED_SDSL_SD_VECTOR
#define INCLUDED_SDSL_SD_VECTOR

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file select_support_mcl.hpp
    \brief select_support_mcl.hpp contains classes that support a sdsl::bit_vector with constant time select information.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_SELECT_SUPPORT_MCL
#define INCLUDED_SDSL_SELECT_SUPPORT_MCL

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file select_support.hpp
    \brief select_support.hpp contains classes that support a sdsl::bit_vector with constant time select information.
	\author Simon Gog
*/
#ifndef INCLUDED_SDSL_SELECT_SUPPORT
#define INCLUDED_SDSL_SELECT_SUPPORT

/** \defgroup select_support_group Select Support (SCS)
 * This group contains data structures which support an sdsl::bit_vector with the select method.
 */

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file rank_support.hpp
    \brief rank_support.hpp contains classes that support a sdsl::bit_vector with constant time rank information.
	\author Simon Gog
*/
#ifndef INCLUDED_SDSL_RANK_SUPPORT
#define INCLUDED_SDSL_RANK_SUPPORT

/** \defgroup rank_support_group Rank Support (RS)
 * This group contains data structures which support an sdsl::bit_vector with the rank method.
 */

//! Namespace for the succinct data structure library.
namespace sdsl {

//! The base class of classes supporting rank_queries for a sdsl::bit_vector in constant time.
/*!
*/
class rank_support {
protected:
    const bit_vector* m_v; //!< Pointer to the rank supported bit_vector
public:
    typedef bit_vector::size_type size_type;

    //! Constructor
    /*! \param v The supported bit_vector.
         */
    rank_support(const bit_vector* v = nullptr);
    //! Copy constructor
    rank_support(const rank_support&) = default;
    rank_support(rank_support&&)	  = default;
    rank_support& operator=(const rank_support&) = default;
    rank_support& operator=(rank_support&&) = default;
    //! Destructor
    virtual ~rank_support() {}

    //! Answers rank queries for the supported bit_vector.
    /*!	\param i Argument for the length of the prefix v[0..i-1].
        	\returns Number of 1-bits in the prefix [0..i-1] of the supported bit_vector.
        	\note Method init has to be called before the first call of rank.
        	\sa init
         */
    virtual size_type rank(size_type i) const = 0;
    //! Alias for rank(i)
    virtual size_type operator()(size_type idx) const = 0;
    //! Serializes rank_support.
    /*! \param out Out-Stream to serialize the data to.
        */
    virtual size_type
    serialize(std::ostream& out, structure_tree_node* v, std::string name) const = 0;
    //! Loads the rank_support.
    /*! \param in In-Stream to load the rank_support data from.
            \param v The supported bit_vector.
         */
    virtual void load(std::istream& in, const bit_vector* v = nullptr) = 0;
    //! Sets the supported bit_vector to the given pointer.
    /*! \param v The new bit_vector to support.
         *  \note Method init has to be called before the next call of rank.
         *  \sa init, rank
         */
    virtual void set_vector(const bit_vector* v = nullptr) = 0;
};

inline rank_support::rank_support(const bit_vector* v) { m_v = v; }

//----------------------------------------------------------------------

template <uint8_t bit_pattern, uint8_t pattern_len>
struct rank_support_trait {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t, uint64_t&) { return 0; }

    static uint32_t word_rank(const uint64_t*, size_type) { return 0; }

    static uint32_t full_word_rank(const uint64_t*, size_type) { return 0; }

    static uint64_t init_carry() { return 0; }
};

template <>
struct rank_support_trait<0, 1> {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t w, uint64_t&) { return bits::cnt(~w); }

    static uint32_t word_rank(const uint64_t* data, size_type idx)
    {
        return bits::cnt((~*(data + (idx >> 6))) & bits::lo_set[idx & 0x3F]);
    }

    static uint32_t full_word_rank(const uint64_t* data, size_type idx)
    {
        return bits::cnt((~*(data + (idx >> 6))));
    }

    static uint64_t init_carry() { return 0; }
};

template <>
struct rank_support_trait<1, 1> {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t w, uint64_t&) { return bits::cnt(w); }

    static uint32_t word_rank(const uint64_t* data, size_type idx)
    {
        return bits::cnt(*(data + (idx >> 6)) & bits::lo_set[idx & 0x3F]);
    }

    static uint32_t full_word_rank(const uint64_t* data, size_type idx)
    {
        return bits::cnt(*(data + (idx >> 6)));
    }

    static uint64_t init_carry() { return 0; }
};

template <>
struct rank_support_trait<10, 2> {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t w, uint64_t& carry) { return bits::cnt10(w, carry); }

    static uint32_t word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 0;
        return bits::cnt(bits::map10(*data, carry) & bits::lo_set[idx & 0x3F]);
    }

    static uint32_t full_word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 0;
        return bits::cnt(bits::map10(*data, carry));
    }

    static uint64_t init_carry() { return 0; }
};

template <>
struct rank_support_trait<01, 2> {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t w, uint64_t& carry) { return bits::cnt01(w, carry); }

    static uint32_t word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 1;
        return bits::cnt(bits::map01(*data, carry) & bits::lo_set[idx & 0x3F]);
    }

    static uint32_t full_word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 1;
        return bits::cnt(bits::map01(*data, carry));
    }

    static uint64_t init_carry() { return 1; }
};

template <>
struct rank_support_trait<00, 2> {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t w, uint64_t& carry)
    {
        size_type res = bits::cnt(~(w | (w << 1 | carry)));
        carry		  = (w >> 63);
        return res;
    }

    static uint32_t word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 1;
        return bits::cnt((~(*data | ((*data) << 1 | carry))) & bits::lo_set[idx & 0x3F]);
    }

    static uint32_t full_word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 1;
        return bits::cnt(~(*data | ((*data) << 1 | carry)));
    }

    static uint64_t init_carry() { return 1; }
};

template <>
struct rank_support_trait<11, 2> {
    typedef rank_support::size_type size_type;

    static size_type args_in_the_word(uint64_t w, uint64_t& carry)
    {
        size_type res = bits::cnt(w & (w << 1 | carry));
        carry		  = (w >> 63);
        return res;
    }

    static uint32_t word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 0;
        return bits::cnt((*data & ((*data) << 1 | carry)) & bits::lo_set[idx & 0x3F]);
    }

    static uint32_t full_word_rank(const uint64_t* data, size_type idx)
    {
        data		   = data + (idx >> 6);
        uint64_t carry = (idx > 63) ? *(data - 1) >> 63 : 0;
        return bits::cnt(*data & ((*data) << 1 | carry));
    }

    static uint64_t init_carry() { return 0; }
};

} // end namespace sdsl

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file rank_support_v.hpp
    \brief rank_support_v.hpp contains rank_support_v.
	\author Simon Gog
*/
#ifndef INCLUDED_SDSL_RANK_SUPPORT_V
#define INCLUDED_SDSL_RANK_SUPPORT_V

//! Namespace for the succinct data structure library.
namespace sdsl {

//! A rank structure proposed by Sebastiano Vigna
/*! \par Space complexity
 *  \f$ 0.25n\f$ for a bit vector of length n bits.
 *
 * The superblock size is 512. Each superblock is subdivided into 512/64 = 8
 * blocks. So absolute counts for the superblock add 64/512 bits on top of each
 * supported bit. Since the first of the 8 relative count values is 0, we can
 * fit the remaining 7 (each of width log(512)=9) in a 64bit word. The relative
 * counts add another 64/512 bits on top of each supported bit.
 * In total this results in 128/512=25% overhead.
 *
 * \tparam t_b       Bit pattern `0`,`1`,`10`,`01` which should be ranked.
 * \tparam t_pat_len Length of the bit pattern.
 *
 * \par Reference
 *    Sebastiano Vigna:
 *    Broadword Implementation of Rank/Select Queries.
 *    WEA 2008: 154-168
 *
 * @ingroup rank_support_group
 */
template <uint8_t t_b = 1, uint8_t t_pat_len = 1>
class rank_support_v : public rank_support {
private:
    static_assert(t_b == 1u or t_b == 0u or t_b == 10u or t_b == 11,
                  "rank_support_v: bit pattern must be `0`,`1`,`10` or `01`");
    static_assert(t_pat_len == 1u or t_pat_len == 2u,
                  "rank_support_v: bit pattern length must be 1 or 2");

public:
    typedef bit_vector bit_vector_type;
    typedef rank_support_trait<t_b, t_pat_len> trait_type;
    enum { bit_pat = t_b };
    enum { bit_pat_len = t_pat_len };

private:
    // basic block for interleaved storage of superblockrank and blockrank
    int_vector<64> m_basic_block;

public:
    explicit rank_support_v(const bit_vector* v = nullptr)
    {
        set_vector(v);
        if (v == nullptr) {
            return;
        } else if (v->empty()) {
            m_basic_block = int_vector<64>(2, 0); // resize structure for basic_blocks
            return;
        }
        size_type basic_block_size = (((v->bit_size() + 63) >> 9) + 1) << 1;
        m_basic_block.resize(basic_block_size); // resize structure for basic_blocks
        if (m_basic_block.empty()) return;
        const uint64_t* data = m_v->data();
        size_type		i, j = 0;
        m_basic_block[0] = m_basic_block[1] = 0;

        uint64_t carry			  = trait_type::init_carry();
        uint64_t sum			  = trait_type::args_in_the_word(*data, carry);
        uint64_t second_level_cnt = 0;
        for (i = 1; i < ((m_v->bit_size() + 63) >> 6); ++i) {
            if (!(i & 0x7)) { // if i%8==0
                j += 2;
                m_basic_block[j - 1] = second_level_cnt;
                m_basic_block[j]	 = m_basic_block[j - 2] + sum;
                second_level_cnt = sum = 0;
            } else {
                second_level_cnt |= sum << (63 - 9 * (i & 0x7)); //  54, 45, 36, 27, 18, 9, 0
            }
            sum += trait_type::args_in_the_word(*(++data), carry);
        }
        if (i & 0x7) { // if i%8 != 0
            second_level_cnt |= sum << (63 - 9 * (i & 0x7));
            m_basic_block[j + 1] = second_level_cnt;
        } else { // if i%8 == 0
            j += 2;
            m_basic_block[j - 1] = second_level_cnt;
            m_basic_block[j]	 = m_basic_block[j - 2] + sum;
            m_basic_block[j + 1] = 0;
        }
    }

    rank_support_v(const rank_support_v&) = default;
    rank_support_v(rank_support_v&&)	  = default;
    rank_support_v& operator=(const rank_support_v&) = default;
    rank_support_v& operator=(rank_support_v&&) = default;

    size_type rank(size_type idx) const
    {
        assert(m_v != nullptr);
        assert(idx <= m_v->size());
        const uint64_t* p =
            m_basic_block.data() + ((idx >> 8) & 0xFFFFFFFFFFFFFFFEULL); // (idx/512)*2
        if (idx & 0x3F)												 // if (idx%64)!=0
            return *p + ((*(p + 1) >> (63 - 9 * ((idx & 0x1FF) >> 6))) & 0x1FF) +
                trait_type::word_rank(m_v->data(), idx);
        else
            return *p + ((*(p + 1) >> (63 - 9 * ((idx & 0x1FF) >> 6))) & 0x1FF);
    }

    inline size_type operator()(size_type idx) const { return rank(idx); }

    size_type size() const { return m_v->size(); }

    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        size_type			 written_bytes = 0;
        structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
        written_bytes += m_basic_block.serialize(out, child, "cumulative_counts");
        structure_tree::add_size(child, written_bytes);
        return written_bytes;
    }

    void load(std::istream& in, const int_vector<1>* v = nullptr)
    {
        set_vector(v);
        m_basic_block.load(in);
    }

    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
    {
        ar(CEREAL_NVP(m_basic_block));
    }

    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
    {
        ar(CEREAL_NVP(m_basic_block));
    }

    bool operator==(const rank_support_v& other) const noexcept
    {
        return m_basic_block == other.m_basic_block;
    }

    bool operator!=(const rank_support_v& other) const noexcept
    {
        return !(*this == other);
    }

    void set_vector(const bit_vector* v = nullptr) { m_v = v; }
};

} // end namespace sds

#endif // end file

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file rank_support_v5.hpp
    \brief rank_support_v5.hpp contains rank_support_v5.5
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_RANK_SUPPORT_VFIVE
#define INCLUDED_SDSL_RANK_SUPPORT_VFIVE

//! Namespace for the succinct data structure library.
namespace sdsl {

template <uint8_t, uint8_t>
struct rank_support_trait;

//! A class supporting rank queries in constant time.
/*! \par Space complexity
 *  \f$ 0.0625n\f$ bits for a bit vector of length n bits.
 *
 * The superblock size is 2048. Each superblock is subdivided into
 * 2048/(6*64) = 5 blocks (with some bit remaining). So absolute counts for
 * the superblock add 64/2048 bits on top of each supported bit. Since the
 * first of the 6 relative count values is 0, we can fit the remaining 5
 * (each of width log(2048)=11) in a 64 bit word. The relative counts add
 * another 64/2048 bits bits on top of each supported bit. In total this
 * results in 128/2048= 6.25% overhead.
 *
 * \tparam t_b       Bit pattern `0`,`1`,`10`,`01` which should be ranked.
 * \tparam t_pat_len Length of the bit pattern.
 *
 * @ingroup rank_support_group
 */
template <uint8_t t_b = 1, uint8_t t_pat_len = 1>
class rank_support_v5 : public rank_support {
private:
    static_assert(t_b == 1u or t_b == 0u or t_b == 10u or t_b == 11u,
                  "rank_support_v5: bit pattern must be `0`,`1`,`10` or `01` or `11`");
    static_assert(t_pat_len == 1u or t_pat_len == 2u,
                  "rank_support_v5: bit pattern length must be 1 or 2");

public:
    typedef bit_vector bit_vector_type;
    typedef rank_support_trait<t_b, t_pat_len> trait_type;
    enum { bit_pat = t_b };
    enum { bit_pat_len = t_pat_len };

private:
    //      basic block for interleaved storage of superblockrank and blockrank
    int_vector<64> m_basic_block;

public:
    explicit rank_support_v5(const bit_vector* v = nullptr)
    {
        set_vector(v);
        if (v == nullptr) {
            return;
        } else if (v->empty()) {
            m_basic_block = int_vector<64>(2, 0);
            return;
        }
        size_type basic_block_size = (((v->bit_size() + 63) >> 11) + 1) << 1;
        m_basic_block.resize(basic_block_size); // resize structure for basic_blocks
        if (m_basic_block.empty()) return;
        const uint64_t* data = m_v->data();
        size_type		i, j = 0;
        m_basic_block[0] = m_basic_block[1] = 0;

        uint64_t carry			  = trait_type::init_carry();
        uint64_t sum			  = trait_type::args_in_the_word(*data, carry);
        uint64_t second_level_cnt = 0;
        uint64_t cnt_words		  = 1;
        for (i = 1; i < ((m_v->bit_size() + 63) >> 6); ++i, ++cnt_words) {
            if (cnt_words == 32) {
                j += 2;
                m_basic_block[j - 1] = second_level_cnt;
                m_basic_block[j]	 = m_basic_block[j - 2] + sum;
                second_level_cnt = sum = cnt_words = 0;
            } else if ((cnt_words % 6) == 0) {
                // pack the prefix sum for each 6x64bit block into the second_level_cnt
                second_level_cnt |= sum << (60 - 12 * (cnt_words / 6)); //  48, 36, 24, 12, 0
            }
            sum += trait_type::args_in_the_word(*(++data), carry);
        }

        if ((cnt_words % 6) == 0) {
            second_level_cnt |= sum << (60 - 12 * (cnt_words / 6));
        }
        if (cnt_words == 32) {
            j += 2;
            m_basic_block[j - 1] = second_level_cnt;
            m_basic_block[j]	 = m_basic_block[j - 2] + sum;
            m_basic_block[j + 1] = 0;
        } else {
            m_basic_block[j + 1] = second_level_cnt;
        }
    }

    rank_support_v5(const rank_support_v5&) = default;
    rank_support_v5(rank_support_v5&&)		= default;
    rank_support_v5& operator=(const rank_support_v5&) = default;
    rank_support_v5& operator=(rank_support_v5&&) = default;

    size_type rank(size_type idx) const
    {
        assert(m_v != nullptr);
        assert(idx <= m_v->size());
        const uint64_t* p =
            m_basic_block.data() + ((idx >> 10) & 0xFFFFFFFFFFFFFFFEULL); // (idx/2048)*2
        //                     ( prefix sum of the 6x64bit blocks | (idx%2048)/(64*6) )
        size_type result = *p + ((*(p + 1) >> (60 - 12 * ((idx & 0x7FF) / (64 * 6)))) & 0x7FFULL) +
            trait_type::word_rank(m_v->data(), idx);
        idx -= (idx & 0x3F);
        uint8_t to_do = ((idx >> 6) & 0x1FULL) % 6;
        --idx;
        while (to_do) {
            result += trait_type::full_word_rank(m_v->data(), idx);
            --to_do;
            idx -= 64;
        }
        return result;
    }

    inline size_type operator()(size_type idx) const { return rank(idx); }
    size_type							  size() const { return m_v->size(); }

    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        size_type			 written_bytes = 0;
        structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
        written_bytes += m_basic_block.serialize(out, child, "cumulative_counts");
        structure_tree::add_size(child, written_bytes);
        return written_bytes;
    }

    void load(std::istream& in, const bit_vector* v = nullptr)
    {
        set_vector(v);
        m_basic_block.load(in);
    }

    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
    {
        ar(CEREAL_NVP(m_basic_block));
    }

    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
    {
        ar(CEREAL_NVP(m_basic_block));
    }

    bool operator==(const rank_support_v5& other) const noexcept
    {
        return m_basic_block == other.m_basic_block;
    }

    bool operator!=(const rank_support_v5& other) const noexcept
    {
        return !(*this == other);
    }

    void set_vector(const bit_vector* v = nullptr) { m_v = v; }
};

} // end namespace sds

#endif // end file

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file rank_support_scan.hpp
    \brief rank_support_scan.hpp contains rank_support_scan that support a sdsl::bit_vector with linear time rank information.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_RANK_SUPPORT_SCAN
#define INCLUDED_SDSL_RANK_SUPPORT_SCAN

//! Namespace for the succinct data structure library.
namespace sdsl {

//! A class supporting rank queries in linear time.
/*! \par Space complexity
 *       Constant.
 *  \par Time complexity
 *       Linear in the size of the supported vector.
 *
 *  \tparam t_b       Bit pattern which should be supported. Either `0`,`1`,`10`,`01`.
 *  \tparam t_pat_len Length of the bit pattern.
 * @ingroup rank_support_group
 */
template <uint8_t t_b = 1, uint8_t t_pat_len = 1>
class rank_support_scan : public rank_support {
private:
    static_assert(t_b == 1u or t_b == 0u or t_b == 10u or t_b == 11u,
                  "rank_support_scan: bit pattern must be `0`,`1`,`10` or `01`");
    static_assert(t_pat_len == 1u or t_pat_len == 2u,
                  "rank_support_scan: bit pattern length must be 1 or 2");

public:
    typedef bit_vector bit_vector_type;
    enum { bit_pat = t_b };
    enum { bit_pat_len = t_pat_len };

public:
    explicit rank_support_scan(const bit_vector* v = nullptr) : rank_support(v){};
    rank_support_scan(const rank_support_scan& rs) = default;
    rank_support_scan(rank_support_scan&& rs)	  = default;
    rank_support_scan& operator=(const rank_support_scan& rs) = default;
    rank_support_scan& operator=(rank_support_scan&& rs) = default;
    size_type rank(size_type idx) const;
    size_type operator()(size_type idx) const { return rank(idx); };
    size_type					   size() const { return m_v->size(); };
    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        return serialize_empty_object(out, v, name, this);
    }
    void load(std::istream&, const int_vector<1>* v = nullptr) { set_vector(v); }
    void set_vector(const bit_vector* v = nullptr) { m_v = v; }
    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t &) const {}
    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t &) {}

    //! Equality operator.
    bool operator==(rank_support_scan const & other) const noexcept
    {
        return (*m_v == *other.m_v);
    }

    //! Inequality operator.
    bool operator!=(rank_support_scan const & other) const noexcept
    {
        return !(*this == other);
    }
};

template <uint8_t t_b, uint8_t t_pat_len>
inline typename rank_support_scan<t_b, t_pat_len>::size_type
rank_support_scan<t_b, t_pat_len>::rank(size_type idx) const
{
    assert(m_v != nullptr);
    assert(idx <= m_v->size());
    const uint64_t* p	  = m_v->data();
    size_type		i	  = 0;
    size_type		result = 0;
    while (i + 64 <= idx) {
        result += rank_support_trait<t_b, t_pat_len>::full_word_rank(p, i);
        i += 64;
    }
    return result + rank_support_trait<t_b, t_pat_len>::word_rank(p, idx);
}

} // end namespace sds

#endif // end file

#endif // end file

//! Namespace for the succinct data structure library.
namespace sdsl {
//! The base class of classes supporting select queries for a sdsl::bit_vector in constant time.
/*! Abstract base class for classes supporting select queries.
 */
class select_support {
protected:
    const int_vector<1>* m_v; //!< Pointer to the select supported sdsl::bit_vector.
public:
    typedef int_vector<1>::size_type size_type;
    const bit_vector*				 vv;

    //! Constructor of select_support.
    /*! \param v The bit_vector to support rank queries.
         */
    select_support(const int_vector<1>* f_v = nullptr) : vv(f_v) { m_v = f_v; }
    //! Copy constructor
    /*! Copy the whole select_support including the  pointer
         *  to the supported bit_vector.
         */
    select_support(const select_support& f_v);
    //! Destructor of select_support.
    virtual ~select_support(){};

    //! Select returns the index of the i-th 1-bit in the supported bit_vector.
    /*!	\param i Argument to calculate the index of the i-th 1-bit in the supported bit_vector.
        	\return The index \f$\in [0..v.size()-1]\f$ of the i-th 1-bit in the supported bit_vector.
        	Call init or load to initialize the data structure before the first call of this method.
         	\sa init, load.
         */
    virtual size_type select(size_type i) const = 0;

    //! Alias for select
    virtual size_type operator()(size_type i) const = 0;
    //! Serialize the select_support to an out file stream.
    virtual size_type
    serialize(std::ostream& out, structure_tree_node* v, std::string name) const = 0;
    //! Load the select_support from an in file stream.
    /*!	Load an previously serialized select_support from a std::istream.
        	\param in The std::istream to load the select_support.
        	\param v The bit_vector to be supported.
        	\sa init, select.
         */
    virtual void load(std::istream& in, const int_vector<1>* v = nullptr) = 0;

    //! This method sets the supported bit_vector
    virtual void set_vector(const int_vector<1>* v = nullptr) = 0;
};

template <uint8_t bit_pattern, uint8_t pattern_len>
struct select_support_trait {
    typedef select_support::size_type size_type;

    /* Count the number of arguments for the specific select support */
    static size_type arg_cnt(const bit_vector&) { return 0; }

    static size_type args_in_the_first_word(uint64_t, uint8_t, uint64_t) { return 0; }

    static size_type ith_arg_pos_in_the_first_word(uint64_t, size_type, uint8_t, uint64_t)
    {
        return 0;
    }

    static size_type args_in_the_word(uint64_t, uint64_t&) { return 0; }

    static size_type ith_arg_pos_in_the_word(uint64_t, size_type, uint64_t) { return 0; }

    static bool found_arg(size_type, const bit_vector&) { return 0; }

    static uint64_t init_carry(const uint64_t*, size_type) { return 0; }

    static uint64_t get_carry(uint64_t) { return 0; }
};

template <>
struct select_support_trait<0, 1> {
    typedef select_support::size_type size_type;

    static size_type arg_cnt(const bit_vector& v) { return v.bit_size() - util::cnt_one_bits(v); }
    static size_type args_in_the_first_word(uint64_t w, uint8_t offset, uint64_t)
    {
        return bits::cnt((~w) & bits::lo_unset[offset]);
    }
    static size_type
    ith_arg_pos_in_the_first_word(uint64_t w, size_type i, uint8_t offset, uint64_t)
    {
        return bits::sel(~w & bits::lo_unset[offset], (uint32_t)i);
    }
    static size_type args_in_the_word(uint64_t w, uint64_t&) { return bits::cnt(~w); }
    static size_type ith_arg_pos_in_the_word(uint64_t w, size_type i, uint64_t)
    {
        return bits::sel(~w, (uint32_t)i);
    }
    static bool found_arg(size_type i, const bit_vector& v) { return !v[i]; }
    static uint64_t init_carry(const uint64_t*, size_type) { return 0; }
    static uint64_t get_carry(uint64_t) { return 0; }
};

template <>
struct select_support_trait<1, 1> {
    typedef select_support::size_type size_type;

    static size_type arg_cnt(const bit_vector& v) { return util::cnt_one_bits(v); }
    static size_type args_in_the_first_word(uint64_t w, uint8_t offset, uint64_t)
    {
        return bits::cnt(w & bits::lo_unset[offset]);
    }
    static size_type
    ith_arg_pos_in_the_first_word(uint64_t w, size_type i, uint8_t offset, uint64_t)
    {
        return bits::sel(w & bits::lo_unset[offset], (uint32_t)i);
    }
    static size_type args_in_the_word(uint64_t w, uint64_t&) { return bits::cnt(w); }
    static size_type ith_arg_pos_in_the_word(uint64_t w, size_type i, uint64_t)
    {
        return bits::sel(w, (uint32_t)i);
    }
    static bool found_arg(size_type i, const bit_vector& v) { return v[i] == 1; }
    static uint64_t init_carry(const uint64_t*, size_type) { return 0; }
    static uint64_t get_carry(uint64_t) { return 0; }
};

template <>
struct select_support_trait<10, 2> {
    typedef select_support::size_type size_type;

    static size_type arg_cnt(const bit_vector& v) { return util::cnt_onezero_bits(v); }
    static size_type args_in_the_first_word(uint64_t w, uint8_t offset, uint64_t carry)
    {
        return bits::cnt(bits::map10(w, carry) & bits::lo_unset[offset]);
    }
    static size_type
    ith_arg_pos_in_the_first_word(uint64_t w, size_type i, uint8_t offset, uint64_t carry)
    {
        return bits::sel(bits::map10(w, carry) & bits::lo_unset[offset], (uint32_t)i);
    }
    static size_type args_in_the_word(uint64_t w, uint64_t& carry) { return bits::cnt10(w, carry); }
    static size_type ith_arg_pos_in_the_word(uint64_t w, size_type i, uint64_t carry)
    {
        return bits::sel(bits::map10(w, carry), (uint32_t)i);
    }
    static bool found_arg(size_type i, const bit_vector& v)
    {
        if (i > 0 and v[i - 1] and !v[i]) return true;
        return false;
    }
    static uint64_t init_carry(const uint64_t* data, size_type word_pos)
    {
        return word_pos ? (*(data - 1) >> 63) : 0;
    }
    static uint64_t get_carry(uint64_t w) { return w >> 63; }
};

template <>
struct select_support_trait<01, 2> {
    typedef select_support::size_type size_type;

    static size_type arg_cnt(const bit_vector& v) { return util::cnt_zeroone_bits(v); }
    static size_type args_in_the_first_word(uint64_t w, uint8_t offset, uint64_t carry)
    {
        return bits::cnt(bits::map01(w, carry) & bits::lo_unset[offset]);
    }
    static size_type
    ith_arg_pos_in_the_first_word(uint64_t w, size_type i, uint8_t offset, uint64_t carry)
    {
        return bits::sel(bits::map01(w, carry) & bits::lo_unset[offset], (uint32_t)i);
    }
    static size_type args_in_the_word(uint64_t w, uint64_t& carry) { return bits::cnt01(w, carry); }
    static size_type ith_arg_pos_in_the_word(uint64_t w, size_type i, uint64_t carry)
    {
        return bits::sel(bits::map01(w, carry), (uint32_t)i);
    }
    static bool found_arg(size_type i, const bit_vector& v)
    {
        if (i > 0 and !v[i - 1] and v[i]) return true;
        return false;
    }
    static uint64_t init_carry(const uint64_t* data, size_type word_pos)
    {
        return word_pos ? (*(data - 1) >> 63) : 1;
    }
    static uint64_t get_carry(uint64_t w) { return w >> 63; }
};

template <>
struct select_support_trait<00, 2> {
    typedef select_support::size_type size_type;

    static size_type arg_cnt(const bit_vector& v)
    {
        const uint64_t* data = v.data();
        if (v.empty()) return 0;
        uint64_t  carry  = rank_support_trait<00, 2>::init_carry();
        size_type result = 0;
        for (auto end = v.data() + (v.size() >> 6); data < end; ++data) {
            result += rank_support_trait<00, 2>::args_in_the_word(*data, carry);
        }
        if (v.bit_size() &
            0x3F) { // if bit_size is not a multiple of 64, add count of the last word
            result += rank_support_trait<00, 2>::args_in_the_word(
                (*data) | bits::lo_unset[v.bit_size() & 0x3F], carry);
        }
        return result;
    }

    static size_type args_in_the_first_word(uint64_t w, uint8_t offset, uint64_t carry)
    {
        size_type res = 0;
        if (offset == 0)
            res = rank_support_trait<00, 2>::args_in_the_word(w, carry);
        else {
            res = bits::cnt((~(w | (w << 1))) & bits::lo_unset[offset]);
        }
        return res;
    }

    static size_type
    ith_arg_pos_in_the_first_word(uint64_t w, size_type i, uint8_t offset, uint64_t carry)
    {
        return bits::sel((~(((w << 1) | carry) | w)) & bits::lo_unset[offset], i);
    }
    static size_type args_in_the_word(uint64_t w, uint64_t& carry)
    {
        return rank_support_trait<00, 2>::args_in_the_word(w, carry);
    }
    static size_type ith_arg_pos_in_the_word(uint64_t w, size_type i, uint64_t carry)
    {
        return bits::sel(~(((w << 1) | carry) | w), i);
    }
    static bool found_arg(size_type i, const bit_vector& v)
    {
        return i > 0 and !v[i - 1] and !v[i];
    }
    static uint64_t init_carry(const uint64_t* data, size_type word_pos)
    {
        return word_pos ? (*(data - 1) >> 63) : 1;
    }
    static uint64_t get_carry(uint64_t w) { return w >> 63; }
};

template <>
struct select_support_trait<11, 2> {
    typedef select_support::size_type size_type;

    static size_type arg_cnt(const bit_vector& v)
    {
        const uint64_t* data = v.data();
        if (v.empty()) return 0;
        uint64_t  carry  = rank_support_trait<11, 2>::init_carry();
        size_type result = 0;
        for (auto end = v.data() + (v.size() >> 6); data < end; ++data) {
            result += rank_support_trait<11, 2>::args_in_the_word(*data, carry);
        }
        if (v.bit_size() &
            0x3F) { // if bit_size is not a multiple of 64, add count of the last word
            result += rank_support_trait<11, 2>::args_in_the_word(
                (*data) & bits::lo_set[v.bit_size() & 0x3F], carry);
        }
        return result;
    }

    static size_type args_in_the_first_word(uint64_t w, uint8_t offset, uint64_t carry)
    {
        size_type res = 0;
        if (offset == 0)
            res = rank_support_trait<11, 2>::args_in_the_word(w, carry);
        else {
            res = bits::cnt((w >> (offset - 1)) & (w >> offset));
        }
        return res;
    }

    static size_type
    ith_arg_pos_in_the_first_word(uint64_t w, size_type i, uint8_t offset, uint64_t carry)
    {
        return bits::sel((((w << 1) | carry) & w) & bits::lo_unset[offset], i);
    }
    static size_type args_in_the_word(uint64_t w, uint64_t& carry)
    {
        return rank_support_trait<11, 2>::args_in_the_word(w, carry);
    }
    static size_type ith_arg_pos_in_the_word(uint64_t w, size_type i, uint64_t carry)
    {
        return bits::sel(((w << 1) | carry) & w, i);
    }
    static bool found_arg(size_type i, const bit_vector& v)
    {
        if (i > 0 and v[i - 1] and v[i]) return true;
        return false;
    }
    static uint64_t init_carry(const uint64_t* data, size_type word_pos)
    {
        return word_pos ? (*(data - 1) >> 63) : 0;
    }
    static uint64_t get_carry(uint64_t w) { return w >> 63; }
};

} // end namespace sdsl

// Copyright (c) 2016, the SDSL Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.
/*! \file select_support_scan.hpp
    \brief select_support_scan.hpp contains classes that support a sdsl::bit_vector with linear time select.
    \author Simon Gog
*/
#ifndef INCLUDED_SDSL_SELECT_SUPPORT_SCAN
#define INCLUDED_SDSL_SELECT_SUPPORT_SCAN

//! Namespace for the succinct data structure library.
namespace sdsl {

//! A class supporting linear time select queries.
/*! \par Space complexity
 *       Constant.
 *  \par Time complexity
 *       Linear in the size of the supported vector.
 *
 *  \tparam t_b       Bit pattern which should be supported. Either `0`,`1`,`10`,`01`.
 *  \tparam t_pat_len Length of the bit pattern.
 * @ingroup select_support_group
 */
template <uint8_t t_b = 1, uint8_t t_pat_len = 1>
class select_support_scan : public select_support {
private:
    static_assert(t_b == 1u or t_b == 0u or t_b == 10u,
                  "select_support_scan: bit pattern must be `0`,`1`,`10` or `01`");
    static_assert(t_pat_len == 1u or t_pat_len == 2u,
                  "select_support_scan: bit pattern length must be 1 or 2");

public:
    typedef bit_vector bit_vector_type;
    enum { bit_pat = t_b };

public:
    explicit select_support_scan(const bit_vector* v = nullptr) : select_support(v) {}
    select_support_scan(const select_support_scan<t_b, t_pat_len>& ss) : select_support(ss.m_v) {}

    inline size_type select(size_type i) const;
    inline size_type operator()(size_type i) const { return select(i); }
    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        return serialize_empty_object(out, v, name, this);
    }
    void load(std::istream&, SDSL_UNUSED const bit_vector* v = nullptr) { set_vector(v); }
    //!\brief Serialise (save) via cereal
    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const;
    //!\brief Serialise (load) via cereal
    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t & ar);
    void set_vector(const bit_vector* v = nullptr) { m_v = v; }
    select_support_scan<t_b, t_pat_len>& operator=(const select_support_scan& ss)
    {
        set_vector(ss.m_v);
        return *this;
    }

    //! Equality operator.
    bool operator==(select_support_scan const & other) const noexcept
    {
        return (*m_v == *other.m_v);
    }

    //! Inequality operator.
    bool operator!=(select_support_scan const & other) const noexcept
    {
        return !(*this == other);
    }
};

template <uint8_t t_b, uint8_t t_pat_len>
template <typename archive_t>
void select_support_scan<t_b, t_pat_len>::CEREAL_SAVE_FUNCTION_NAME(archive_t &) const {}

template <uint8_t t_b, uint8_t t_pat_len>
template <typename archive_t>
void select_support_scan<t_b, t_pat_len>::CEREAL_LOAD_FUNCTION_NAME(archive_t &) {}

template <uint8_t t_b, uint8_t t_pat_len>
inline typename select_support_scan<t_b, t_pat_len>::size_type
select_support_scan<t_b, t_pat_len>::select(size_type i) const
{
    const uint64_t* data	 = m_v->data();
    size_type		word_pos = 0;
    size_type		word_off = 0;
    uint64_t		carry	= select_support_trait<t_b, t_pat_len>::init_carry(data, word_pos);
    size_type		args =
        select_support_trait<t_b, t_pat_len>::args_in_the_first_word(*data, word_off, carry);
    if (args >= i) {
        return (word_pos << 6) +
            select_support_trait<t_b, t_pat_len>::ith_arg_pos_in_the_first_word(
                *data, i, word_off, carry);
    }
    word_pos += 1;
    size_type sum_args = args;
    carry			   = select_support_trait<t_b, t_pat_len>::get_carry(*data);
    uint64_t old_carry = carry;
    args			   = select_support_trait<t_b, t_pat_len>::args_in_the_word(*(++data), carry);
    while (sum_args + args < i) {
        sum_args += args;
        assert(data + 1 < m_v->data() + (m_v->capacity() >> 6));
        old_carry = carry;
        args	  = select_support_trait<t_b, t_pat_len>::args_in_the_word(*(++data), carry);
        word_pos += 1;
    }
    return (word_pos << 6) + select_support_trait<t_b, t_pat_len>::ith_arg_pos_in_the_word(
        *data, i - sum_args, old_carry);
}

} // end namespace
#endif

#endif

//! Namespace for the succinct data structure library.
namespace sdsl {

//! A class supporting constant time select queries.
/*!
 * \par Space usage
 *      The space usage of the data structure depends on the number of \f$ m \f$ of ones in the
 *      original bitvector $b$. We store the position of every $4096$th set bit
 *      (called L1-sampled bits) of $b$.
 *      This takes in the worst case \f$\frac{m}{4096} \log{n} \leq \frac{n}{64}\f$ bits.
 *      Next,
 *      (1) if the distance of two adjacent L1-sampled bits $b[i]$ and $b[j]$
 *      is greater or equal than $\log^4 n$, then
 *      we store each of the 4096 positions of the set $b$ in [i..j-1] with
 *      $\log{n}$ bits. This results in at most
 *      \$ \frac{4096\cdot \log n}{\log^4 n}=\frac{4096}{\log^3 n}\$ bits per bit.
 *      For a bitvector of 4GB, i.e. \f$ \log n = 35 \f$ we get about 0.01 bits per bit.
 *      If the $j-i+1 < \log^4 n$ then
 *      (2) we store the relative position of every $64$th set bit (called L2-sampled bits)
 *      in b[i..j-1] in at most $4\log\log n$ bits per L2-sampled bits.
 *      An pessimistic upper bound for the space would be
 *      \f$ \frac{4\log\log n}{64} \leq \frac{24}{64} = 0.375\f$ bit per
 *      bit (since $\log\log n\leq 6$. It is very pessimistic, since we store
 *      the relative position in $\log\log(j-i+1)\leq \log\log n$ bits.
 *
 * \tparam t_b       Bit pattern `0`,`1`,`10`,`01` which should be ranked.
 * \tparam t_pat_len Length of the bit pattern.
 *
 * The implementation is a practical variant of the following reference:
 *
 * \par Reference
 *      David Clark:
 *      PhD Thesis: Compact Pat Trees
 *      University of Waterloo, 1996 (Section 2.2.2).
 *      http://www.nlc-bnc.ca/obj/s4/f2/dsk3/ftp04/nq21335.pdf
 *
 * @ingroup select_support_group
 */
template <uint8_t t_b = 1, uint8_t t_pat_len = 1>
class select_support_mcl : public select_support {
private:
    static_assert(t_b == 1u or t_b == 0u or t_b == 10u or t_b == 11u,
                  "select_support_mcl: bit pattern must be `0`,`1`,`10`, `01`, or `11`");
    static_assert(t_pat_len == 1u or t_pat_len == 2u,
                  "select_support_mcl: bit pattern length must be 1 or 2");

public:
    typedef bit_vector bit_vector_type;
    enum { bit_pat = t_b };
    enum { bit_pat_len = t_pat_len };

private:
    uint32_t m_logn = 0, // \f$ log(size) \f$
    m_logn2			= 0, // \f$ log^2(size) \f$
    m_logn4			= 0; // \f$ log^4(size) \f$
    // entry i of m_superblock equals the answer to select_1(B,i*4096)
    int_vector<0>  m_superblock;
    int_vector<0>* m_longsuperblock = nullptr;
    int_vector<0>* m_miniblock		= nullptr;
    size_type	  m_arg_cnt		= 0;
    void		   initData();
    void init_fast(const bit_vector* v = nullptr);

public:
    explicit select_support_mcl(const bit_vector* v = nullptr);
    select_support_mcl(const select_support_mcl<t_b, t_pat_len>& ss);
    select_support_mcl(select_support_mcl<t_b, t_pat_len>&& ss);
    ~select_support_mcl();

    void init_slow(const bit_vector* v = nullptr);
    //! Select function
    inline size_type select(size_type i) const;
    //! Alias for select(i).
    inline size_type operator()(size_type i) const;
    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const;
    void load(std::istream& in, const bit_vector* v = nullptr);
    void set_vector(const bit_vector* v = nullptr);
    //!\brief Serialise (save) via cereal
    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const;
    //!\brief Serialise (load) via cereal
    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t & ar);
    select_support_mcl<t_b, t_pat_len>& operator=(const select_support_mcl& ss);
    select_support_mcl<t_b, t_pat_len>& operator=(select_support_mcl&&);
    bool operator==(const select_support_mcl& other) const noexcept;
    bool operator!=(const select_support_mcl& other) const noexcept;
};

template <uint8_t t_b, uint8_t t_pat_len>
select_support_mcl<t_b, t_pat_len>::select_support_mcl(const bit_vector* f_v) : select_support(f_v)
{
    if (t_pat_len > 1 or (vv != nullptr and vv->size() < 100000))
        init_slow(vv);
    else
        init_fast(vv);
    return;
}

template <uint8_t t_b, uint8_t t_pat_len>
select_support_mcl<t_b, t_pat_len>::select_support_mcl(const select_support_mcl& ss)
    : select_support(ss.m_v)
    , m_logn(ss.m_logn)
    , m_logn2(ss.m_logn2)
    , m_logn4(ss.m_logn4)
    , m_superblock(ss.m_superblock)
    , m_arg_cnt(ss.m_arg_cnt)
{
    size_type sb = (m_arg_cnt + 4095) >> 12;
    if (ss.m_longsuperblock != nullptr) {
        m_longsuperblock = new int_vector<0>[sb]; //copy longsuperblocks
        for (size_type i = 0; i < sb; ++i) {
            m_longsuperblock[i] = ss.m_longsuperblock[i];
        }
    }
    m_miniblock = nullptr;
    if (ss.m_miniblock != nullptr) {
        m_miniblock = new int_vector<0>[sb]; // copy miniblocks
        for (size_type i = 0; i < sb; ++i) {
            m_miniblock[i] = ss.m_miniblock[i];
        }
    }
}

template <uint8_t t_b, uint8_t t_pat_len>
select_support_mcl<t_b, t_pat_len>::select_support_mcl(select_support_mcl&& ss)
    : select_support(ss.m_v)
{
    *this = std::move(ss);
}

template <uint8_t t_b, uint8_t		t_pat_len>
select_support_mcl<t_b, t_pat_len>& select_support_mcl<t_b, t_pat_len>::
operator=(const select_support_mcl& ss)
{
    if (this != &ss) {
        select_support_mcl tmp(ss);
        *this = std::move(tmp);
    }
    return *this;
}

template <uint8_t t_b, uint8_t		t_pat_len>
select_support_mcl<t_b, t_pat_len>& select_support_mcl<t_b, t_pat_len>::
operator=(select_support_mcl&& ss)
{
    if (this != &ss) {
        m_logn		 = ss.m_logn;				   // copy log n
        m_logn2		 = ss.m_logn2;				   // copy (logn)^2
        m_logn4		 = ss.m_logn4;				   // copy (logn)^4
        m_superblock = std::move(ss.m_superblock); // move long superblock
        m_arg_cnt	= ss.m_arg_cnt;			   // copy count of 1-bits
        m_v			 = ss.m_v;					   // copy pointer to the supported bit vector

        delete[] m_longsuperblock;
        m_longsuperblock	= ss.m_longsuperblock;
        ss.m_longsuperblock = nullptr;

        delete[] m_miniblock;
        m_miniblock	= ss.m_miniblock;
        ss.m_miniblock = nullptr;
    }
    return *this;
}

template <uint8_t t_b, uint8_t t_pat_len>
select_support_mcl<t_b, t_pat_len>::~select_support_mcl()
{
    delete[] m_longsuperblock;
    delete[] m_miniblock;
}

template <uint8_t t_b, uint8_t t_pat_len>
void select_support_mcl<t_b, t_pat_len>::init_slow(const bit_vector* v)
{
    set_vector(v);
    initData();
    if (m_v == nullptr) return;
    // Count the number of arguments in the bit vector
    m_arg_cnt = select_support_trait<t_b, t_pat_len>::arg_cnt(*v);

    const size_type SUPER_BLOCK_SIZE = 4096;

    if (m_arg_cnt == 0) // if there are no arguments in the vector we are done...
        return;

    size_type sb = (m_arg_cnt + SUPER_BLOCK_SIZE - 1) / SUPER_BLOCK_SIZE; // number of superblocks
    delete[] m_miniblock;
    m_miniblock = new int_vector<0>[sb];

    m_superblock = int_vector<0>(sb, 0, m_logn);

    size_type arg_position[SUPER_BLOCK_SIZE], arg_cnt = 0;
    size_type sb_cnt = 0;
    for (size_type i = 0; i < v->size(); ++i) {
        if (select_support_trait<t_b, t_pat_len>::found_arg(i, *v)) {
            arg_position[arg_cnt % SUPER_BLOCK_SIZE] = i;
            assert(arg_position[arg_cnt % SUPER_BLOCK_SIZE] == i);
            ++arg_cnt;
            if (arg_cnt % SUPER_BLOCK_SIZE == 0 or arg_cnt == m_arg_cnt) { //
                assert(sb_cnt < sb);
                m_superblock[sb_cnt] = arg_position[0];

                size_type pos_diff =
                    arg_position[(arg_cnt - 1) % SUPER_BLOCK_SIZE] - arg_position[0];
                if (pos_diff > m_logn4) { // longblock
                    if (m_longsuperblock == nullptr)
                        m_longsuperblock = new int_vector<0>[sb]; // create longsuperblock
                    m_longsuperblock[sb_cnt] =
                        int_vector<0>(SUPER_BLOCK_SIZE,
                                      0,
                                      bits::hi(arg_position[(arg_cnt - 1) % SUPER_BLOCK_SIZE]) + 1);

                    for (size_type j = 0; j <= (arg_cnt - 1) % SUPER_BLOCK_SIZE; ++j)
                        m_longsuperblock[sb_cnt][j] =
                            arg_position[j]; // copy argument positions to longsuperblock
                } else {				 // short block
                    m_miniblock[sb_cnt] = int_vector<0>(64, 0, bits::hi(pos_diff) + 1);
                    for (size_type j = 0; j <= (arg_cnt - 1) % SUPER_BLOCK_SIZE; j += 64) {
                        m_miniblock[sb_cnt][j / 64] = arg_position[j] - arg_position[0];
                    }
                }
                ++sb_cnt;
            }
        }
    }
}

// TODO: find bug, detected by valgrind
template <uint8_t t_b, uint8_t t_pat_len>
void select_support_mcl<t_b, t_pat_len>::init_fast(const bit_vector* v)
{
    set_vector(v);
    initData();
    if (m_v == nullptr) return;
    // Count the number of arguments in the bit vector
    m_arg_cnt = select_support_trait<t_b, t_pat_len>::arg_cnt(*v);

    const size_type SUPER_BLOCK_SIZE = 64 * 64;

    if (m_arg_cnt == 0) // if there are no arguments in the vector we are done...
        return;

    //    size_type sb = (m_arg_cnt+63+SUPER_BLOCK_SIZE-1)/SUPER_BLOCK_SIZE; // number of superblocks, add 63 as the last block could contain 63 uninitialized bits
    size_type sb = (m_arg_cnt + SUPER_BLOCK_SIZE - 1) / SUPER_BLOCK_SIZE; // number of superblocks
    delete[] m_miniblock;
    m_miniblock = new int_vector<0>[sb];

    m_superblock = int_vector<0>(sb, 0, m_logn); // TODO: hier koennte man logn noch optimieren...s

    bit_vector::size_type arg_position[SUPER_BLOCK_SIZE];
    const uint64_t*		  data		= v->data();
    uint64_t			  carry_new = 0;
    size_type			  last_k64 = 1, sb_cnt = 0;
    for (size_type i = 0, cnt_old = 0, cnt_new = 0, last_k64_sum = 1; i < (((v->bit_size() + 63) >> 6) << 6);
         i += 64, ++data) {
        cnt_new += select_support_trait<t_b, t_pat_len>::args_in_the_word(*data, carry_new);
        if (cnt_new >= last_k64_sum) {
            arg_position[last_k64 - 1] =
                i + select_support_trait<t_b, t_pat_len>::ith_arg_pos_in_the_word(
                    *data, last_k64_sum - cnt_old, carry_new);
            last_k64 += 64;
            last_k64_sum += 64;

            if (last_k64 == SUPER_BLOCK_SIZE + 1) {
                m_superblock[sb_cnt]				   = arg_position[0];
                size_type pos_of_last_arg_in_the_block = arg_position[last_k64 - 65];

                for (size_type ii = arg_position[last_k64 - 65] + 1, j = last_k64 - 65;
                     ii < v->size() and j < SUPER_BLOCK_SIZE;
                     ++ii)
                    if (select_support_trait<t_b, t_pat_len>::found_arg(ii, *v)) {
                        pos_of_last_arg_in_the_block = ii;
                        ++j;
                    }
                size_type pos_diff = pos_of_last_arg_in_the_block - arg_position[0];
                if (pos_diff > m_logn4) { // long block
                    if (m_longsuperblock == nullptr)
                        m_longsuperblock = new int_vector<0>[sb + 1]; // create longsuperblock
                    // GEANDERT am 2010-07-17 +1 nach pos_of_last_arg..
                    m_longsuperblock[sb_cnt] =
                        int_vector<0>(SUPER_BLOCK_SIZE, 0, bits::hi(pos_of_last_arg_in_the_block) + 1);
                    for (size_type j = arg_position[0], k = 0;
                         k < SUPER_BLOCK_SIZE and j <= pos_of_last_arg_in_the_block;
                         ++j)
                        if (select_support_trait<t_b, t_pat_len>::found_arg(j, *v)) {
                            if (k >= SUPER_BLOCK_SIZE) {
                                for (size_type ii = 0; ii < SUPER_BLOCK_SIZE; ++ii) {
                                    std::cout << "(" << ii << "," << m_longsuperblock[sb_cnt][ii]
                                              << ") ";
                                }
                                std::cout << std::endl;
                                std::cout << "k=" << k << " SUPER_BLOCK_SIZE=" << SUPER_BLOCK_SIZE
                                          << std::endl;
                                std::cout << "pos_of_last_arg_in_the_block"
                                          << pos_of_last_arg_in_the_block << std::endl;
                                std::cout.flush();
                            }
                            m_longsuperblock[sb_cnt][k++] = j;
                        }
                } else {
                    m_miniblock[sb_cnt] = int_vector<0>(64, 0, bits::hi(pos_diff) + 1);
                    for (size_type j = 0; j < SUPER_BLOCK_SIZE; j += 64) {
                        m_miniblock[sb_cnt][j / 64] = arg_position[j] - arg_position[0];
                    }
                }
                ++sb_cnt;
                last_k64 = 1;
            }
        }
        cnt_old = cnt_new;
    }
    // handle last block: append long superblock
    if (last_k64 > 1) {
        if (m_longsuperblock == nullptr)
            m_longsuperblock	 = new int_vector<0>[sb + 1]; // create longsuperblock
        m_longsuperblock[sb_cnt] = int_vector<0>(SUPER_BLOCK_SIZE, 0, bits::hi(v->size() - 1) + 1);
        for (size_type i = arg_position[0], k = 0; i < v->size(); ++i) {
            if (select_support_trait<t_b, t_pat_len>::found_arg(i, *v)) {
                m_longsuperblock[sb_cnt][k++] = i;
            }
        }
        ++sb_cnt;
    }
}

template <uint8_t t_b, uint8_t t_pat_len>
inline auto select_support_mcl<t_b, t_pat_len>::select(size_type i) const -> size_type
{
    assert(i > 0 and i <= m_arg_cnt);

    i				 = i - 1;
    size_type sb_idx = i >> 12;   // i/4096
    size_type offset = i & 0xFFF; // i%4096
    if (m_longsuperblock != nullptr and !m_longsuperblock[sb_idx].empty()) {
        return m_longsuperblock[sb_idx][offset];
    } else {
        if ((offset & 0x3F) == 0) {
            assert(sb_idx < m_superblock.size());
            assert((offset >> 6) < m_miniblock[sb_idx].size());
            return m_superblock[sb_idx] + m_miniblock[sb_idx][offset >> 6 /*/64*/];
        } else {
            i = i - (sb_idx << 12) - ((offset >> 6) << 6);
            // now i > 0 and i <= 64
            assert(i > 0);
            size_type pos = m_superblock[sb_idx] + m_miniblock[sb_idx][offset >> 6] + 1;

            // now pos is the position from where we search for the ith argument
            size_type		word_pos = pos >> 6;
            size_type		word_off = pos & 0x3F;
            const uint64_t* data	 = m_v->data() + word_pos;
            uint64_t  carry = select_support_trait<t_b, t_pat_len>::init_carry(data, word_pos);
            size_type args =
                select_support_trait<t_b, t_pat_len>::args_in_the_first_word(*data, word_off, carry);

            if (args >= i) {
                return (word_pos << 6) +
                    select_support_trait<t_b, t_pat_len>::ith_arg_pos_in_the_first_word(
                        *data, i, word_off, carry);
            }
            word_pos += 1;
            size_type sum_args = args;
            carry			   = select_support_trait<t_b, t_pat_len>::get_carry(*data);
            uint64_t old_carry = carry;
            args = select_support_trait<t_b, t_pat_len>::args_in_the_word(*(++data), carry);
            while (sum_args + args < i) {
                sum_args += args;
                assert(data + 1 < m_v->data() + ((m_v->bit_size() + 63) >> 6));
                old_carry = carry;
                args = select_support_trait<t_b, t_pat_len>::args_in_the_word(*(++data), carry);
                word_pos += 1;
            }
            return (word_pos << 6) + select_support_trait<t_b, t_pat_len>::ith_arg_pos_in_the_word(
                *data, i - sum_args, old_carry);
        }
    }
}

template <uint8_t t_b, uint8_t t_pat_len>
inline auto select_support_mcl<t_b, t_pat_len>::operator()(size_type i) const -> size_type
{
    return select(i);
}

template <uint8_t t_b, uint8_t t_pat_len>
void select_support_mcl<t_b, t_pat_len>::initData()
{
    m_arg_cnt = 0;
    if (nullptr == m_v) {
        m_logn = m_logn2 = m_logn4 = 0;
    } else {
        m_logn = bits::hi(((m_v->bit_size() + 63) >> 6) << 6) + 1; // TODO maybe it's better here to take a max(...,12)
        m_logn2 = m_logn * m_logn;
        m_logn4 = m_logn2 * m_logn2;
    }
    delete[] m_longsuperblock;
    m_longsuperblock = nullptr;
    delete[] m_miniblock;
    m_miniblock = nullptr;
}

template <uint8_t t_b, uint8_t t_pat_len>
void select_support_mcl<t_b, t_pat_len>::set_vector(const bit_vector* v)
{
    m_v = v;
}

template <uint8_t t_b, uint8_t t_pat_len>
auto select_support_mcl<t_b, t_pat_len>::serialize(std::ostream& out,
                                                   structure_tree_node* v,
                                                   std::string			name) const -> size_type
{
    structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
    size_type			 written_bytes = 0;
    // write the number of 1-bits in the supported bit_vector
    out.write((char*)&m_arg_cnt, sizeof(size_type) / sizeof(char));
    written_bytes = sizeof(size_type) / sizeof(char);
    // number of superblocks in the data structure
    size_type sb = (m_arg_cnt + 4095) >> 12;

    if (m_arg_cnt) { // if there exists 1-bits to be supported
        written_bytes += m_superblock.serialize(out, child, "superblock"); // serialize superblocks
        bit_vector mini_or_long; // Helper vector: mini or long block?
        if (m_longsuperblock != nullptr) {
            mini_or_long.resize(sb); // resize indicator bit_vector to the number of superblocks
            for (size_type i	= 0; i < sb; ++i)
                mini_or_long[i] = !m_miniblock[i].empty();
        }
        written_bytes += mini_or_long.serialize(out, child, "mini_or_long");
        size_type written_bytes_long = 0;
        size_type written_bytes_mini = 0;
        for (size_type i = 0; i < sb; ++i)
            if (!mini_or_long.empty() and !mini_or_long[i]) {
                written_bytes_long += m_longsuperblock[i].serialize(out);
            } else {
                written_bytes_mini += m_miniblock[i].serialize(out);
            }
        written_bytes += written_bytes_long;
        written_bytes += written_bytes_mini;
        structure_tree_node* child_long =
            structure_tree::add_child(child, "longsuperblock", util::class_name(m_longsuperblock));
        structure_tree::add_size(child_long, written_bytes_long);
        structure_tree_node* child_mini =
            structure_tree::add_child(child, "minisuperblock", util::class_name(m_miniblock));
        structure_tree::add_size(child_mini, written_bytes_mini);
    }
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}

template <uint8_t t_b, uint8_t t_pat_len>
void select_support_mcl<t_b, t_pat_len>::load(std::istream& in, const bit_vector* v)
{
    set_vector(v);
    initData();
    // read the number of 1-bits in the supported bit_vector
    in.read((char*)&m_arg_cnt, sizeof(size_type) / sizeof(char));
    size_type sb = (m_arg_cnt + 4095) >> 12;

    if (m_arg_cnt) {		   // if there exists 1-bits to be supported
        m_superblock.load(in); // load superblocks

        delete[] m_miniblock;
        m_miniblock = nullptr;
        delete[] m_longsuperblock;
        m_longsuperblock = nullptr;

        bit_vector mini_or_long;			 // Helper vector: mini or long block?
        mini_or_long.load(in);				 // Load the helper vector
        m_miniblock = new int_vector<0>[sb]; // Create miniblock int_vector<0>
        if (!mini_or_long.empty())
            m_longsuperblock = new int_vector<0>[sb]; // Create longsuperblock int_vector<0>

        for (size_type i = 0; i < sb; ++i)
            if (!mini_or_long.empty() and not mini_or_long[i]) {
                m_longsuperblock[i].load(in);
            } else {
                m_miniblock[i].load(in);
            }
    }
}

template <uint8_t t_b, uint8_t t_pat_len>
template <typename archive_t>
void select_support_mcl<t_b, t_pat_len>::CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
{
    ar(CEREAL_NVP(m_arg_cnt));
    ar(CEREAL_NVP(m_logn));
    ar(CEREAL_NVP(m_logn2));
    ar(CEREAL_NVP(m_logn4));
    size_type sb = (m_arg_cnt + 4095) >> 12;
    if (m_arg_cnt) {
        ar(CEREAL_NVP(m_superblock));
        bit_vector mini_or_long;
        if (m_longsuperblock != nullptr) {
            mini_or_long.resize(sb);
            for (size_type i	= 0; i < sb; ++i) {
                mini_or_long[i] = !m_miniblock[i].empty();
            }
        }
        ar(CEREAL_NVP(mini_or_long));
        for (size_type i = 0; i < sb; ++i) {
            if (!mini_or_long.empty() and !mini_or_long[i]) {
                ar(CEREAL_NVP(m_longsuperblock[i]));
            }
            else {
                ar(CEREAL_NVP(m_miniblock[i]));
            }
        }
    }
}

template <uint8_t t_b, uint8_t t_pat_len>
template <typename archive_t>
void select_support_mcl<t_b, t_pat_len>::CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
{
    delete[] m_longsuperblock;
    m_longsuperblock = nullptr;
    delete[] m_miniblock;
    m_miniblock = nullptr;

    ar(CEREAL_NVP(m_arg_cnt));
    ar(CEREAL_NVP(m_logn));
    ar(CEREAL_NVP(m_logn2));
    ar(CEREAL_NVP(m_logn4));

    size_type sb = (m_arg_cnt + 4095) >> 12;

    if (m_arg_cnt) {
        ar(CEREAL_NVP(m_superblock));

        delete[] m_miniblock;
        m_miniblock = nullptr;
        delete[] m_longsuperblock;
        m_longsuperblock = nullptr;

        bit_vector mini_or_long;
        ar(CEREAL_NVP(mini_or_long));
        m_miniblock = new int_vector<0>[sb];

        if (!mini_or_long.empty()) {
            m_longsuperblock = new int_vector<0>[sb];
        }

        for (size_type i = 0; i < sb; ++i) {
            if (!mini_or_long.empty() and !mini_or_long[i]) {
                ar(CEREAL_NVP(m_longsuperblock[i]));
            }
            else {
                ar(CEREAL_NVP(m_miniblock[i]));
            }
        }
    }
}

template <uint8_t t_b, uint8_t t_pat_len>
bool select_support_mcl<t_b, t_pat_len>::operator==(const select_support_mcl& other) const noexcept
{
    return (m_logn == other.m_logn) && (m_logn2 == other.m_logn2) && (m_logn4 == other.m_logn4) &&
        (m_superblock == other.m_superblock) && (m_arg_cnt == other.m_arg_cnt) &&
        ((m_longsuperblock == nullptr && other.m_longsuperblock == nullptr) ||
            (*m_longsuperblock == *other.m_longsuperblock)) &&
        ((m_miniblock == other.m_miniblock) || (*m_miniblock == *other.m_miniblock));
}

template <uint8_t t_b, uint8_t t_pat_len>
bool select_support_mcl<t_b, t_pat_len>::operator!=(const select_support_mcl& other) const noexcept
{
    return !(*this == other);
}
}

#endif

//! Namespace for the succinct data structure library
namespace sdsl {

// forward declaration needed for friend declaration
template <uint8_t t_b			= 1,
    class t_hi_bit_vector = bit_vector,
    class t_select_1		= typename t_hi_bit_vector::select_1_type,
    class t_select_0		= typename t_hi_bit_vector::select_0_type>
class rank_support_sd; // in sd_vector

// forward declaration needed for friend declaration
template <uint8_t t_b			= 1,
    class t_hi_bit_vector = bit_vector,
    class t_select_1		= typename t_hi_bit_vector::select_1_type,
    class t_select_0		= typename t_hi_bit_vector::select_0_type>
class select_support_sd; // in sd_vector

// forward declaration needed for friend declaration
template <typename, typename, typename>
class sd_vector; // in sd_vector

//! Class for in-place construction of sd_vector from a strictly increasing sequence
/*! \par Building an sd_vector will clear the builder.
 */
class sd_vector_builder {
    template <typename, typename, typename>
    friend class sd_vector;

public:
    typedef bit_vector::size_type size_type;

private:
    size_type m_size, m_capacity;
    size_type m_wl;
    size_type m_tail, m_items;
    size_type m_last_high, m_highpos;

    int_vector<> m_low;
    bit_vector   m_high;

public:
    sd_vector_builder();

    //! Constructor
    /*! \param n Vector size.
         *  \param m The number of 1-bits.
         */
    sd_vector_builder(size_type n, size_type m);

    inline size_type size() const { return m_size; }
    inline size_type capacity() const { return m_capacity; }
    inline size_type tail() const { return m_tail; }
    inline size_type items() const { return m_items; }

    //! Set a bit to 1.
    /*! \param i The position of the bit.
         *  \par The position must be strictly greater than for the previous call.
         */
    inline void set(size_type i)
    {
        assert(i >= m_tail && i < m_size);
        assert(m_items < m_capacity);

        size_type cur_high = i >> m_wl;
        m_highpos += (cur_high - m_last_high);
        m_last_high			= cur_high;
        m_low[m_items++]	= i; // int_vector truncates the most significant logm bits
        m_high[m_highpos++] = 1; // write 1 for the entry
        m_tail				= i + 1;
    }
};

//! A bit vector which compresses very sparse populated bit vectors by
// representing the positions of 1 by the Elias-Fano representation for non-decreasing sequences
/*!
 * \par Other implementations of this data structure:
 *  - the sdarray of Okanohara and Sadakane
 *  - Sebastiano Vigna implemented a elias_fano class in this sux library.
 *
 * \par References
 *  - P. Elias: ,,Efficient storage and retrieval by content and address of static files'',
 *              Journal of the ACM, 1974
 *  - R. Fano: ,,On the number of bits required to implement an associative memory''.
 *             Memorandum 61. Computer Structures Group, Project MAC, MIT, 1971
 *  - D. Okanohara, K. Sadakane: ,,Practical Entropy-Compressed Rank/Select Dictionary'',
 *             Proceedings of ALENEX 2007.
 *
 *  \tparam t_hi_bit_vector Type of the bitvector used for the unary decoded differences of
 *                          the high part of the positions of the 1s.
 *  \tparam t_select_1      Type of the select structure which is used to select ones in HI.
 *  \tparam t_select_0      Type of the select structure which is used to select zeros in HI.
 */
template <class t_hi_bit_vector = bit_vector,
    class t_select_1		= typename t_hi_bit_vector::select_1_type,
    class t_select_0		= typename t_hi_bit_vector::select_0_type>
class sd_vector {
public:
    typedef bit_vector::size_type					size_type;
    typedef size_type								value_type;
    typedef bit_vector::difference_type				difference_type;
    typedef random_access_const_iterator<sd_vector> iterator;
    typedef iterator								const_iterator;
    typedef bv_tag									index_category;
    typedef t_select_0								select_0_support_type;
    typedef t_select_1								select_1_support_type;

    typedef rank_support_sd<0, t_hi_bit_vector, select_1_support_type, select_0_support_type>
        rank_0_type;
    typedef rank_support_sd<1, t_hi_bit_vector, select_1_support_type, select_0_support_type>
        rank_1_type;
    typedef select_support_sd<0, t_hi_bit_vector, select_1_support_type, select_0_support_type>
        select_0_type;
    typedef select_support_sd<1, t_hi_bit_vector, select_1_support_type, select_0_support_type>
        select_1_type;

    typedef t_hi_bit_vector hi_bit_vector_type;

private:
    // we need this variables to represent the m ones of the original bit vector of size n
    size_type m_size = 0; // length of the original bit vector
    uint8_t   m_wl   = 0; // log n - log m, where n is the length of the original bit vector
    // and m is the number of ones in the bit vector, wl is the abbreviation
    // for ,,width (of) low (part)''

    int_vector<> m_low; // vector for the least significant bits of the positions of the m ones
    hi_bit_vector_type
        m_high; // bit vector that represents the most significant bit in permuted order
    select_1_support_type m_high_1_select; // select support for the ones in m_high
    select_0_support_type m_high_0_select; // select support for the zeros in m_high

public:
    const uint8_t&				 wl			   = m_wl;
    const hi_bit_vector_type&	high		   = m_high;
    const int_vector<>&			 low		   = m_low;
    const select_1_support_type& high_1_select = m_high_1_select;
    const select_0_support_type& high_0_select = m_high_0_select;

    sd_vector() {}

    sd_vector(const sd_vector& sd)
        : m_size(sd.m_size)
        , m_wl(sd.m_wl)
        , m_low(sd.m_low)
        , m_high(sd.m_high)
        , m_high_1_select(sd.m_high_1_select)
        , m_high_0_select(sd.m_high_0_select)
    {
        m_high_1_select.set_vector(&m_high);
        m_high_0_select.set_vector(&m_high);
    }

    sd_vector& operator=(const sd_vector& v)
    {
        if (this != &v) {
            sd_vector tmp(v);
            *this = std::move(tmp);
        }
        return *this;
    }

    sd_vector& operator=(sd_vector&& v)
    {
        if (this != &v) {
            m_size			= v.m_size;
            m_wl			= v.m_wl;
            m_low			= std::move(v.m_low);
            m_high			= std::move(v.m_high);
            m_high_1_select = std::move(v.m_high_1_select);
            m_high_1_select.set_vector(&m_high);
            m_high_0_select = std::move(v.m_high_0_select);
            m_high_0_select.set_vector(&m_high);
        }
        return *this;
    }

    sd_vector(sd_vector&& sd) { *this = std::move(sd); }

    sd_vector(const bit_vector& bv)
    {
        m_size = bv.size();
        size_type m	= util::cnt_one_bits(bv);
        std::pair<size_type, size_type> params = get_params(m_size, m);
        m_wl = params.first;
        m_low = int_vector<>(m, 0, params.first);
        bit_vector high = bit_vector(params.second, 0);

        const uint64_t* bvp  = bv.data();
        for (size_type i = 0, mm = 0, last_high = 0, highpos = 0; i < (bv.size() + 63) / 64;
             ++i, ++bvp) {
            size_type position = 64 * i;
            uint64_t  w		   = *bvp;
            while (w) { // process bit_vector word by word
                uint8_t offset = bits::lo(w);
                w >>= offset; // note:  w >>= (offset+1) can not be applied for offset=63!
                position += offset;
                if (position >=
                    bv.size()) // check that we have not reached the end of the bitvector
                    break;
                // (1) handle high part
                size_type cur_high = position >> m_wl;
                highpos += (cur_high - last_high); // write cur_high-last_high 0s
                last_high = cur_high;
                // (2) handle low part
                m_low[mm++]		= position; // int_vector truncates the most significant logm bits
                high[highpos++] = 1;		// write 1 for the entry
                position += 1;
                w >>= 1;
            }
        }
        m_high = std::move(high);
        util::init_support(m_high_1_select, &m_high);
        util::init_support(m_high_0_select, &m_high);
    }

    template <class t_itr>
    sd_vector(const t_itr begin, const t_itr end)
    {
        if (begin == end) {
            return;
        }
        size_type m = std::distance(begin, end);
        m_size = *(end - 1) + 1;
        std::pair<size_type, size_type> params = get_params(m_size, m);
        m_wl = params.first;
        m_low = int_vector<>(m, 0, params.first);
        bit_vector high = bit_vector(params.second, 0);

        auto	   itr  = begin;
        size_type  mm = 0, last_high = 0, highpos = 0;
        while (itr != end) {
            auto position = *itr;
            // (1) handle high part
            size_type cur_high = position >> m_wl;
            highpos += (cur_high - last_high); // write cur_high-last_high 0s
            last_high = cur_high;
            // (2) handle low part
            m_low[mm++]		= position; // int_vector truncates the most significant logm bits
            high[highpos++] = 1;		// write 1 for the entry
            ++itr;
        }

        m_high = std::move(high);
        util::init_support(m_high_1_select, &m_high);
        util::init_support(m_high_0_select, &m_high);
    }

    sd_vector(sd_vector_builder& builder)
    {
        if (builder.items() != builder.capacity()) {
            throw std::runtime_error("sd_vector: builder is not at full capacity.");
        }

        m_size = builder.m_size;
        m_wl   = builder.m_wl;
        m_low  = std::move(builder.m_low);
        m_high = std::move(builder.m_high);
        util::init_support(m_high_1_select, &(this->m_high));
        util::init_support(m_high_0_select, &(this->m_high));

        builder = sd_vector_builder();
    }

    // Returns `(low.width(), high.size())`.
    //
    // This is based on:
    //
    //   Ma, Puglisi, Raman, Zhukova:
    //   On Elias-Fano for Rank Queries in FM-Indexes.
    //   DCC 2021.
    //
    // Implementation credit: Jouni Siren, https://github.com/vgteam/sdsl-lite
    static std::pair<size_type, size_type> get_params(size_type universe, size_type ones)
    {
        size_type low_width = 1;
        // Multisets with too many ones will have width 1.
        if (ones > 0 && ones <= universe) {
            double ideal_width = std::log2((static_cast<double>(universe) * std::log(2.0)) / static_cast<double>(ones));
            low_width = std::round(std::max(ideal_width, 1.0));
        }
        size_type buckets = get_buckets(universe, low_width);
        return std::pair<size_type, size_type>(low_width, ones + buckets);
    }

    // Returns the number of buckets.
    static size_type get_buckets(size_type universe, size_type low_width)
    {
        size_type buckets = universe >> low_width;
        if ((universe & bits::lo_set[low_width]) != 0) {
            buckets++;
        }
        return buckets;
    }

    //! Accessing the i-th element of the original bit_vector
    /*! \param i An index i with \f$ 0 \leq i < size()  \f$.
        *   \return The i-th bit of the original bit_vector
        *   \par Time complexity
        *           \f$ \Order{t_{select0} + n/m} \f$, where m equals the number of zeros
        *    \par Remark
         *         The time complexity can be easily improved to
        *            \f$\Order{t_{select0}+\log(n/m)}\f$
        *        by using binary search in the second step.
        */
    value_type operator[](size_type i) const
    {
        size_type high_val = (i >> (m_wl));
        size_type sel_high = m_high_0_select(high_val + 1);
        size_type rank_low = sel_high - high_val;
        if (0 == rank_low) return 0;
        size_type val_low = i & bits::lo_set[m_wl]; // extract the low m_wl = log n -log m bits
        --sel_high;
        --rank_low;
        while (m_high[sel_high] and m_low[rank_low] > val_low) {
            if (sel_high > 0) {
                --sel_high;
                --rank_low;
            } else
                return 0;
        }
        return m_high[sel_high] and m_low[rank_low] == val_low;
    }

    //! Get the integer value of the binary string of length len starting at position idx.
    /*! \param idx Starting index of the binary representation of the integer.
         *  \param len Length of the binary representation of the integer. Default value is 64.
         *  \returns The integer value of the binary string of length len starting at position idx.
         *
         *  \pre idx+len-1 in [0..size()-1]
         *  \pre len in [1..64]
         */
    uint64_t get_int(size_type idx, const uint8_t len = 64) const
    {
        uint64_t i		  = idx + len - 1;
        uint64_t high_val = (i >> (m_wl));
        uint64_t sel_high = m_high_0_select(high_val + 1);
        uint64_t rank_low = sel_high - high_val;
        if (0 == rank_low) return 0;
        size_type val_low = i & bits::lo_set[m_wl]; // extract the low m_wl = log n -log m bits
        --sel_high;
        --rank_low;
        while (m_high[sel_high] and m_low[rank_low] > val_low) {
            if (sel_high > 0) {
                --sel_high;
                --rank_low;
            } else
                return 0;
        }
        uint64_t res = 0;
        while (true) {
            while (!m_high[sel_high]) {
                if (sel_high > 0 and (high_val << m_wl) >= idx) {
                    --sel_high;
                    --high_val;
                } else {
                    return res;
                }
            }
            while (m_high[sel_high]) {
                uint64_t val = (high_val << m_wl) + m_low[rank_low];
                if (val >= idx) {
                    res |= 1ULL << (val - idx);
                } else {
                    return res;
                }
                if (sel_high > 0) {
                    --sel_high;
                    --rank_low;
                } else {
                    return res;
                }
            }
        }
    }

    //! Returns the size of the original bit vector.
    size_type size() const { return m_size; }

    //! Serializes the data structure into the given ostream
    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
        size_type			 written_bytes = 0;
        written_bytes += write_member(m_size, out, child, "size");
        written_bytes += write_member(m_wl, out, child, "wl");
        written_bytes += m_low.serialize(out, child, "low");
        written_bytes += m_high.serialize(out, child, "high");
        written_bytes += m_high_1_select.serialize(out, child, "high_1_select");
        written_bytes += m_high_0_select.serialize(out, child, "high_0_select");
        structure_tree::add_size(child, written_bytes);
        return written_bytes;
    }

    //! Loads the data structure from the given istream.
    void load(std::istream& in)
    {
        read_member(m_size, in);
        read_member(m_wl, in);
        m_low.load(in);
        m_high.load(in);
        m_high_1_select.load(in, &m_high);
        m_high_0_select.load(in, &m_high);
    }

    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
    {
        ar(CEREAL_NVP(m_size));
        ar(CEREAL_NVP(m_wl));
        ar(CEREAL_NVP(m_low));
        ar(CEREAL_NVP(m_high));
        ar(CEREAL_NVP(m_high_1_select));
        ar(CEREAL_NVP(m_high_0_select));
    }

    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
    {
        ar(CEREAL_NVP(m_size));
        ar(CEREAL_NVP(m_wl));
        ar(CEREAL_NVP(m_low));
        ar(CEREAL_NVP(m_high));
        ar(CEREAL_NVP(m_high_1_select));
        m_high_1_select.set_vector(&m_high);
        ar(CEREAL_NVP(m_high_0_select));
        m_high_0_select.set_vector(&m_high);
    }

    iterator begin() const { return iterator(this, 0); }

    iterator end() const { return iterator(this, size()); }

    bool operator==(const sd_vector& v) const
    {
        return m_size == v.m_size && m_wl == v.m_wl && m_low == v.m_low && m_high == v.m_high;
    }

    bool operator!=(const sd_vector& v) const { return !(*this == v); }
};

//! Specialized constructor that is a bit more space-efficient than the default.
template <>
sd_vector<>::sd_vector(sd_vector_builder& builder);

template <uint8_t t_b>
struct rank_support_sd_trait {
    typedef bit_vector::size_type size_type;
    static size_type adjust_rank(size_type r, size_type) { return r; }
};

template <>
struct rank_support_sd_trait<0> {
    typedef bit_vector::size_type size_type;
    static size_type adjust_rank(size_type r, size_type n) { return n - r; }
};

//! Rank data structure for sd_vector
/*! \tparam t_b             Bit pattern.
 *  \tparam t_hi_bit_vector Type of the bitvector used for the unary decoded differences of
 *                          the high part of the positions of the 1s.
 *  \tparam t_select_1      Type of the select structure which is used to select ones in HI.
 *  \tparam t_select_0      Type of the select structure which is used to select zeros in HI.
 */
template <uint8_t t_b, class t_hi_bit_vector, class t_select_1, class t_select_0>
class rank_support_sd {
    static_assert(t_b == 1u or t_b == 0u, "rank_support_sd: bit pattern must be `0` or `1`");

public:
    typedef bit_vector::size_type size_type;
    typedef sd_vector<t_hi_bit_vector, t_select_1, t_select_0> bit_vector_type;
    enum { bit_pat = t_b };
    enum { bit_pat_len = (uint8_t)1 };

private:
    const bit_vector_type* m_v;

public:
    explicit rank_support_sd(const bit_vector_type* v = nullptr) { set_vector(v); }

    size_type rank(size_type i) const
    {
        assert(m_v != nullptr);
        assert(i <= m_v->size());
        // split problem in two parts:
        // (1) find  >=
        size_type high_val = (i >> (m_v->wl));
        size_type sel_high = m_v->high_0_select(high_val + 1);
        size_type rank_low = sel_high - high_val; //
        if (0 == rank_low) return rank_support_sd_trait<t_b>::adjust_rank(0, i);
        size_type val_low = i & bits::lo_set[m_v->wl];
        // now since rank_low > 0 => sel_high > 0
        do {
            if (!sel_high) return rank_support_sd_trait<t_b>::adjust_rank(0, i);
            --sel_high;
            --rank_low;
        } while (m_v->high[sel_high] and m_v->low[rank_low] >= val_low);
        return rank_support_sd_trait<t_b>::adjust_rank(rank_low + 1, i);
    }

    size_type operator()(size_type i) const { return rank(i); }

    size_type size() const { return m_v->size(); }

    void set_vector(const bit_vector_type* v = nullptr) { m_v = v; }

    void load(std::istream&, const bit_vector_type* v = nullptr) { set_vector(v); }

    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        return serialize_empty_object(out, v, name, this);
    }

    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t &) const {}

    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t &) {}

    bool operator==(const rank_support_sd& other) const noexcept
    {
        return *m_v == *other.m_v;
    }

    bool operator!=(const rank_support_sd& other) const noexcept
    {
        return !(*this == other);
    }
};

template <uint8_t t_b, class t_sd_vec>
struct select_support_sd_trait {
    typedef bit_vector::size_type size_type;
    static size_type select(size_type i, const t_sd_vec* v)
    {
        return v->low[i - 1] +							   // lower part of the number
            ((v->high_1_select(i) + 1 - i) << (v->wl)); // upper part
        //^-number of 0 before the i-th 1-^    ^-shift by wl
    }
};

template <class t_sd_vec>
struct select_support_sd_trait<0, t_sd_vec> {
    typedef bit_vector::size_type size_type;
    static size_type select(size_type i, const t_sd_vec* v)
    {
        auto ones = v->low.size();
        assert(0 < i and i <= v->size() - ones);
        size_type lb = 1, rb = ones + 1;
        size_type r0  = 0;
        size_type pos = (size_type)-1;
        // rb exclusive
        // invariant: rank0(select_1(rb)) >= i
        while (lb < rb) {
            auto mid   = lb + (rb - lb) / 2;
            auto x	 = select_support_sd_trait<1, t_sd_vec>::select(mid, v);
            auto rank0 = x + 1 - mid;
            if (rank0 >= i) {
                rb = mid;
            } else {
                r0  = rank0;
                pos = x;
                lb  = mid + 1;
            }
        }
        return pos + i - r0;
    }
};

//! Select data structure for sd_vector
/*! \tparam t_b             Bit pattern.
 *  \tparam t_hi_bit_vector Type of the bitvector used for the unary decoded differences of
 *                          the high part of the positions of the 1s.
 *  \tparam t_select_1      Type of the select structure which is used to select ones in HI.
 *  \tparam t_select_0      Type of the select structure which is used to select zeros in HI.
 */
template <uint8_t t_b, class t_hi_bit_vector, class t_select_1, class t_select_0>
class select_support_sd {
public:
    typedef bit_vector::size_type size_type;
    typedef sd_vector<t_hi_bit_vector, t_select_1, t_select_0> bit_vector_type;
    enum { bit_pat = t_b };
    enum { bit_pat_len = (uint8_t)1 };

private:
    const bit_vector_type* m_v;

public:
    explicit select_support_sd(const bit_vector_type* v = nullptr) { set_vector(v); }

    //! Returns the position of the i-th occurrence in the bit vector.
    size_type select(size_type i) const
    {
        return select_support_sd_trait<t_b, bit_vector_type>::select(i, m_v);
    }

    size_type operator()(size_type i) const { return select(i); }

    size_type size() const { return m_v->size(); }

    void set_vector(const bit_vector_type* v = nullptr) { m_v = v; }

    void load(std::istream&, const bit_vector_type* v = nullptr) { set_vector(v); }

    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        return serialize_empty_object(out, v, name, this);
    }

    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t &) const {}

    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t &) {}

    bool operator==(const select_support_sd& other) const noexcept
    {
        return *m_v == *other.m_v;
    }

    bool operator!=(const select_support_sd& other) const noexcept
    {
        return !(*this == other);
    }
};

//! Select_0 data structure for sd_vector
/*! \tparam t_sd_vector sd_vector type
 *  \tparam t_rank_1    Rank support for high part of sd_vector
 */
template <typename t_sd_vector = sd_vector<>>
class select_0_support_sd {
public:
    typedef bit_vector::size_type size_type;
    typedef t_sd_vector			  bit_vector_type;
    using rank_1	= typename t_sd_vector::rank_1_type;
    using sel0_type = typename t_sd_vector::select_0_type;
    typedef bit_vector y_high_type;
    enum { bit_pat = 0 };
    enum { bit_pat_len = (uint8_t)1 };

private:
    const bit_vector_type* m_v;
    int_vector<>		   m_pointer;
    int_vector<>		   m_rank1;

public:
    explicit select_0_support_sd(const bit_vector_type* v = nullptr)
    {
        set_vector(v);
        if (nullptr != m_v) {
            size_type		rank_0 = 0; // rank0 in H
            const size_type bs	 = 1ULL << (m_v->wl);
            size_type		z	  = 0;
            size_type		rank1  = 0;										 // rank1 in H
            size_type		zeros  = m_v->size() - rank_1(m_v)(m_v->size()); // zeros in B
            m_pointer = int_vector<>(zeros / (64 * bs) + 1, 0, bits::hi(m_v->high.size() / 64) + 1);
            m_rank1   = int_vector<>(m_pointer.size(), 0, bits::hi(m_v->high.size()) + 1);
            uint64_t w = 0;
            for (size_type i = 0, sel0 = 1; i < m_v->high.size(); i += 64) {
                size_type old_rank1 = rank1;
                w					= m_v->high.get_int(i, 64);
                rank1 += bits::cnt(w);
                rank_0 = (i + 64) - rank1;
                if (rank1 > 0 and (w >> 63) & 1) {
                    uint64_t pos =
                        rank_0 * bs + m_v->low[rank1 - 1]; // pos of last one (of previous block in B
                    z = pos + 1 - rank1;
                } else {
                    z = rank_0 * bs - rank1;
                }
                while (sel0 <= z and sel0 <= zeros) {
                    m_pointer[(sel0 - 1) / (64 * bs)] = i / 64;
                    m_rank1[(sel0 - 1) / (64 * bs)]   = old_rank1;
                    sel0 += 64 * bs;
                }
            }
        }
    }

    //! Returns the position of the i-th occurrence in the bit vector.
    size_type select(size_type i) const
    {
        const size_type bs	= 1ULL << (m_v->wl);
        size_type		j	 = m_pointer[(i - 1) / (64 * bs)] * 64; // index into m_high
        size_type		rank1 = m_rank1[(i - 1) / (64 * bs)];		 // rank_1(j*bs*64) in B
        size_type		pos   = 0;
        // size_type		rank0 = 0;

        if (rank1 > 0 and (m_v->high[j - 1]) & 1) {
            pos   = (j - rank1) * bs + m_v->low[rank1 - 1]; // starting position of current block
            // rank0 = pos + 1 - rank1;
        } else {
            pos   = (j - rank1) * bs; // starting position of current block
            // rank0 = pos - rank1;
        }
        uint64_t w = m_v->high.get_int(j, 64);
        do {
            uint64_t _rank1 = rank1 + bits::cnt(w);
            uint64_t _rank0 = 0;
            if (_rank1 > 0 and (w >> 63) & 1) {
                pos	= (j + 64 - _rank1) * bs + m_v->low[_rank1 - 1];
                _rank0 = pos + 1 - _rank1;
            } else {
                pos	= (j + 64 - _rank1) * bs;
                _rank0 = pos - _rank1;
            }
            if (_rank0 < i) {
                j += 64;
                w	 = m_v->high.get_int(j, 64);
                rank1 = _rank1;
            } else {
                break;
            }
        } while (true);
        // invariant i >zeros
        do {
            uint64_t _rank1 = rank1 + bits::lt_cnt[w & 0xFFULL];
            uint64_t _rank0 = 0;
            if (_rank1 > 0 and (w >> 7) & 1) {
                pos	= (j + 8 - _rank1) * bs + m_v->low[_rank1 - 1];
                _rank0 = pos + 1 - _rank1;
            } else {
                pos	= (j + 8 - _rank1) * bs;
                _rank0 = pos - _rank1;
            }
            if (_rank0 < i) {
                j += 8;
                w >>= 8;
                rank1 = _rank1;
            } else {
                break;
            }
        } while (true);

        do {
            bool b = w & 1ULL;
            w >>= 1; // zeros are shifted in
            ++j;
            if (0 == b) {
                pos				= (j - rank1) * bs;
                size_type zeros = pos - rank1;
                if (zeros >= i) {
                    pos = pos - (zeros - i) - 1;
                    break;
                }
            } else {
                pos				  = (j - 1 - rank1) * bs;
                size_type one_pos = pos + m_v->low[rank1];
                ++rank1;
                size_type zeros = one_pos + 1 - rank1;
                if (zeros >= i) {
                    pos = one_pos - (zeros - i) - 1;
                    break;
                }
            }
            if (j % 64 == 0) {
                w = m_v->high.get_int(j, 64);
            }
        } while (true);
        return pos;
    }

    size_type operator()(size_type i) const { return select(i); }

    size_type size() const { return m_v->size(); }

    void set_vector(const bit_vector_type* v = nullptr) { m_v = v; }

    void load(std::istream& in, const bit_vector_type* v = nullptr)
    {
        m_pointer.load(in);
        m_rank1.load(in);
        set_vector(v);
    }

    size_type
    serialize(std::ostream& out, structure_tree_node* v = nullptr, std::string name = "") const
    {
        structure_tree_node* child = structure_tree::add_child(v, name, util::class_name(*this));
        size_type			 written_bytes = 0;
        written_bytes += m_pointer.serialize(out, child, "pointer");
        written_bytes += m_rank1.serialize(out, child, "rank1");
        structure_tree::add_size(child, written_bytes);
        return written_bytes;
    }

    template <typename archive_t>
    void CEREAL_SAVE_FUNCTION_NAME(archive_t & ar) const
    {
        ar(CEREAL_NVP(m_pointer));
        ar(CEREAL_NVP(m_rank1));
    }

    template <typename archive_t>
    void CEREAL_LOAD_FUNCTION_NAME(archive_t & ar)
    {
        ar(CEREAL_NVP(m_pointer));
        ar(CEREAL_NVP(m_rank1));
    }

    bool operator==(const select_0_support_sd& other) const noexcept
    {
        return (m_pointer == other.m_pointer) && (m_rank1 == other.m_rank1);
    }

    bool operator!=(const select_0_support_sd& other) const noexcept
    {
        return !(*this == other);
    }
};

inline sd_vector_builder::sd_vector_builder()
    : m_size(0), m_capacity(0), m_wl(0), m_tail(0), m_items(0), m_last_high(0), m_highpos(0)
{
}

inline sd_vector_builder::sd_vector_builder(size_type n, size_type m)
    : m_size(n), m_capacity(m), m_wl(0), m_tail(0), m_items(0), m_last_high(0), m_highpos(0)
{
    if (m_capacity > m_size) {
        throw std::runtime_error(
            "sd_vector_builder: requested capacity is larger than vector size.");
    }

    std::pair<size_type, size_type> params = sd_vector<>::get_params(m_size, m_capacity);
    m_wl = params.first;
    m_low = int_vector<>(m_capacity, 0, params.first);
    m_high = bit_vector(params.second, 0);
}

template <>
inline sd_vector<>::sd_vector(sd_vector_builder& builder)
{
    if (builder.items() != builder.capacity()) {
        throw std::runtime_error("sd_vector: the builder is not full.");
    }

    m_size = builder.m_size;
    m_wl   = builder.m_wl;
    m_low  = std::move(builder.m_low);
    m_high = std::move(builder.m_high);
    util::init_support(m_high_1_select, &m_high);
    util::init_support(m_high_0_select, &m_high);

    builder = sd_vector_builder();
}

} // end namespace
#endif
