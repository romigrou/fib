#include <rmgr/fib/sse.h>
#include <gtest/gtest.h>


TEST(IS, epi8_extract)
{
    const __m128i v = _mm_set_epi8(-128,-127,-65,-64,-63,-2,-1,0,1,2,3,63,64,65,126,127);
    ASSERT_EQ(    127, _mm_extract_epi8(v,0));
    ASSERT_EQ(    126, _mm_extract_epi8(v,1));
    ASSERT_EQ(     65, _mm_extract_epi8(v,2));
    ASSERT_EQ(     64, _mm_extract_epi8(v,3));
    ASSERT_EQ(     63, _mm_extract_epi8(v,4));
    ASSERT_EQ(      3, _mm_extract_epi8(v,5));
    ASSERT_EQ(      2, _mm_extract_epi8(v,6));
    ASSERT_EQ(      1, _mm_extract_epi8(v,7));
    ASSERT_EQ(      0, _mm_extract_epi8(v,8));
    ASSERT_EQ(256  -1, _mm_extract_epi8(v,9));
    ASSERT_EQ(256  -2, _mm_extract_epi8(v,10));
    ASSERT_EQ(256 -63, _mm_extract_epi8(v,11));
    ASSERT_EQ(256 -64, _mm_extract_epi8(v,12));
    ASSERT_EQ(256 -65, _mm_extract_epi8(v,13));
    ASSERT_EQ(256-127, _mm_extract_epi8(v,14));
    ASSERT_EQ(256-128, _mm_extract_epi8(v,15));
}


TEST(IS, epi32_extract)
{
    const __m128i v = _mm_set_epi32(INT32_MIN,-1,1,INT32_MAX);
    ASSERT_EQ(INT32_MAX, _mm_extract_epi32(v,0));
    ASSERT_EQ( 1,        _mm_extract_epi32(v,1));
    ASSERT_EQ(-1,        _mm_extract_epi32(v,2));
    ASSERT_EQ(INT32_MIN, _mm_extract_epi32(v,3));
}


TEST(IS, epi64_extract)
{
    const __m128i a = _mm_set_epi64x(INT64_MIN,-1);
    const __m128i b = _mm_set_epi64x(1,INT64_MAX);
    ASSERT_EQ(-1,        _mm_cvtsi128_si64(a));
    ASSERT_EQ(-1,        _mm_extract_epi64(a,0));
    ASSERT_EQ(INT64_MIN, _mm_extract_epi64(a,1));
    ASSERT_EQ(INT64_MAX, _mm_extract_epi64(b,0));
    ASSERT_EQ(1,         _mm_extract_epi64(b,1));
}


enum Comparison
{
    COMP_EQ,
    COMP_NE,
    COMP_LT,
    COMP_LE,
    COMP_GT,
    COMP_GE
};

template<typename T>
static RMGR_NOINLINE bool compare(T a, T b, Comparison comp)
{
    switch (comp)
    {
        case COMP_EQ: return (a == b);
        case COMP_NE: return (a != b);
        case COMP_LT: return (a <  b);
        case COMP_LE: return (a <= b);
        case COMP_GT: return (a >  b);
        case COMP_GE: return (a >= b);
    }
    return false;
}


template<typename Scalar>
static void store(Scalar buffer[], const __m128i& v)
{
    _mm_storeu_si128(reinterpret_cast<__m128i*>(buffer), v);
}


static void store(float buffer[], const __m128& v)
{
    _mm_store_ps(buffer, v);
}


static void store(double buffer[], const __m128d& v)
{
    _mm_storeu_pd(buffer, v);
}


template<typename Scalar, typename Vector>
static RMGR_NOINLINE void assert_comparison(const Vector& a, const Vector& b, const Vector& res, Comparison comp)
{
    const size_t length = sizeof(Vector) / sizeof(Scalar);
    Scalar bufA[length];
    Scalar bufB[length];
    Scalar bufR[length];
    store(bufA, a);
    store(bufB, b);
    store(bufR, res);
    for (size_t i=0; i<length; ++i)
    {
        ASSERT_EQ((bufR[i]!=0), compare(bufA[i], bufB[i], comp));
    }
}


