/*
 * Copyright (c) 2022, Romain Bailly
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef RMGR_FIB_SSE_H
#define RMGR_FIB_SSE_H


/*
 * The following macros can be used to control which instruction sets to use:
 *  - RMGR_FIB_ENABLE_SSE2
 *  - RMGR_FIB_ENABLE_SSE3
 *  - RMGR_FIB_ENABLE_SSSE3
 *  - RMGR_FIB_ENABLE_SSE41
 *  - RMGR_FIB_ENABLE_SSE42
 *  - RMGR_FIB_ENABLE_AVX
 *  - RMGR_FIB_ENABLE_FMA
 *  - RMGR_FIB_ENABLE_AVX512F
 *  - RMGR_FIB_ENABLE_AVX512VL
 *  - RMGR_FIB_ENABLE_AVX512DQ
 *
 * If none of the above is defined, auto-configuration will be performed. Auto-configuration is reliable
 * with GCC and Clang but not so much with Visual C++, so you are encouraged to always use manual
 * configuration.
 *
 * If any of the above is defined, manual configuration takes place and the instruction sets are used
 * according to the macros' values (COROLLARY: macros cannot be defined to empty).
 *
 * Of course, enabling a more recent instruction set implies enabling the older ones too (although there
 * are subtleties, watch out). This means that you cannot enable SSE41 and disable SSE3, this would be
 * inconsistent. Checks are made to prevent such inconsistencies but without any warranty.
 */


//=================================================================================================
// Compiler detection

/** @cond cecfb4eb5863e49f4cf59adc01ed0452 */
#ifndef RMGR_COMPILER_IS_NOT_DOXYGEN
    #define RMGR_COMPILER_IS_NOT_DOXYGEN     1
#endif
/** @endcond */

#if !defined(RMGR_COMPILER_IS_NOT_DOXYGEN)
    #define RMGR_COMPILER_IS_DOXYGEN         1

#elif defined(__clang__)
    #ifndef RMGR_COMPILER_IS_CLANG
        #define RMGR_COMPILER_IS_CLANG       1
    #endif
    #ifndef RMGR_COMPILER_VERSION_MAJOR
        #define RMGR_COMPILER_VERSION_MAJOR  __clang_major__
    #endif
    #ifndef RMGR_COMPILER_VERSION_MINOR
        #define RMGR_COMPILER_VERSION_MINOR  __clang_minor__
    #endif
    #ifndef RMGR_COMPILER_VERSION_PATCH
        #define RMGR_COMPILER_VERSION_PATCH  __clang_patchlevel__
    #endif
    #ifndef RMGR_WARNING_PUSH
        #define RMGR_WARNING_PUSH()          _Pragma("clang diagnostic push")
    #endif
    #ifndef RMGR_WARNING_POP
        #define RMGR_WARNING_POP()           _Pragma("clang diagnostic pop")
    #endif
    #ifndef RMGR_WARNING_CLANG_DISABLE
        #define RMGR_WARNING_CLANG_DO_DISABLE(string)  _Pragma(#string)
        #define RMGR_WARNING_CLANG_DISABLE(name)       RMGR_WARNING_CLANG_DO_DISABLE(clang diagnostic ignored name)
    #endif
    #ifndef RMGR_ALIGNED
        #define RMGR_ALIGNED(alignment)      __attribute__((aligned(alignment)))
    #endif

#elif defined(_MSC_VER)
    #ifndef RMGR_COMPILER_IS_MSVC
        #define RMGR_COMPILER_IS_MSVC        1
    #endif
    #ifndef RMGR_COMPILER_VERSION_MAJOR
        #define RMGR_COMPILER_VERSION_MAJOR  (_MSC_VER / 100)
    #endif
    #ifndef RMGR_COMPILER_VERSION_MINOR
        #define RMGR_COMPILER_VERSION_MINOR  (_MSC_VER % 100)
    #endif
    #ifndef RMGR_COMPILER_VERSION_PATCH
        #define RMGR_COMPILER_VERSION_PATCH  (_MSC_FULL_VER % 100000)
    #endif
    #ifndef RMGR_WARNING_PUSH
        #define RMGR_WARNING_PUSH()          __pragma(warning(push))
    #endif
    #ifndef RMGR_WARNING_POP
        #define RMGR_WARNING_POP()           __pragma(warning(pop))
    #endif
    #ifndef RMGR_WARNING_MSVC_DISABLE
        #define RMGR_WARNING_MSVC_DISABLE(number)  __pragma(warning(disable: number))
    #endif
    #ifndef RMGR_ALIGNED
        #define RMGR_ALIGNED(alignment)      __declspec(align(alignment))
    #endif

#elif defined(__GNUC__)
    #ifndef RMGR_COMPILER_IS_GCC
        #define RMGR_COMPILER_IS_GCC         1
    #endif
    #ifndef RMGR_COMPILER_VERSION_MAJOR
        #define RMGR_COMPILER_VERSION_MAJOR  __GNUC__
    #endif
    #ifndef RMGR_COMPILER_VERSION_MINOR
        #define RMGR_COMPILER_VERSION_MINOR  __GNUC_MINOR__
    #endif
    #ifndef RMGR_COMPILER_VERSION_PATCH
        #define RMGR_COMPILER_VERSION_PATCH  __GNUC_PATCHLEVEL__
    #endif
    #ifndef RMGR_WARNING_PUSH
        #define RMGR_WARNING_PUSH()          _Pragma("GCC diagnostic push")
    #endif
    #ifndef RMGR_WARNING_POP
        #define RMGR_WARNING_POP()           _Pragma("GCC diagnostic pop")
    #endif
    #ifndef RMGR_WARNING_GCC_DISABLE
        #define RMGR_WARNING_GCC_DO_DISABLE(string)  _Pragma(#string)
        #define RMGR_WARNING_GCC_DISABLE(name)       RMGR_WARNING_GCC_DO_DISABLE(GCC diagnostic ignored name)
    #endif
