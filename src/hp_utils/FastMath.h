#pragma once

#include <emmintrin.h>
#include <math.h>

// Some SSE code borrowed from Surge
namespace FastMath
{

inline float vSum(__m128 x)
{
    __m128 a = _mm_add_ps(x, _mm_movehl_ps(x, x));
    a = _mm_add_ss(a, _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 1)));
    float f;
    _mm_store_ss(&f, a);

    return f;
}

inline __m128 fastsinSSE(__m128 x) noexcept
{
#define M(a, b) _mm_mul_ps(a, b)
#define A(a, b) _mm_add_ps(a, b)
#define S(a, b) _mm_sub_ps(a, b)
#define F(a) _mm_set_ps1(a)
#define C(x) __m128 m##x = F((float)x)

    /*
    auto numerator = -x * (-(float)11511339840 +
                           x2 * ((float)1640635920 + x2 * (-(float)52785432 + x2 * (float)479249)));
    auto denominator =
        (float)11511339840 + x2 * ((float)277920720 + x2 * ((float)3177720 + x2 * (float)18361));
        */
    C(11511339840);
    C(1640635920);
    C(52785432);
    C(479249);
    C(277920720);
    C(3177720);
    C(18361);
    auto mnegone = F(-1);

    auto x2 = M(x, x);

    auto num = M(mnegone,
                 M(x, S(M(x2, A(m1640635920, M(x2, S(M(x2, m479249), m52785432)))), m11511339840)));
    auto den = A(m11511339840, M(x2, A(m277920720, M(x2, A(m3177720, M(x2, m18361))))));

#undef C
#undef M
#undef A
#undef S
#undef F
    return _mm_div_ps(num, den);
}

inline __m128 fastcosSSE(__m128 x) noexcept
{
#define M(a, b) _mm_mul_ps(a, b)
#define A(a, b) _mm_add_ps(a, b)
#define S(a, b) _mm_sub_ps(a, b)
#define F(a) _mm_set_ps1(a)
#define C(x) __m128 m##x = F((float)x)

    // auto x2 = x * x;
    auto x2 = M(x, x);

    C(39251520);
    C(18471600);
    C(1075032);
    C(14615);
    C(1154160);
    C(16632);
    C(127);

    // auto numerator = -(-(float)39251520 + x2 * ((float)18471600 + x2 * (-1075032 + 14615 * x2)));
    auto num = S(m39251520, M(x2, A(m18471600, M(x2, S(M(m14615, x2), m1075032)))));

    // auto denominator = (float)39251520 + x2 * (1154160 + x2 * (16632 + x2 * 127));
    auto den = A(m39251520, M(x2, A(m1154160, M(x2, A(m16632, M(x2, m127))))));
#undef C
#undef M
#undef A
#undef S
#undef F
    return _mm_div_ps(num, den);
}

inline __m128 clampToPiRangeSSE(__m128 x)
{
    const auto mpi = _mm_set1_ps(M_PI);
    const auto m2pi = _mm_set1_ps(2.0 * M_PI);
    const auto oo2p = _mm_set1_ps(1.0 / (2.0 * M_PI));
    const auto mz = _mm_setzero_ps();

    auto y = _mm_add_ps(x, mpi);
    auto yip = _mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(y, oo2p)));
    auto p = _mm_sub_ps(y, _mm_mul_ps(m2pi, yip));
    auto off = _mm_and_ps(_mm_cmplt_ps(p, mz), m2pi);
    p = _mm_add_ps(p, off);

    return _mm_sub_ps(p, mpi);
}

} // namespace FastMath