TEST(IS, epi8_comparisons)
{
    const __m128i a = _mm_set_epi8(-128,-128,-127,-127,-1,-1,0,0,1,1,2,2,126,126,127,127);
    const __m128i b = _mm_set_epi8(-128,-127,-128,-127,-2,-1,1,0,2,1,1,2,127,126,126,127);
    assert_comparison<int8_t>(a, b, _mm_cmpeq_epi8(a,b),  COMP_EQ);
    assert_comparison<int8_t>(b, a, _mm_cmpeq_epi8(b,a),  COMP_EQ);
    assert_comparison<int8_t>(a, b, _mm_cmpneq_epi8(a,b), COMP_NE);
    assert_comparison<int8_t>(b, a, _mm_cmpneq_epi8(b,a), COMP_NE);
    assert_comparison<int8_t>(a, b, _mm_cmplt_epi8(a,b),  COMP_LT);
    assert_comparison<int8_t>(b, a, _mm_cmplt_epi8(b,a),  COMP_LT);
    assert_comparison<int8_t>(a, b, _mm_cmple_epi8(a,b),  COMP_LE);
    assert_comparison<int8_t>(b, a, _mm_cmple_epi8(b,a),  COMP_LE);
    assert_comparison<int8_t>(a, b, _mm_cmpgt_epi8(a,b),  COMP_GT);
    assert_comparison<int8_t>(b, a, _mm_cmpgt_epi8(b,a),  COMP_GT);
    assert_comparison<int8_t>(a, b, _mm_cmpge_epi8(a,b),  COMP_GE);
    assert_comparison<int8_t>(b, a, _mm_cmpge_epi8(b,a),  COMP_GE);
}


TEST(IS, epu8_comparisons)
{
    const __m128i a = _mm_set_epi8(-128,-128,-127,-127,-1,-1,0,0,1,1,2,2,126,126,127,127);
    const __m128i b = _mm_set_epi8(-128,-127,-128,-127,-2,-1,1,0,2,1,1,2,127,126,126,127);
    assert_comparison<uint8_t>(a, b, _mm_cmpeq_epu8(a,b),  COMP_EQ);
    assert_comparison<uint8_t>(b, a, _mm_cmpeq_epu8(b,a),  COMP_EQ);
    assert_comparison<uint8_t>(a, b, _mm_cmpneq_epu8(a,b), COMP_NE);
    assert_comparison<uint8_t>(b, a, _mm_cmpneq_epu8(b,a), COMP_NE);
    assert_comparison<uint8_t>(a, b, _mm_cmplt_epu8(a,b),  COMP_LT);
    assert_comparison<uint8_t>(b, a, _mm_cmplt_epu8(b,a),  COMP_LT);
    assert_comparison<uint8_t>(a, b, _mm_cmple_epu8(a,b),  COMP_LE);
    assert_comparison<uint8_t>(b, a, _mm_cmple_epu8(b,a),  COMP_LE);
    assert_comparison<uint8_t>(a, b, _mm_cmpgt_epu8(a,b),  COMP_GT);
    assert_comparison<uint8_t>(b, a, _mm_cmpgt_epu8(b,a),  COMP_GT);
    assert_comparison<uint8_t>(a, b, _mm_cmpge_epu8(a,b),  COMP_GE);
    assert_comparison<uint8_t>(b, a, _mm_cmpge_epu8(b,a),  COMP_GE);
}


TEST(IS, epi16_comparisons)
{
    const __m128i a = _mm_set_epi16(-32768,-32768,-32767,-1,0, 1,32767,32767);
    const __m128i b = _mm_set_epi16(-32768,-32767,-1,     1,0,-1,    0,32767);
    assert_comparison<int16_t>(a, b, _mm_cmpeq_epi16(a,b),  COMP_EQ);
    assert_comparison<int16_t>(b, a, _mm_cmpeq_epi16(b,a),  COMP_EQ);
    assert_comparison<int16_t>(a, b, _mm_cmpneq_epi16(a,b), COMP_NE);
    assert_comparison<int16_t>(b, a, _mm_cmpneq_epi16(b,a), COMP_NE);
    assert_comparison<int16_t>(a, b, _mm_cmplt_epi16(a,b),  COMP_LT);
    assert_comparison<int16_t>(b, a, _mm_cmplt_epi16(b,a),  COMP_LT);
    assert_comparison<int16_t>(a, b, _mm_cmple_epi16(a,b),  COMP_LE);
    assert_comparison<int16_t>(b, a, _mm_cmple_epi16(b,a),  COMP_LE);
    assert_comparison<int16_t>(a, b, _mm_cmpgt_epi16(a,b),  COMP_GT);
    assert_comparison<int16_t>(b, a, _mm_cmpgt_epi16(b,a),  COMP_GT);
    assert_comparison<int16_t>(a, b, _mm_cmpge_epi16(a,b),  COMP_GE);
    assert_comparison<int16_t>(b, a, _mm_cmpge_epi16(b,a),  COMP_GE);
}


