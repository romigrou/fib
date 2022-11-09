Fill in the Blanks
==================

SIMD instruction sets are powerful but lacking. Not all operations are supported on all type sizes
and/or signednesses. In particular, the SSE matrix of sizes and instructions is very sparse. My
favourite being the 64-bit comparison instructions that made it quite late and even took the luxury
to be split over two SSE revisions (4.1 and 4.2).

This repository contains an attempt to fill in the blanks by emulating the missing intrinsics.

SSE Intrinsics
==============

Here's the list of emulated intrinsics. Of course, emulation is disabled when an intrinsic is natively
supported, as indicated in the middle column.

| Intrinsic         | Native Support | Description                               |
|-------------------|----------------|-------------------------------------------|
| _mm_set_epi64x    | x64            | Set 64-bit lanes                          |
| _mm_set1_epi64x   | x64            | Set all 64-bit lanes to the same value    |
| _mm_cvtsi128_si64 | x64            | Retrieve first 64-bit lane                |
| _mm_extract_epi8  | SSE 4.1        | Retrieve an 8-bit lane                    |
| _mm_extract_epi32 | SSE 4.1        | Retrieve a 32-bit lane                    |
| _mm_extract_epi64 | SSE 4.1 + x64  | Retrieve a 64-bit lane                    |
| _mm_not_si128     |                | Bitwise not                               |
| _mm_neg_epi8      |                | Sign change                               |
| _mm_neg_epi16     |                | Sign change                               |
| _mm_neg_epi32     |                | Sign change                               |
| _mm_neg_epi64     |                | Sign change                               |
| _mm_neg_epi64     |                | Sign change                               |
| _mm_neg_epi64     |                | Sign change                               |
| _mm_neg_ps        |                | Sign change                               |
| _mm_neg_pd        |                | Sign change                               |
| _mm_cmpneq_epi8   |                | `!=` signed 8-bit comparison              |
| _mm_cmpge_epi8    |                | `>=` signed 8-bit comparison              |
| _mm_cmple_epi8    |                | `<=` signed 8-bit comparison              |
| _mm_cmpeq_epu8    |                | `==` unsigned 8-bit comparison            |
| _mm_cmpneq_epu8   |                | `!=` unsigned 8-bit comparison            |
| _mm_cmplt_epu8    |                | `< ` unsigned 8-bit comparison            |
| _mm_cmple_epu8    |                | `<=` unsigned 8-bit comparison            |
| _mm_cmpgt_epu8    |                | `> ` unsigned 8-bit comparison            |
| _mm_cmpge_epu8    |                | `>=` unsigned 8-bit comparison            |
| _mm_cmpneq_epi16  |                | `!=` signed 16-bit comparison             |
| _mm_cmpge_epi16   |                | `>=` signed 16-bit comparison             |
| _mm_cmple_epi16   |                | `<=` signed 16-bit comparison             |
| _mm_cmpeq_epu16   |                | `==` unsigned 16-bit comparison           |
| _mm_cmpneq_epu16  |                | `!=` unsigned 16-bit comparison           |
| _mm_cmplt_epu16   |                | `< ` unsigned 16-bit comparison           |
| _mm_cmple_epu16   |                | `<=` unsigned 16-bit comparison           |
| _mm_cmpgt_epu16   |                | `> ` unsigned 16-bit comparison           |
| _mm_cmpge_epu16   |                | `>=` unsigned 16-bit comparison           |
| _mm_cmpneq_epi32  |                | `!=` signed 32-bit comparison             |
| _mm_cmpge_epi32   |                | `>=` signed 32-bit comparison             |
| _mm_cmple_epi32   |                | `<=` signed 32-bit comparison             |
| _mm_cmpeq_epu32   |                | `==` unsigned 32-bit comparison           |
| _mm_cmpneq_epu32  |                | `!=` unsigned 32-bit comparison           |
| _mm_cmplt_epu32   |                | `< ` unsigned 32-bit comparison           |
| _mm_cmple_epu32   |                | `<=` unsigned 32-bit comparison           |
| _mm_cmpgt_epu32   |                | `> ` unsigned 32-bit comparison           |
| _mm_cmpge_epu32   |                | `>=` unsigned 32-bit comparison           |
| _mm_cmpeq_epi64   | SSE 4.1        | `==` signed 64-bit comparison             |
| _mm_cmpneq_epi64  |                | `!=` signed 64-bit comparison             |
| _mm_cmplt_epi64   |                | `< ` signed 64-bit comparison             |
| _mm_cmple_epi64   |                | `<=` signed 64-bit comparison             |
| _mm_cmpgt_epi64   | SSE 4.2        | `> ` signed 64-bit comparison             |
| _mm_cmpge_epi64   |                | `>=` signed 64-bit comparison             |
| _mm_cmpeq_epu64   |                | `==` unsigned 64-bit comparison           |
| _mm_cmpneq_epu64  |                | `!=` unsigned 64-bit comparison           |
| _mm_cmplt_epu64   |                | `< ` unsigned 64-bit comparison           |
| _mm_cmple_epu64   |                | `<=` unsigned 64-bit comparison           |
| _mm_cmpgt_epu64   |                | `> ` unsigned 64-bit comparison           |
| _mm_cmpge_epu64   |                | `>=` unsigned 64-bit comparison           |
| _mm_slli_epi8     |                | 8-bit logical left shift by constant      |
| _mm_srli_epi8     |                | 8-bit logical right shift by constant     |
| _mm_srai_epi8     |                | 8-bit arithmetic right shift by constant  |
| _mm_sll_epi8      |                | 8-bit logical left shift by variable      |
| _mm_srl_epi8      |                | 8-bit logical right shift by variable     |
| _mm_sra_epi8      |                | 8-bit arithmetic right shift by variable  |
| _mm_srai_epi64    | AVX512-VL      | 64-bit arithmetic right shift by constant |
| _mm_sra_epi64     | AVX512-VL      | 64-bit arithmetic right shift by variable |
| _mm_min_epi8      | SSE 4.1        | 8-bit signed min                          |
| _mm_max_epi8      | SSE 4.1        | 8-bit signed max                          |
| _mm_min_epu16     | SSE 4.1        | 16-bit unsigned min                       |
| _mm_max_epu16     | SSE 4.1        | 16-bit unsigned max                       |
| _mm_min_epu32     | SSE 4.1        | 32-bit unsigned min                       |
| _mm_max_epu32     | SSE 4.1        | 32-bit unsigned max                       |
| _mm_min_epi64     | AVX512-VL      | 64-bit signed min                         |
| _mm_max_epi64     | AVX512-VL      | 64-bit signed max                         |
| _mm_min_epu64     | AVX512-VL      | 64-bit unsigned min                       |
| _mm_max_epu64     | AVX512-VL      | 64-bit unsigned max                       |