#endif

#ifndef RMGR_COMPILER_IS_DOXYGEN
    #define RMGR_COMPILER_IS_DOXYGEN       0 ///< Whether the compiler is Doxygen
#endif
#ifndef RMGR_COMPILER_IS_CLANG
    #define RMGR_COMPILER_IS_CLANG         0 ///< Whether the compiler is Clang
#endif
#ifndef RMGR_COMPILER_IS_MSVC
    #define RMGR_COMPILER_IS_MSVC          0 ///< Whether the compiler is Microsoft Visual C++
#endif
#ifndef RMGR_COMPILER_IS_GCC
    #define RMGR_COMPILER_IS_GCC           0 ///< Whether the compiler is GCC
#endif
#ifndef RMGR_COMPILER_IS_GCC_OR_CLANG
    #define RMGR_COMPILER_IS_GCC_OR_CLANG  (RMGR_COMPILER_IS_GCC || RMGR_COMPILER_IS_CLANG)
#endif
#ifndef RMGR_COMPILER_VERSION_IS_AT_LEAST
    /// Returns non-zero if the compiler version is >= the specified one
    #define RMGR_COMPILER_VERSION_IS_AT_LEAST(major,minor,patch)                                   \
        (    (major) <  RMGR_COMPILER_VERSION_MAJOR                                                \
         || ((major) == RMGR_COMPILER_VERSION_MAJOR && (    (minor) <  RMGR_COMPILER_VERSION_MINOR \
                                                        || ((minor) == RMGR_COMPILER_VERSION_MINOR && (patch)<=RMGR_COMPILER_VERSION_PATCH))))
#endif
#ifndef RMGR_WARNING_PUSH
    #define RMGR_WARNING_PUSH()
#endif
#ifndef RMGR_WARNING_POP
    #define RMGR_WARNING_POP()
#endif
#ifndef RMGR_WARNING_MSVC_DISABLE
    #define RMGR_WARNING_MSVC_DISABLE(number)
#endif
#ifndef RMGR_WARNING_GCC_DISABLE
    #define RMGR_WARNING_GCC_DISABLE(name)
#endif
#ifndef RMGR_WARNING_CLANG_DISABLE
    #define RMGR_WARNING_CLANG_DISABLE(name)
#endif


//=================================================================================================
// Architecture detection

/**
 * @brief Whether the architecture is 32-bit x86 (exclusive of the x32 ABI)
 *
 * @see `#RMGR_ARCH_IS_X86_64`
 * @see `#RMGR_ARCH_IS_X86_ANY`
 */
#ifndef RMGR_ARCH_IS_X86_32
    #if defined(_M_IX86) || defined(__i386__)
        #define RMGR_ARCH_IS_X86_32  1
    #else
        #define RMGR_ARCH_IS_X86_32  0
    #endif
#endif

/**
 * @brief Whether the architecture is 64-bit x86 (inclusive of the x32 ABI)
 *
 * @see `#RMGR_ARCH_IS_X86_32`
 * @see `#RMGR_ARCH_IS_X86_ANY`
 */
#ifndef RMGR_ARCH_IS_X86_64
    #if defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__)
        #define RMGR_ARCH_IS_X86_64  1
    #else
        #define RMGR_ARCH_IS_X86_64  0
    #endif
#endif

/**
 * @brief Whether the architecture is x86, in either 32-bit or 64-bit flavour
 *
 * @see `#RMGR_ARCH_IS_X86_32`
 * @see `#RMGR_ARCH_IS_X86_64`
 */
#ifndef RMGR_ARCH_IS_X86_ANY
    #define RMGR_ARCH_IS_X86_ANY  (RMGR_ARCH_IS_X86_32 || RMGR_ARCH_IS_X86_64)
#endif

#if !RMGR_ARCH_IS_X86_ANY
    #error This file is only intended for x86
#endif


//=================================================================================================
// C++ version

#ifndef RMGR_CPP_VERSION
    #ifdef _MSVC_LANG
        #define RMGR_CPP_VERSION  _MSVC_LANG
    #else
        #define RMGR_CPP_VERSION  __cplusplus
    #endif
#endif

#ifndef RMGR_CPP_VERSION_2011
    #define RMGR_CPP_VERSION_2011  201103L
#endif

#ifndef RMGR_CPP_VERSION_2014
    #define RMGR_CPP_VERSION_2014  201402L
#endif

#ifndef RMGR_CPP_VERSION_2017
    #define RMGR_CPP_VERSION_2017  201703L
#endif

#ifndef RMGR_CPP_VERSION_2020
    #define RMGR_CPP_VERSION_2020  202002L
#endif

#ifndef RMGR_NOEXCEPT
    #if RMGR_CPP_VERSION >= RMGR_CPP_VERSION_2011
        #define RMGR_NOEXCEPT  noexcept
    #else
        #define RMGR_NOEXCEPT
    #endif
#endif


//=================================================================================================
// Instruction set configuration