TEST(IS, epu16_comparisons)
{
    const __m128i a = _mm_set_epi16(-32768,-32768,-32767,-1,0, 1,32767,32767);
    const __m128i b = _mm_set_epi16(-32768,-32767,-1,     1,0,-1,    0,32767);
    assert_comparison<uint16_t>(a, b, _mm_cmpeq_epu16(a,b),  COMP_EQ);
    assert_comparison<uint16_t>(b, a, _mm_cmpeq_epu16(b,a),  COMP_EQ);
    assert_comparison<uint16_t>(a, b, _mm_cmpneq_epu16(a,b), COMP_NE);
    assert_comparison<uint16_t>(b, a, _mm_cmpneq_epu16(b,a), COMP_NE);
    assert_comparison<uint16_t>(a, b, _mm_cmplt_epu16(a,b),  COMP_LT);
    assert_comparison<uint16_t>(b, a, _mm_cmplt_epu16(b,a),  COMP_LT);
    assert_comparison<uint16_t>(a, b, _mm_cmple_epu16(a,b),  COMP_LE);
    assert_comparison<uint16_t>(b, a, _mm_cmple_epu16(b,a),  COMP_LE);
    assert_comparison<uint16_t>(a, b, _mm_cmpgt_epu16(a,b),  COMP_GT);
    assert_comparison<uint16_t>(b, a, _mm_cmpgt_epu16(b,a),  COMP_GT);
    assert_comparison<uint16_t>(a, b, _mm_cmpge_epu16(a,b),  COMP_GE);
    assert_comparison<uint16_t>(b, a, _mm_cmpge_epu16(b,a),  COMP_GE);
}


TEST(IS, epi32_comparisons)
{
    const __m128i a = _mm_set_epi32(INT32_MIN,INT32_MIN,0,INT32_MAX);
    const __m128i b = _mm_set_epi32(INT32_MIN,-1,      -1,INT32_MIN);
    assert_comparison<int32_t>(a, b, _mm_cmpeq_epi32(a,b),  COMP_EQ);
    assert_comparison<int32_t>(b, a, _mm_cmpeq_epi32(b,a),  COMP_EQ);
    assert_comparison<int32_t>(a, b, _mm_cmpneq_epi32(a,b), COMP_NE);
    assert_comparison<int32_t>(b, a, _mm_cmpneq_epi32(b,a), COMP_NE);
    assert_comparison<int32_t>(a, b, _mm_cmplt_epi32(a,b),  COMP_LT);
    assert_comparison<int32_t>(b, a, _mm_cmplt_epi32(b,a),  COMP_LT);
    assert_comparison<int32_t>(a, b, _mm_cmple_epi32(a,b),  COMP_LE);
    assert_comparison<int32_t>(b, a, _mm_cmple_epi32(b,a),  COMP_LE);
    assert_comparison<int32_t>(a, b, _mm_cmpgt_epi32(a,b),  COMP_GT);
    assert_comparison<int32_t>(b, a, _mm_cmpgt_epi32(b,a),  COMP_GT);
    assert_comparison<int32_t>(a, b, _mm_cmpge_epi32(a,b),  COMP_GE);
    assert_comparison<int32_t>(b, a, _mm_cmpge_epi32(b,a),  COMP_GE);
}


TEST(IS, epu32_comparisons)
{
    const __m128i a = _mm_set_epi32(INT32_MIN,INT32_MIN,0,INT32_MAX);
    const __m128i b = _mm_set_epi32(INT32_MIN,-1,      -1,INT32_MIN);
    assert_comparison<uint32_t>(a, b, _mm_cmpeq_epu32(a,b),  COMP_EQ);
    assert_comparison<uint32_t>(b, a, _mm_cmpeq_epu32(b,a),  COMP_EQ);
    assert_comparison<uint32_t>(a, b, _mm_cmpneq_epu32(a,b), COMP_NE);
    assert_comparison<uint32_t>(b, a, _mm_cmpneq_epu32(b,a), COMP_NE);
    assert_comparison<uint32_t>(a, b, _mm_cmplt_epu32(a,b),  COMP_LT);
    assert_comparison<uint32_t>(b, a, _mm_cmplt_epu32(b,a),  COMP_LT);
    assert_comparison<uint32_t>(a, b, _mm_cmple_epu32(a,b),  COMP_LE);
    assert_comparison<uint32_t>(b, a, _mm_cmple_epu32(b,a),  COMP_LE);
    assert_comparison<uint32_t>(a, b, _mm_cmpgt_epu32(a,b),  COMP_GT);
    assert_comparison<uint32_t>(b, a, _mm_cmpgt_epu32(b,a),  COMP_GT);
    assert_comparison<uint32_t>(a, b, _mm_cmpge_epu32(a,b),  COMP_GE);
    assert_comparison<uint32_t>(b, a, _mm_cmpge_epu32(b,a),  COMP_GE);
}


TEST(IS, epi64_comparisons)
{
    const __m128i a = _mm_set_epi64x(INT64_MIN,INT64_MIN);
    const __m128i b = _mm_set_epi64x(INT64_MIN,-1);
    const __m128i c = _mm_set_epi64x(0, INT64_MAX);
    assert_comparison<int64_t>(a, b, _mm_cmpeq_epi64(a, b),  COMP_EQ);
    assert_comparison<int64_t>(b, a, _mm_cmpeq_epi64(b, a),  COMP_EQ);
    assert_comparison<int64_t>(a, c, _mm_cmpeq_epi64(a, c),  COMP_EQ);
    assert_comparison<int64_t>(c, a, _mm_cmpeq_epi64(c, a),  COMP_EQ);
    assert_comparison<int64_t>(b, c, _mm_cmpeq_epi64(b, c),  COMP_EQ);
    assert_comparison<int64_t>(c, b, _mm_cmpeq_epi64(c, b),  COMP_EQ);
    assert_comparison<int64_t>(a, b, _mm_cmpneq_epi64(a, b), COMP_NE);
    assert_comparison<int64_t>(b, a, _mm_cmpneq_epi64(b, a), COMP_NE);
    assert_comparison<int64_t>(a, c, _mm_cmpneq_epi64(a, c), COMP_NE);
    assert_comparison<int64_t>(c, a, _mm_cmpneq_epi64(c, a), COMP_NE);
    assert_comparison<int64_t>(b, c, _mm_cmpneq_epi64(b, c), COMP_NE);
    assert_comparison<int64_t>(c, b, _mm_cmpneq_epi64(c, b), COMP_NE);
    assert_comparison<int64_t>(a, b, _mm_cmplt_epi64(a, b),  COMP_LT);
    assert_comparison<int64_t>(b, a, _mm_cmplt_epi64(b, a),  COMP_LT);
    assert_comparison<int64_t>(a, c, _mm_cmplt_epi64(a, c),  COMP_LT);
    assert_comparison<int64_t>(c, a, _mm_cmplt_epi64(c, a),  COMP_LT);
    assert_comparison<int64_t>(b, c, _mm_cmplt_epi64(b, c),  COMP_LT);
    assert_comparison<int64_t>(c, b, _mm_cmplt_epi64(c, b),  COMP_LT);
    assert_comparison<int64_t>(a, b, _mm_cmple_epi64(a, b),  COMP_LE);
    assert_comparison<int64_t>(b, a, _mm_cmple_epi64(b, a),  COMP_LE);
    assert_comparison<int64_t>(a, c, _mm_cmple_epi64(a, c),  COMP_LE);
    assert_comparison<int64_t>(c, a, _mm_cmple_epi64(c, a),  COMP_LE);
    assert_comparison<int64_t>(b, c, _mm_cmple_epi64(b, c),  COMP_LE);
    assert_comparison<int64_t>(c, b, _mm_cmple_epi64(c, b),  COMP_LE);
    assert_comparison<int64_t>(a, b, _mm_cmpgt_epi64(a, b),  COMP_GT);
    assert_comparison<int64_t>(b, a, _mm_cmpgt_epi64(b, a),  COMP_GT);
    assert_comparison<int64_t>(a, c, _mm_cmpgt_epi64(a, c),  COMP_GT);
    assert_comparison<int64_t>(c, a, _mm_cmpgt_epi64(c, a),  COMP_GT);
    assert_comparison<int64_t>(b, c, _mm_cmpgt_epi64(b, c),  COMP_GT);
    assert_comparison<int64_t>(c, b, _mm_cmpgt_epi64(c, b),  COMP_GT);
    assert_comparison<int64_t>(a, b, _mm_cmpge_epi64(a, b),  COMP_GE);
    assert_comparison<int64_t>(b, a, _mm_cmpge_epi64(b, a),  COMP_GE);
    assert_comparison<int64_t>(a, c, _mm_cmpge_epi64(a, c),  COMP_GE);
    assert_comparison<int64_t>(c, a, _mm_cmpge_epi64(c, a),  COMP_GE);
    assert_comparison<int64_t>(b, c, _mm_cmpge_epi64(b, c),  COMP_GE);
    assert_comparison<int64_t>(c, b, _mm_cmpge_epi64(c, b),  COMP_GE);
}