// Auto-detection
#if    !defined(RMGR_FIB_ENABLE_SSE2) && !defined(RMGR_FIB_ENABLE_SSE3) && !defined(RMGR_FIB_ENABLE_SSSE3)   && !defined(RMGR_FIB_ENABLE_SSE41)    && !defined(RMGR_FIB_ENABLE_SSE42) \
    && !defined(RMGR_FIB_ENABLE_AVX)  && !defined(RMGR_FIB_ENABLE_FMA)  && !defined(RMGR_FIB_ENABLE_AVX512F) && !defined(RMGR_FIB_ENABLE_AVX512VL) && !defined(RMGR_FIB_ENABLE_AVX512DQ)

    #if defined(__SSE2__) || INTERNAL_RMGR_FIB_USE_SSE3
        #define INTERNAL_RMGR_FIB_USE_SSE2      1
    #endif
    #if defined(__SSE3__) || defined(__PMMINTRIN_H) || defined(_INCLUDED_PMM) || defined(_PMMINTRIN_H_INCLUDED) || defined(PMMINTRIN_H_)
        #define INTERNAL_RMGR_FIB_USE_SSE3      1
    #endif
    #if defined(__SSSE3__) || defined(__TMMINTRIN_H) || defined(_INCLUDED_TMM) || defined(_TMMINTRIN_H_INCLUDED) || defined(TMMINTRIN_H_)
        #define INTERNAL_RMGR_FIB_USE_SSSE3     1
    #endif
    #if defined(__SSE41__) || defined(__SMMINTRIN_H) || defined(_INCLUDED_SMM) || defined(_SMMINTRIN_H) || defined(_SMMINTRIN_H_INCLUDED) || defined(SMMINTRIN_H_)
        #define INTERNAL_RMGR_FIB_USE_SSE41     1
    #endif
    #if defined(__SSE42__) || defined(__NMMINTRIN_H) || defined(_INCLUDED_NMM) || defined(_NMMINTRIN_H) || defined(_NMMINTRIN_H_INCLUDED)
        #define INTERNAL_RMGR_FIB_USE_SSE42     1
    #endif
    #if defined(__AVX__) || defined(__IMMINTRIN_H) || defined(_IMMINTRIN_H_INCLUDED) || defined(_INCLUDED_IMM)
        #define INTERNAL_RMGR_FIB_USE_AVX       1
    #endif
    #if defined(__FMA__)
        #define INTERNAL_RMGR_FIB_USE_FMA       1
    #endif
    #if defined(__AVX2__)
        #define INTERNAL_RMGR_FIB_USE_AVX2      1
    #endif
    #if defined(__AVX512F__)
        #define INTERNAL_RMGR_FIB_USE_AVX512F   1
    #endif
    #if defined(__AVX512DQ__)
        #define INTERNAL_RMGR_FIB_USE_AVX512DQ  1
    #endif
    #if defined(__AVX512VL__)
        #define INTERNAL_RMGR_FIB_USE_AVX512VL  1
    #endif

 // Manual configuration
 #else

    #if defined(RMGR_FIB_ENABLE_SSE2)
        #define INTERNAL_RMGR_FIB_USE_SSE2      RMGR_FIB_ENABLE_SSE2
    #endif
    #if defined(RMGR_FIB_ENABLE_SSE3)
        #define INTERNAL_RMGR_FIB_USE_SSE3      RMGR_FIB_ENABLE_SSE3
    #endif
    #if defined(RMGR_FIB_ENABLE_SSSE3)
        #define INTERNAL_RMGR_FIB_USE_SSSE3     RMGR_FIB_ENABLE_SSSE3
    #endif
    #if defined(RMGR_FIB_ENABLE_SSE41)
        #define INTERNAL_RMGR_FIB_USE_SSE41     RMGR_FIB_ENABLE_SSE41
    #endif
    #if defined(RMGR_FIB_ENABLE_SSE42)
        #define INTERNAL_RMGR_FIB_USE_SSE42     RMGR_FIB_ENABLE_SSE42
    #endif
    #if defined(RMGR_FIB_ENABLE_AVX)
        #define INTERNAL_RMGR_FIB_USE_AVX       RMGR_FIB_ENABLE_AVX
    #endif
    #if defined(RMGR_FIB_ENABLE_FMA)
        #define INTERNAL_RMGR_FIB_USE_FMA       RMGR_FIB_ENABLE_AVX512F
    #endif
    #if defined(RMGR_FIB_ENABLE_AVX2)
        #define INTERNAL_RMGR_FIB_USE_AVX2      RMGR_FIB_ENABLE_AVX2
    #endif
    #if defined(RMGR_FIB_ENABLE_AVX512F)
        #define INTERNAL_RMGR_FIB_USE_AVX512F   RMGR_FIB_ENABLE_AVX512F
    #endif
    #if defined(RMGR_FIB_ENABLE_AVX512DQ)
        #define INTERNAL_RMGR_FIB_USE_AVX512DQ  RMGR_FIB_ENABLE_AVX512DQ
    #endif
    #if defined(RMGR_FIB_ENABLE_AVX512VL)
        #define INTERNAL_RMGR_FIB_USE_AVX512VL  RMGR_FIB_ENABLE_AVX512VL
    #endif
#endif


// Instruction set dependencies
#ifndef INTERNAL_RMGR_FIB_USE_AVX512VL
    #define INTERNAL_RMGR_FIB_USE_AVX512VL  0
#endif
#ifndef INTERNAL_RMGR_FIB_USE_AVX512DQ
    #define INTERNAL_RMGR_FIB_USE_AVX512DQ  0
#endif
#ifndef INTERNAL_RMGR_FIB_USE_AVX512F
    #define INTERNAL_RMGR_FIB_USE_AVX512F   (INTERNAL_RMGR_FIB_USE_AVX512DQ || INTERNAL_RMGR_FIB_USE_AVX512VL)
#endif
#ifndef INTERNAL_RMGR_FIB_USE_AVX2
    #define INTERNAL_RMGR_FIB_USE_AVX2      INTERNAL_RMGR_FIB_USE_AVX512F
#endif
#ifndef INTERNAL_RMGR_FIB_USE_FMA
    #define INTERNAL_RMGR_FIB_USE_FMA       0
#endif
#ifndef INTERNAL_RMGR_FIB_USE_AVX
    #define INTERNAL_RMGR_FIB_USE_AVX       (INTERNAL_RMGR_FIB_USE_AVX2 || INTERNAL_RMGR_FIB_USE_FMA)
#endif
#ifndef INTERNAL_RMGR_FIB_USE_SSE42
    #define INTERNAL_RMGR_FIB_USE_SSE42     INTERNAL_RMGR_FIB_USE_AVX
#endif
#ifndef INTERNAL_RMGR_FIB_USE_SSE41
    #define INTERNAL_RMGR_FIB_USE_SSE41     INTERNAL_RMGR_FIB_USE_SSE42
#endif
#ifndef INTERNAL_RMGR_FIB_USE_SSSE3
    #define INTERNAL_RMGR_FIB_USE_SSSE3     INTERNAL_RMGR_FIB_USE_SSE41
#endif
#ifndef INTERNAL_RMGR_FIB_USE_SSE3
    #define INTERNAL_RMGR_FIB_USE_SSE3      INTERNAL_RMGR_FIB_USE_SSSE3
#endif
#ifndef INTERNAL_RMGR_FIB_USE_SSE2
    #define INTERNAL_RMGR_FIB_USE_SSE2      INTERNAL_RMGR_FIB_USE_SSE3
#endif

// Sanity checks
#if INTERNAL_RMGR_FIB_USE_SSE3 && !INTERNAL_RMGR_FIB_USE_SSE2
    #error Configuration error, you cannot enable SSE3 while disabling SSE2
#endif
#if INTERNAL_RMGR_FIB_USE_SSSE3 && !INTERNAL_RMGR_FIB_USE_SSE3
    #error Configuration error, you cannot enable SSSE3 while disabling SSE3
#endif
#if INTERNAL_RMGR_FIB_USE_SSE41 && !INTERNAL_RMGR_FIB_USE_SSSE3
    #error Configuration error, you cannot enable SSE4.1 while disabling SSSE3
#endif
#if INTERNAL_RMGR_FIB_USE_SSE42 && !INTERNAL_RMGR_FIB_USE_SSE41
    #error Configuration error, you cannot enable SSE4.2 while disabling SSE4.1
#endif
#if INTERNAL_RMGR_FIB_USE_AVX && !INTERNAL_RMGR_FIB_USE_SSE42
    #error Configuration error, you cannot enable AVX while disabling SSE4.2
#endif
#if INTERNAL_RMGR_FIB_USE_FMA && !INTERNAL_RMGR_FIB_USE_AVX
    #error Configuration error, you cannot enable FMA while disabling AVX
#endif
#if INTERNAL_RMGR_FIB_USE_AVX2 && !INTERNAL_RMGR_FIB_USE_AVX
    #error Configuration error, you cannot enable AVX2 while disabling AVX
#endif
#if INTERNAL_RMGR_FIB_USE_AVX512F && !INTERNAL_RMGR_FIB_USE_AVX2
    #error Configuration error, you cannot enable AVX512-F while disabling AVX2
#endif
#if INTERNAL_RMGR_FIB_USE_AVX512DQ && !INTERNAL_RMGR_FIB_USE_AVX512F
    #error Configuration error, you cannot enable AVX512-DQ while disabling AVX512-F
#endif
#if INTERNAL_RMGR_FIB_USE_AVX512VL && !INTERNAL_RMGR_FIB_USE_AVX512F
    #error Configuration error, you cannot enable AVX512-VL while disabling AVX512-F
#endif


//=================================================================================================
// Inlining control

#ifndef RMGR_FORCEINLINE
    #if RMGR_COMPILER_IS_MSVC
        #define RMGR_FORCEINLINE  __forceinline
    #elif RMGR_COMPILER_IS_GCC_OR_CLANG
        #define RMGR_FORCEINLINE  inline __attribute__((always_inline))
    #else
        #define RMGR_FORCEINLINE  inline
    #endif
#endif

#ifndef RMGR_NOINLINE
    #if RMGR_COMPILER_IS_MSVC
        #define RMGR_NOINLINE  __declspec(noinline)
    #elif RMGR_COMPILER_IS_GCC_OR_CLANG
        #define RMGR_NOINLINE  __attribute__((noinline))
    #else
        #define RMGR_NOINLINE
    #endif
#endif


//=================================================================================================
// Other stuff

#ifndef RMGR_STATIC_ASSERT
    #if RMGR_CPP_VERSION >= RMGR_CPP_VERSION_2011
        #define RMGR_STATIC_ASSERT(cond)  static_assert(cond, #cond)
    #else
        #define RMGR_STATIC_ASSERT(cond)
    #endif
#endif


//=================================================================================================
// Includes

#if   INTERNAL_RMGR_FIB_USE_SSE42
    #include <nmmintrin.h>
#elif INTERNAL_RMGR_FIB_USE_SSE41
    #include <smmintrin.h>
#elif INTERNAL_RMGR_FIB_USE_SSSE3
    #include <tmmintrin.h>
#elif INTERNAL_RMGR_FIB_USE_SSE3
    #include <pmmintrin.h>
#else
    #include <emmintrin.h>
#endif
#include <cstdint>