TEST(IS, epu64_comparisons)
{
    const __m128i a = _mm_set_epi64x(INT64_MIN,INT64_MIN);
    const __m128i b = _mm_set_epi64x(INT64_MIN,-1);
    const __m128i c = _mm_set_epi64x(0, INT64_MAX);
    assert_comparison<uint64_t>(a, b, _mm_cmpeq_epu64(a, b),  COMP_EQ);
    assert_comparison<uint64_t>(b, a, _mm_cmpeq_epu64(b, a),  COMP_EQ);
    assert_comparison<uint64_t>(a, c, _mm_cmpeq_epu64(a, c),  COMP_EQ);
    assert_comparison<uint64_t>(c, a, _mm_cmpeq_epu64(c, a),  COMP_EQ);
    assert_comparison<uint64_t>(b, c, _mm_cmpeq_epu64(b, c),  COMP_EQ);
    assert_comparison<uint64_t>(c, b, _mm_cmpeq_epu64(c, b),  COMP_EQ);
    assert_comparison<uint64_t>(a, b, _mm_cmpneq_epu64(a, b), COMP_NE);
    assert_comparison<uint64_t>(b, a, _mm_cmpneq_epu64(b, a), COMP_NE);
    assert_comparison<uint64_t>(a, c, _mm_cmpneq_epu64(a, c), COMP_NE);
    assert_comparison<uint64_t>(c, a, _mm_cmpneq_epu64(c, a), COMP_NE);
    assert_comparison<uint64_t>(b, c, _mm_cmpneq_epu64(b, c), COMP_NE);
    assert_comparison<uint64_t>(c, b, _mm_cmpneq_epu64(c, b), COMP_NE);
    assert_comparison<uint64_t>(a, b, _mm_cmplt_epu64(a, b),  COMP_LT);
    assert_comparison<uint64_t>(b, a, _mm_cmplt_epu64(b, a),  COMP_LT);
    assert_comparison<uint64_t>(a, c, _mm_cmplt_epu64(a, c),  COMP_LT);
    assert_comparison<uint64_t>(c, a, _mm_cmplt_epu64(c, a),  COMP_LT);
    assert_comparison<uint64_t>(b, c, _mm_cmplt_epu64(b, c),  COMP_LT);
    assert_comparison<uint64_t>(c, b, _mm_cmplt_epu64(c, b),  COMP_LT);
    assert_comparison<uint64_t>(a, b, _mm_cmple_epu64(a, b),  COMP_LE);
    assert_comparison<uint64_t>(b, a, _mm_cmple_epu64(b, a),  COMP_LE);
    assert_comparison<uint64_t>(a, c, _mm_cmple_epu64(a, c),  COMP_LE);
    assert_comparison<uint64_t>(c, a, _mm_cmple_epu64(c, a),  COMP_LE);
    assert_comparison<uint64_t>(b, c, _mm_cmple_epu64(b, c),  COMP_LE);
    assert_comparison<uint64_t>(c, b, _mm_cmple_epu64(c, b),  COMP_LE);
    assert_comparison<uint64_t>(a, b, _mm_cmpgt_epu64(a, b),  COMP_GT);
    assert_comparison<uint64_t>(b, a, _mm_cmpgt_epu64(b, a),  COMP_GT);
    assert_comparison<uint64_t>(a, c, _mm_cmpgt_epu64(a, c),  COMP_GT);
    assert_comparison<uint64_t>(c, a, _mm_cmpgt_epu64(c, a),  COMP_GT);
    assert_comparison<uint64_t>(b, c, _mm_cmpgt_epu64(b, c),  COMP_GT);
    assert_comparison<uint64_t>(c, b, _mm_cmpgt_epu64(c, b),  COMP_GT);
    assert_comparison<uint64_t>(a, b, _mm_cmpge_epu64(a, b),  COMP_GE);
    assert_comparison<uint64_t>(b, a, _mm_cmpge_epu64(b, a),  COMP_GE);
    assert_comparison<uint64_t>(a, c, _mm_cmpge_epu64(a, c),  COMP_GE);
    assert_comparison<uint64_t>(c, a, _mm_cmpge_epu64(c, a),  COMP_GE);
    assert_comparison<uint64_t>(b, c, _mm_cmpge_epu64(b, c),  COMP_GE);
    assert_comparison<uint64_t>(c, b, _mm_cmpge_epu64(c, b),  COMP_GE);
}