//=================================================================================================
// Disable warnings

RMGR_WARNING_PUSH()
RMGR_WARNING_MSVC_DISABLE(4505) // unreferenced function with internal linkage has been removed


//=================================================================================================
// Utility stuff

#if INTERNAL_RMGR_FIB_USE_SSE41
    #define INTERNAL_RMGR_FIB_SELECT(mask, a, b)  _mm_blendv_epi8((b), (a), (mask))
#else
    #define INTERNAL_RMGR_FIB_SELECT(mask, a, b)  _mm_or_si128(_mm_and_si128((mask),(a)), _mm_andnot_si128((mask),(b)))
#endif


//=================================================================================================
// Bitwise NOT and negation

#define _mm_not_si128(a)  _mm_xor_si128((a), _mm_cmpeq_epi32((a),(a))

#define _mm_neg_epi8( a)  _mm_sub_epi8( _mm_setzero_si128(), (a))
#define _mm_neg_epi16(a)  _mm_sub_epi16(_mm_setzero_si128(), (a))
#define _mm_neg_epi32(a)  _mm_sub_epi32(_mm_setzero_si128(), (a))
#define _mm_neg_epi64(a)  _mm_sub_epi64(_mm_setzero_si128(), (a))
#define _mm_neg_epi64(a)  _mm_sub_epi64(_mm_setzero_si128(), (a))
#define _mm_neg_epi64(a)  _mm_sub_epi64(_mm_setzero_si128(), (a))
#define _mm_neg_ps(   a)  _mm_sub_ps(   _mm_setzero_ps(),    (a))
#define _mm_neg_pd(   a)  _mm_sub_pd(   _mm_setzero_pd(),    (a))


//=================================================================================================
// 32-bit x86 compat layer

#if RMGR_ARCH_IS_X86_32
    #define _mm_set_epi64x(e1,e0)  _mm_set_epi32(int32_t(int64_t(e1)>>32), int32_t(int64_t(e1)&UINT64_C(0xFFFFFFFF)), int32_t(int64_t(e0)>>32), int32_t(int64_t(e0)&UINT64_C(0xFFFFFFFF)))
    #define _mm_set1_epi64x(a)     _mm_set_epi64x((a), (a))
    #define _mm_cvtsi128_si64(a)   int64_t((uint64_t(_mm_extract_epi32((a),1)) << 32) | uint32_t(_mm_cvtsi128_si32(a)))
#endif


//=================================================================================================
// Lane access

#if !INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_extract_epi8( a, imm8)  ((_mm_extract_epi16((a), (imm8)/2) >> (((imm8)%2)*8)) & 255)
    #define _mm_extract_epi32(a, imm8)  _mm_cvtsi128_si32(_mm_srli_si128((a), (imm8)*4))
    #define _mm_extract_epi64(a, imm8)  _mm_cvtsi128_si64(_mm_srli_si128((a), (imm8)*8))
#endif

#if RMGR_ARCH_IS_X86_32 && INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_extract_epi64(a, imm8)  int64_t((uint64_t(uint32_t(_mm_extract_epi32((a),2*(imm8)+1))) << 32) | uint32_t(_mm_extract_epi32((a), 2*(imm8))))
#endif


//=================================================================================================
// Comparisons

// 8-bit signed
#define _mm_cmpneq_epi8(a,b)   _mm_xor_si128(_mm_cmpeq_epi8((a),(b)), _mm_cmpeq_epi8((a),(a)))
#define _mm_cmpge_epi8(a,b)    _mm_xor_si128(_mm_cmpgt_epi8((b),(a)), _mm_cmpeq_epi8((a),(a)))
#define _mm_cmple_epi8(a,b)    _mm_cmpge_epi8((b), (a))

// 8-bit unsigned
#define _mm_cmpeq_epu8         _mm_cmpeq_epi8
#define _mm_cmpneq_epu8        _mm_cmpneq_epi8
#define _mm_cmpge_epu8(a,b)    _mm_cmpeq_epi8((a), _mm_max_epu8((a), (b)))
#define _mm_cmple_epu8(a,b)    _mm_cmpge_epu8((b), (a))
#define _mm_cmpgt_epu8(a,b)    _mm_xor_si128(_mm_cmpge_epu8((b),(a)), _mm_cmpeq_epi8((a),(a)))
#define _mm_cmplt_epu8(a,b)    _mm_cmpgt_epu8((b), (a))

// 16-bit signed
#define _mm_cmpneq_epi16(a,b)  _mm_xor_si128(_mm_cmpeq_epi16((a),(b)), _mm_cmpeq_epi16((a),(a)))
#define _mm_cmpge_epi16(a,b)   _mm_xor_si128(_mm_cmpgt_epi16((b),(a)), _mm_cmpeq_epi16((a),(a)))
#define _mm_cmple_epi16(a,b)   _mm_cmpge_epi16((b), (a))

// 16-bit unsigned
#define _mm_cmpeq_epu16        _mm_cmpeq_epi16
#define _mm_cmpneq_epu16       _mm_cmpneq_epi16
#define _mm_cmple_epu16(a,b)   _mm_cmpge_epu16((b), (a))
#define _mm_cmplt_epu16(a,b)   _mm_cmpgt_epu16((b), (a))
#if INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_cmpge_epu16(a,b)    _mm_cmpeq_epi16((a), _mm_max_epu16((a), (b)))
    #define _mm_cmpgt_epu16(a,b)    _mm_xor_si128(_mm_cmpge_epu16((b),(a)), _mm_cmpeq_epi16((a),(a)))
#else
    #define _mm_cmpgt_epu16         rmgr_fib_mm_cmpgt_epu16
    #define _mm_cmpge_epu16(a,b)    _mm_cmpeq_epi16(_mm_subs_epu16((b),(a)), _mm_setzero_si128())
    static inline __m128i rmgr_fib_mm_cmpgt_epu16(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        const __m128i flip = _mm_set1_epi16(0x8000u);
        return _mm_cmpgt_epi16(_mm_xor_si128(a,flip), _mm_xor_si128(b,flip));
    }
#endif

// 32-bit signed
#define _mm_cmpneq_epi32(a,b)  _mm_xor_si128(_mm_cmpeq_epi32((a),(b)), _mm_cmpeq_epi32((a),(a)))
#define _mm_cmpge_epi32(a,b)   _mm_xor_si128(_mm_cmpgt_epi32((b),(a)), _mm_cmpeq_epi32((a),(a)))
#define _mm_cmple_epi32(a,b)   _mm_cmpge_epi32((b), (a))

// 32-bit unsigned
#define _mm_cmpeq_epu32        _mm_cmpeq_epi32
#define _mm_cmpneq_epu32       _mm_cmpneq_epi32
#define _mm_cmple_epu32(a,b)   _mm_cmpge_epu32((b), (a))
#define _mm_cmplt_epu32(a,b)   _mm_cmpgt_epu32((b), (a))
#if INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_cmpge_epu32(a,b)    _mm_cmpeq_epi32((a), _mm_max_epu32((a), (b)))
    #define _mm_cmpgt_epu32(a,b)    _mm_xor_si128(_mm_cmpge_epu32((b),(a)), _mm_cmpeq_epi32((a),(a)))
#else
    #define _mm_cmpgt_epu32         rmgr_fib_mm_cmpgt_epu32
    #define _mm_cmpge_epu32(a,b)    _mm_xor_si128(_mm_cmpgt_epu32((b),(a)), _mm_cmpeq_epi32((a),(a)))
    static inline __m128i rmgr_fib_mm_cmpgt_epu32(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        const __m128i flip = _mm_set1_epi32(0x80000000u);
        return _mm_cmpgt_epi32(_mm_xor_si128(a,flip), _mm_xor_si128(b,flip));
    }
#endif

// 64-bit signed
#define _mm_cmpneq_epi64(a,b)  _mm_xor_si128(_mm_cmpeq_epi64((a),(b)), _mm_cmpeq_epi32((a),(a)))
#define _mm_cmpge_epi64(a,b)   _mm_xor_si128(_mm_cmpgt_epi64((b),(a)), _mm_cmpeq_epi32((a),(a)))
#define _mm_cmplt_epi64(a,b)   _mm_cmpgt_epi64((b), (a))
#define _mm_cmple_epi64(a,b)   _mm_cmpge_epi64((b), (a))
#if !INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_cmpeq_epi64    rmgr_fib_mm_cmpeq_epi64
    static inline __m128i rmgr_fib_mm_cmpeq_epi64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        const __m128i eq32 = _mm_cmpeq_epi32(a,b);
        return _mm_and_si128(eq32, _mm_shuffle_epi32(eq32, _MM_SHUFFLE(2,3,0,1)));
    }
#endif
#if !INTERNAL_RMGR_FIB_USE_SSE42
    #define _mm_cmpgt_epi64   rmgr_fib_mm_cmpgt_epi64
    static inline __m128i rmgr_fib_mm_cmpgt_epi64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        // See https://stackoverflow.com/a/65460455
        const __m128i s = _mm_xor_si128(a, b);
        const __m128i d = _mm_sub_epi64(b, a);
        const __m128i r = INTERNAL_RMGR_FIB_SELECT(s, b, d);
        return _mm_shuffle_epi32(_mm_srai_epi32(r,31), _MM_SHUFFLE(3,3,1,1));
    }
#endif

// 64-bit unsigned
#define _mm_cmpeq_epu64        _mm_cmpeq_epi64
#define _mm_cmpneq_epu64       _mm_cmpneq_epi64
#define _mm_cmpge_epu64(a,b)   _mm_xor_si128(_mm_cmpgt_epu64((b),(a)), _mm_cmpeq_epi32((a),(a)))
#define _mm_cmplt_epu64(a,b)   _mm_cmpgt_epu64((b), (a))
#define _mm_cmple_epu64(a,b)   _mm_cmpge_epu64((b), (a))
static inline __m128i _mm_cmpgt_epu64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
{
#if INTERNAL_RMGR_FIB_USE_SSE42
    const __m128i flip = _mm_set1_epi64x(0x8000000000000000ll);
    return _mm_cmpgt_epi64(_mm_xor_si128(a,flip), _mm_xor_si128(b,flip));
#else
    // See https://stackoverflow.com/a/65460455
    const __m128i s = _mm_xor_si128(a, b);
    const __m128i d = _mm_sub_epi64(b, a);
    const __m128i r = INTERNAL_RMGR_FIB_SELECT(s, a, d);
    return _mm_shuffle_epi32(_mm_srai_epi32(r,31), _MM_SHUFFLE(3,3,1,1));
#endif
}


//=================================================================================================
// Shifts

// 8-bit shifts
#define _mm_slli_epi8(a, imm8)   _mm_and_si128(_mm_slli_epi16((a),(imm8)), _mm_set1_epi8(uint8_t(255 & (255<<((imm8)&7)))))
#define _mm_srli_epi8(a, imm8)   _mm_and_si128(_mm_srli_epi16((a),(imm8)), _mm_set1_epi8(int8_t(255u>>((imm8)&7))))
#define _mm_srai_epi8(a, imm8)   _mm_packs_epi16(_mm_srai_epi16(_mm_unpacklo_epi8((a),(a)), (imm8)+8), _mm_srai_epi16(_mm_unpackhi_epi8((a),(a)), (imm8)+8))