template<typename Scalar, typename Vector>
static RMGR_NOINLINE void assert_left_shift(const Vector& a, const Vector res, unsigned count)
{
    const size_t length = sizeof(Vector) / sizeof(Scalar);
    Scalar bufA[length];
    Scalar bufR[length];
    store(bufA, a);
    store(bufR, res);
    for (size_t i=0; i<length; ++i)
    {
        ASSERT_EQ(Scalar(bufA[i] << count), bufR[i]);
    }
}

template<typename Scalar, typename Vector>
static RMGR_NOINLINE void assert_right_shift(const Vector& a, const Vector res, unsigned count)
{
    const size_t length = sizeof(Vector) / sizeof(Scalar);
    Scalar bufA[length];
    Scalar bufR[length];
    store(bufA, a);
    store(bufR, res);
    for (size_t i=0; i<length; ++i)
    {
        ASSERT_EQ(Scalar(bufA[i] >> count), bufR[i]);
    }
}

TEST(IS, epi8_shifts)
{
    const __m128i v = _mm_set_epi8(-128,-128,-127,-127,-1,-1,0,0,1,1,2,2,126,126,127,127);
    for (unsigned i=0; i<8u; ++i)
    {
        assert_left_shift< uint8_t>(v, _mm_sll_epi8(v, _mm_set1_epi64x(i)), i);
        assert_right_shift<uint8_t>(v, _mm_srl_epi8(v, _mm_set1_epi64x(i)), i);
        assert_right_shift<int8_t >(v, _mm_sra_epi8(v, _mm_set1_epi64x(i)), i);
    }
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 0),  0);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 1),  1);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 2),  2);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 3),  3);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 4),  4);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 5),  5);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 6),  6);
    assert_left_shift< uint8_t>(v, _mm_slli_epi8(v, 7),  7);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 0),  0);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 1),  1);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 2),  2);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 3),  3);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 4),  4);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 5),  5);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 6),  6);
    assert_right_shift<uint8_t>(v, _mm_srli_epi8(v, 7),  7);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 0),  0);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 1),  1);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 2),  2);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 3),  3);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 4),  4);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 5),  5);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 6),  6);
    assert_right_shift<int8_t >(v, _mm_srai_epi8(v, 7),  7);
}