static inline __m128i _mm_sll_epi8(const __m128i& a, const __m128i& count) RMGR_NOEXCEPT
{
    const __m128i twoFiftySix = _mm_set1_epi16(256);
    const __m128i mask        = _mm_sub_epi16(_mm_sll_epi16(twoFiftySix, count), twoFiftySix);
    return _mm_andnot_si128(mask, _mm_sll_epi16(a, count));
}

static inline __m128i _mm_srl_epi8(const __m128i& a, const __m128i& count) RMGR_NOEXCEPT
{
    const __m128i twoFiftySix = _mm_set1_epi16(256);
    const __m128i mask        = _mm_sub_epi16(twoFiftySix, _mm_srl_epi16(twoFiftySix, count));
    return _mm_andnot_si128(mask, _mm_srl_epi16(a, count));
}

static inline __m128i _mm_sra_epi8(const __m128i& a, const __m128i& count) RMGR_NOEXCEPT
{
    const __m128i count16 = _mm_add_epi64(count, _mm_set1_epi64x(8));
    return _mm_packs_epi16(_mm_sra_epi16(_mm_unpacklo_epi8(a,a), count16), _mm_sra_epi16(_mm_unpackhi_epi8(a,a), count16));
}


// 64-bit arithmetic right shift
#if !INTERNAL_RMGR_FIB_USE_AVX512VL
    #define _mm_srai_epi64(a, imm8)  rmgr_fib_mm_srai_epi64<(imm8)>(a)
    #define _mm_sra_epi64            rmgr_fib_mm_sra_epi64

    static __m128i rmgr_fib_mm_sra_epi64(const __m128i& a, const __m128i& count) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE42
            const __m128i sign = _mm_cmpgt_epi64(_mm_setzero_si128(), a);
        #else
            const __m128i sign = _mm_shuffle_epi32(_mm_cmpgt_epi32(_mm_setzero_si128(), a), _MM_SHUFFLE(3,3,1,1));
        #endif
        return _mm_or_si128(_mm_srl_epi64(a,count), _mm_sll_epi64(sign,_mm_sub_epi64(_mm_set1_epi64x(63),count)));
    }

    template<unsigned N>
    static __m128i rmgr_fib_mm_srai_epi64(const __m128i& a) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE42
            const __m128i sign = _mm_cmpgt_epi64(_mm_setzero_si128(), a);
        #else
            const __m128i sign = _mm_shuffle_epi32(_mm_cmpgt_epi32(_mm_setzero_si128(), a), _MM_SHUFFLE(3,3,1,1));
        #endif
        return _mm_or_si128(_mm_srli_epi64(a,N), _mm_slli_epi64(sign,63-N));
    }

    template<>
    __m128i rmgr_fib_mm_srai_epi64<0u>(const __m128i& a) RMGR_NOEXCEPT
    {
        return a;
    }

    template<>
    __m128i rmgr_fib_mm_srai_epi64<63u>(const __m128i& a) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE42
            return _mm_cmpgt_epi64(_mm_setzero_si128(), a);
        #else
            return _mm_shuffle_epi32(_mm_cmpgt_epi32(_mm_setzero_si128(), a), _MM_SHUFFLE(3,3,1,1));
        #endif
    }
#endif


//=================================================================================================
// Min & max

// 8-bit signed
#if !INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_min_epi8   rmgr_fib_mm_min_epi8
    #define _mm_max_epi8   rmgr_fib_mm_max_epi8

    static inline __m128i rmgr_fib_mm_min_epi8(const __m128i& a, const __m128i b) RMGR_NOEXCEPT
    {
        const __m128i mask = _mm_cmpgt_epi8(a, b);
        return INTERNAL_RMGR_FIB_SELECT(mask, b, a);
    }

    static inline __m128i rmgr_fib_mm_max_epi8(const __m128i& a, const __m128i b) RMGR_NOEXCEPT
    {
        const __m128i mask = _mm_cmpgt_epi8(a, b);
        return INTERNAL_RMGR_FIB_SELECT(mask, a, b);
    }
#endif

// 16-bit unsigned
#if !INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_min_epu16(a,b)  _mm_sub_epi16((a), _mm_subs_epu16((a),(b)))
    #define _mm_max_epu16(a,b)  _mm_add_epi16((b), _mm_subs_epu16((a),(b)))
#endif

// 32-bit unsigned
#if !INTERNAL_RMGR_FIB_USE_SSE41
    #define _mm_min_epu32   rmgr_fib_mm_min_epu32
    #define _mm_max_epu32   rmgr_fib_mm_max_epu32

    static inline __m128i rmgr_fib_mm_min_epu32(const __m128i& a, const __m128i b) RMGR_NOEXCEPT
    {
        const __m128i mask = _mm_cmpgt_epu32(a, b);
        return INTERNAL_RMGR_FIB_SELECT(mask, b, a);
    }

    static inline __m128i rmgr_fib_mm_max_epu32(const __m128i& a, const __m128i b) RMGR_NOEXCEPT
    {
        const __m128i mask = _mm_cmpgt_epu32(a, b);
        return INTERNAL_RMGR_FIB_SELECT(mask, a, b);
    }
#endif