TEST(IS, epi64_right_shift)
{
    const __m128i v = _mm_set_epi64x(0xFEDCBA9876543210ll, 0x0123456789ABCDEFll);
    for (unsigned i=0; i<64u; ++i)
    {
        assert_right_shift<int64_t>(v, _mm_sra_epi64(v, _mm_set1_epi64x(i)), i);
    }

    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 0),  0);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 1),  1);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 2),  2);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 3),  3);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 4),  4);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 5),  5);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 6),  6);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 7),  7);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 8),  8);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v, 9),  9);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,10), 10);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,11), 11);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,12), 12);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,13), 13);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,14), 14);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,15), 15);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,16), 16);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,17), 17);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,18), 18);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,19), 19);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,20), 20);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,21), 21);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,22), 22);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,23), 23);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,24), 24);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,25), 25);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,26), 26);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,27), 27);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,28), 28);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,29), 29);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,30), 30);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,31), 31);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,32), 32);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,33), 33);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,34), 34);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,35), 35);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,36), 36);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,37), 37);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,38), 38);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,39), 39);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,40), 40);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,41), 41);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,42), 42);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,43), 43);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,44), 44);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,45), 45);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,46), 46);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,47), 47);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,48), 48);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,49), 49);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,50), 50);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,51), 51);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,52), 52);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,53), 53);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,54), 54);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,55), 55);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,56), 56);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,57), 57);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,58), 58);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,59), 59);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,60), 60);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,61), 61);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,62), 62);
    assert_right_shift<int64_t>(v, _mm_srai_epi64(v,63), 63);
}


template<typename Scalar, typename Vector>
static void assert_min_max(const Vector& a, const Vector& b, const Vector& res, const Scalar& (*fct)(const Scalar&, const Scalar&))
{
    const size_t length = sizeof(Vector) / sizeof(Scalar);
    Scalar bufA[length];
    Scalar bufB[length];
    Scalar bufR[length];
    store(bufA, a);
    store(bufB, b);
    store(bufR, res);
    for (size_t i=0; i<length; ++i)
    {
        ASSERT_EQ(fct(bufA[i],bufB[i]), bufR[i]);
    }
}


TEST(IS, epi8_min_max)
{
    const __m128i a = _mm_set_epi8(-128,-128,-127,-127,-1,-1,0,0,1,1,2,2,126,126,127,127);
    const __m128i b = _mm_set_epi8(-128,-127,-128,-127,-2,-1,1,0,2,1,1,2,127,126,126,127);
    assert_min_max<int8_t>(a, b, _mm_min_epi8(a,b), std::min);
    assert_min_max<int8_t>(b, a, _mm_min_epi8(b,a), std::min);
    assert_min_max<int8_t>(a, b, _mm_max_epi8(a,b), std::max);
    assert_min_max<int8_t>(b, a, _mm_max_epi8(b,a), std::max);
}


TEST(IS, epu16_min_max)
{
    const __m128i a = _mm_set_epi16(-32768,-32768,-32767,-1,0, 1,32767,32767);
    const __m128i b = _mm_set_epi16(-32768,-32767,-1,     1,0,-1,    0,32767);
    assert_min_max<uint16_t>(a, b, _mm_min_epu16(a,b), std::min);
    assert_min_max<uint16_t>(b, a, _mm_min_epu16(b,a), std::min);
    assert_min_max<uint16_t>(a, b, _mm_max_epu16(a,b), std::max);
    assert_min_max<uint16_t>(b, a, _mm_max_epu16(b,a), std::max);
}


TEST(IS, epu32_min_max)
{
    const __m128i a = _mm_set_epi32(INT32_MIN,INT32_MIN,0,INT32_MAX);
    const __m128i b = _mm_set_epi32(INT32_MIN,-1,      -1,INT32_MIN);
    assert_min_max<uint32_t>(a, b, _mm_min_epu32(a,b), std::min);
    assert_min_max<uint32_t>(b, a, _mm_min_epu32(b,a), std::min);
    assert_min_max<uint32_t>(a, b, _mm_max_epu32(a,b), std::max);
    assert_min_max<uint32_t>(b, a, _mm_max_epu32(b,a), std::max);
}


TEST(IS, epi64_min_max)
{
    const __m128i a = _mm_set_epi64x(INT64_MIN,INT64_MIN);
    const __m128i b = _mm_set_epi64x(INT64_MIN,-1);
    const __m128i c = _mm_set_epi64x(0, INT64_MAX);
    assert_min_max<int64_t>(a, b, _mm_min_epi64(a,b), std::min);
    assert_min_max<int64_t>(b, a, _mm_min_epi64(b,a), std::min);
    assert_min_max<int64_t>(a, c, _mm_min_epi64(a,c), std::min);
    assert_min_max<int64_t>(c, a, _mm_min_epi64(c,a), std::min);
    assert_min_max<int64_t>(b, c, _mm_min_epi64(b,c), std::min);
    assert_min_max<int64_t>(c, b, _mm_min_epi64(c,b), std::min);
    assert_min_max<int64_t>(a, b, _mm_max_epi64(a,b), std::max);
    assert_min_max<int64_t>(b, a, _mm_max_epi64(b,a), std::max);
    assert_min_max<int64_t>(a, c, _mm_max_epi64(a,c), std::max);
    assert_min_max<int64_t>(c, a, _mm_max_epi64(c,a), std::max);
    assert_min_max<int64_t>(b, c, _mm_max_epi64(b,c), std::max);
    assert_min_max<int64_t>(c, b, _mm_max_epi64(c,b), std::max);
}