// 64-bit signed
#if !INTERNAL_RMGR_FIB_USE_AVX512VL
    #define _mm_min_epi64  rmgr_fib_mm_min_epi64
    #define _mm_max_epi64  rmgr_fib_mm_max_epi64

    static inline __m128i rmgr_fib_mm_min_epi64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE41
            // _mm_blendv_pd() allows us to save two instructions compared to calling _mm_cmpgt_epi64()
            const __m128i s = _mm_xor_si128(a, b);
            const __m128i d = _mm_sub_epi64(b, a);
            const __m128i m = _mm_blendv_epi8(d, b, s);
            return _mm_castpd_si128(_mm_blendv_pd(_mm_castsi128_pd(a), _mm_castsi128_pd(b), _mm_castsi128_pd(m)));
        #else
            const __m128i m = _mm_cmpgt_epi64(a, b);
            return INTERNAL_RMGR_FIB_SELECT(m, b, a);
        #endif
    }

    static inline __m128i rmgr_fib_mm_max_epi64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE41
            // _mm_blendv_pd() allows us to save two instructions compared to calling _mm_cmpgt_epi64()
            const __m128i s = _mm_xor_si128(a, b);
            const __m128i d = _mm_sub_epi64(b, a);
            const __m128i m = _mm_blendv_epi8(d, b, s);
            return _mm_castpd_si128(_mm_blendv_pd(_mm_castsi128_pd(b), _mm_castsi128_pd(a), _mm_castsi128_pd(m)));
        #else
            const __m128i m = _mm_cmpgt_epi64(a, b);
            return INTERNAL_RMGR_FIB_SELECT(m, a, b);
        #endif
    }
#endif

// 64-bit unsigned
#if !INTERNAL_RMGR_FIB_USE_AVX512VL
    #define _mm_min_epu64  rmgr_fib_mm_min_epu64
    #define _mm_max_epu64  rmgr_fib_mm_max_epu64

    static inline __m128i rmgr_fib_mm_min_epu64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE41
            // _mm_blendv_pd() allows us to save two instructions compared to calling _mm_cmpgt_epu64()
            const __m128i s = _mm_xor_si128(a, b);
            const __m128i d = _mm_sub_epi64(b, a);
            const __m128i m = _mm_blendv_epi8(d, a, s);
            return _mm_castpd_si128(_mm_blendv_pd(_mm_castsi128_pd(a), _mm_castsi128_pd(b), _mm_castsi128_pd(m)));
        #else
            const __m128i m = _mm_cmpgt_epu64(a, b);
            return INTERNAL_RMGR_FIB_SELECT(m, b, a);
        #endif
    }

    static inline __m128i rmgr_fib_mm_max_epu64(const __m128i& a, const __m128i& b) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE41
            // _mm_blendv_pd() allows us to save two instructions compared to calling _mm_cmpgt_epu64()
            const __m128i s = _mm_xor_si128(a, b);
            const __m128i d = _mm_sub_epi64(b, a);
            const __m128i m = _mm_blendv_epi8(d, a, s);
            return _mm_castpd_si128(_mm_blendv_pd(_mm_castsi128_pd(b), _mm_castsi128_pd(a), _mm_castsi128_pd(m)));
        #else
            const __m128i m = _mm_cmpgt_epu64(a, b);
            return INTERNAL_RMGR_FIB_SELECT(m, a, b);
        #endif
    }
#endif


//=================================================================================================
// Absolute value

// 8, 16 & 32-bit
#if !INTERNAL_RMGR_FIB_USE_SSSE3
    #define _mm_abs_epi8   rmgr_fib_mm_abs_epi8
    #define _mm_abs_epi16  rmgr_fib_mm_abs_epi16
    #define _mm_abs_epi32  rmgr_fib_mm_abs_epi32

    static inline __m128i rmgr_fib_mm_abs_epi8(const __m128i& a) RMGR_NOEXCEPT
    {
        const __m128i s = _mm_cmpgt_epi8(_mm_setzero_si128(), a);
        return _mm_sub_epi8(_mm_xor_si128(a, s), s);
    }

    static inline __m128i rmgr_fib_mm_abs_epi16(const __m128i& a) RMGR_NOEXCEPT
    {
        const __m128i s = _mm_srai_epi16(a, 15);
        return _mm_sub_epi16(_mm_xor_si128(a, s), s);
    }

    static inline __m128i rmgr_fib_mm_abs_epi32(const __m128i& a) RMGR_NOEXCEPT
    {
        const __m128i s = _mm_srai_epi32(a, 31);
        return _mm_sub_epi32(_mm_xor_si128(a, s), s);
    }
#endif

// 64-bit
#if !INTERNAL_RMGR_FIB_USE_AVX512VL
    #define _mm_abs_epi64  rmgr_fib_mm_abs_epi64

    static inline __m128i rmgr_fib_mm_abs_epi64(const __m128i& a) RMGR_NOEXCEPT
    {
        #if INTERNAL_RMGR_FIB_USE_SSE42
            const __m128i s = _mm_cmpgt_epi64(_mm_setzero_si128(), a);
        #else
            const __m128i s = _mm_shuffle_epi32(_mm_srai_epi32(a,31), _MM_SHUFFLE(3,3,1,1));
        #endif
        return _mm_sub_epi64(_mm_xor_si128(a, s), s);
    }
#endif

// floating point
#define _mm_abs_ps(a)  _mm_and_ps((a), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFFu)))
#define _mm_abs_pd(a)  _mm_and_pd((a), _mm_castsi128_pd(_mm_set1_epi64x(UINT64_C(0x7FFFFFFFFFFFFFFF))))


RMGR_WARNING_POP()


#endif // RMGR_FIB_SSE_H