TEST(IS, epu64_min_max)
{
    const __m128i a = _mm_set_epi64x(INT64_MIN,INT64_MIN);
    const __m128i b = _mm_set_epi64x(INT64_MIN,-1);
    const __m128i c = _mm_set_epi64x(0, INT64_MAX);
    assert_min_max<uint64_t>(a, b, _mm_min_epu64(a,b), std::min);
    assert_min_max<uint64_t>(b, a, _mm_min_epu64(b,a), std::min);
    assert_min_max<uint64_t>(a, c, _mm_min_epu64(a,c), std::min);
    assert_min_max<uint64_t>(c, a, _mm_min_epu64(c,a), std::min);
    assert_min_max<uint64_t>(b, c, _mm_min_epu64(b,c), std::min);
    assert_min_max<uint64_t>(c, b, _mm_min_epu64(c,b), std::min);
    assert_min_max<uint64_t>(a, b, _mm_max_epu64(a,b), std::max);
    assert_min_max<uint64_t>(b, a, _mm_max_epu64(b,a), std::max);
    assert_min_max<uint64_t>(a, c, _mm_max_epu64(a,c), std::max);
    assert_min_max<uint64_t>(c, a, _mm_max_epu64(c,a), std::max);
    assert_min_max<uint64_t>(b, c, _mm_max_epu64(b,c), std::max);
    assert_min_max<uint64_t>(c, b, _mm_max_epu64(c,b), std::max);
}


template<typename Scalar, typename UScalar, typename Vector>
static void assert_abs(const Vector& a, const Vector& res)
{
    const size_t length = sizeof(Vector) / sizeof(Scalar);
    Scalar  bufA[length];
    UScalar bufR[length];
    store(bufA, a);
    store(bufR, res);
    for (size_t i=0; i<length; ++i)
    {
        ASSERT_EQ(UScalar(std::abs(bufA[i])), bufR[i]);
    }
}


TEST(IS, epi8_abs)
{
    const __m128i a = _mm_set_epi8(-128,-128,-127,-127,-1,-1,0,0,1,1,2,2,126,126,127,127);
    assert_abs<int8_t, uint8_t>(a, _mm_abs_epi8(a));
}


TEST(IS, epi16_abs)
{
    const __m128i a = _mm_set_epi16(-32768,-32767,-1,1,0,-1,0,32767);
    assert_abs<int16_t, uint16_t>(a, _mm_abs_epi16(a));
}


TEST(IS, epi32_abs)
{
    const __m128i a = _mm_set_epi32(INT32_MIN,-1,0,INT32_MAX);
    assert_abs<int32_t, uint32_t>(a, _mm_abs_epi32(a));
}


TEST(IS, epi64_abs)
{
    const __m128i a = _mm_set_epi64x(INT64_MIN,0);
    const __m128i b = _mm_set_epi64x(-1,INT64_MAX);
    assert_abs<int64_t, uint64_t>(a, _mm_abs_epi64(a));
    assert_abs<int64_t, uint64_t>(b, _mm_abs_epi64(b));
}


TEST(IS, ps_abs)
{
    const __m128 a = _mm_set_ps(-FLT_MAX,-1,0,FLT_MAX);
    assert_abs<float, float>(a, _mm_abs_ps(a));
}


TEST(IS, pd_abs)
{
    const __m128d a = _mm_set_pd(-FLT_MAX,0);
    const __m128d b = _mm_set_pd(-1,FLT_MAX);
    assert_abs<double, double>(a, _mm_abs_pd(a));
    assert_abs<double, double>(b, _mm_abs_pd(b));
}
