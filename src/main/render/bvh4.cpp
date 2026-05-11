#include "bvh4.hpp"
#include <cstdint>
#include <immintrin.h>
#include <optional>
#include <xmmintrin.h>

#ifdef AVX

// with a lot of help from
// https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf or random
// searches
std::optional<uint32_t>
BVHNode::intersect_avx(__m256 rix, __m256 riy, __m256 riz, __m256 roix,
                       __m256 roiy, __m256 roiz, float sxf, float syf,
                       float szf, float ploxf, float ployf, float plozf,
                       float tminf, float tmaxf) {

  if (internal.children[0] == 0) {
    return std::optional(leaf.ptr);
  }

  // dequantize bytes to floats:
  // - load copies of the 8 bytes into the 8 slots (loading as i64)
  // - _mm256_cvtepu8_epi32 unpacks bytes to i32s with leading 0s
  // - i32 -> float
  __m256 x = _mm256_cvtepi32_ps(
      _mm256_cvtepu8_epi32(_mm_cvtsi64_si128(internal.xlohi)));

  __m256 y = _mm256_cvtepi32_ps(
      _mm256_cvtepu8_epi32(_mm_cvtsi64_si128(internal.ylohi)));

  __m256 z = _mm256_cvtepi32_ps(
      _mm256_cvtepu8_epi32(_mm_cvtsi64_si128(internal.zlohi)));

  // transform to world space: world = parent_lo + local*parent_scale
  {

    __m256 plox = _mm256_set1_ps(ploxf);
    __m256 sx = _mm256_set1_ps(sxf);
    x = _mm256_fmadd_ps(x, sx, plox);
  }
  {

    __m256 ploy = _mm256_set1_ps(ployf);
    __m256 sy = _mm256_set1_ps(syf);
    y = _mm256_fmadd_ps(y, sy, ploy);
  }
  {
    __m256 ploz = _mm256_set1_ps(plozf);
    __m256 sz = _mm256_set1_ps(szf);
    z = _mm256_fmadd_ps(z, sz, ploz);
  }

  // https://en.wikipedia.org/wiki/Slab_method
  __m256 tx = _mm256_fmsub_ps(x, rix, roix);
  __m256i txi = _mm256_castps_si256(tx);
  // swap lower/upper 128-bit halves (swap lo hi)
  __m256 txs = _mm256_castsi256_ps(_mm256_permute2x128_si256(txi, txi, 0x01));

  __m128 tc = _mm256_castps256_ps128(_mm256_min_ps(tx, txs));
  __m128 tf = _mm256_castps256_ps128(_mm256_max_ps(tx, txs));

  {
    __m256 ty = _mm256_fmsub_ps(y, riy, roiy);
    __m256i tyi = _mm256_castps_si256(ty);
    __m256 tys = _mm256_castsi256_ps(_mm256_permute2x128_si256(tyi, tyi, 0x01));

    __m128 tcy = _mm256_castps256_ps128(_mm256_min_ps(ty, tys));
    __m128 tfy = _mm256_castps256_ps128(_mm256_max_ps(ty, tys));
    tc = _mm_max_ps(tc, tcy);
    tf = _mm_min_ps(tf, tfy);
  }
  {

    __m256 tz = _mm256_fmsub_ps(z, riz, roiz);
    __m256i tzi = _mm256_castps_si256(tz);
    __m256 tzs = _mm256_castsi256_ps(_mm256_permute2x128_si256(tzi, tzi, 0x01));

    __m128 tcz = _mm256_castps256_ps128(_mm256_min_ps(tz, tzs));
    __m128 tfz = _mm256_castps256_ps128(_mm256_max_ps(tz, tzs));
    tc = _mm_max_ps(tc, tcz);
    tf = _mm_min_ps(tf, tfz);
  }

  __m128 tmin = _mm_set1_ps(tminf);
  __m128 tmax = _mm_set1_ps(tmaxf);

  // clamping t
  tc = _mm_max_ps(tc, tmin);
  tf = _mm_min_ps(tf, tmax);

  __m128 hit_mask = _mm_cmple_ps(tc, tf);

  __m128 inf = _mm_set1_ps(std::numeric_limits<float>::infinity());
  __m128 ts = _mm_blendv_ps(inf, tc, hit_mask); // ray hit distances

  float tsf[4];

  _mm_store_ps(tsf, ts);

  while (true) {
    // find child with closest bounding box to test first
    __m128 shuf = _mm_shuffle_ps(ts, ts, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 min1 = _mm_min_ps(ts, shuf);
    shuf = _mm_shuffle_ps(min1, min1, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 min = _mm_min_ps(min1, shuf); // [min, min, min, min]
    __m128 min_eq = _mm_cmpeq_ps(ts, min);

    __m128 valid_min = _mm_and_ps(hit_mask, min_eq);
    int min_bit = _mm_movemask_ps(valid_min);
    if (min_bit == 0)
      return {}; // no collision
    int min_i = _tzcnt_u32(min_bit);

    // test ray
    float xlo =
        static_cast<float>((internal.xlohi >> (min_i * 4)) & 255) * sxf + ploxf;
    float xhi =
        static_cast<float>((internal.xlohi >> (min_i * 4 + 32)) & 255) * sxf +
        ploxf;
    float sx = xhi - xlo;

    float ylo =
        static_cast<float>((internal.ylohi >> (min_i * 4)) & 255) * syf + ployf;
    float yhi =
        static_cast<float>((internal.ylohi >> (min_i * 4 + 32)) & 255) * syf +
        ployf;
    float sy = yhi - ylo;

    float zlo =
        static_cast<float>((internal.zlohi >> (min_i * 4)) & 255) * szf + plozf;
    float zhi =
        static_cast<float>((internal.zlohi >> (min_i * 4 + 32)) & 255) * szf +
        plozf;
    float sz = zhi - zlo;

    auto res = this[internal.children[min_i]].intersect_avx(
        rix, riy, riz, roix, roiy, roiz, sx, sy, sz, xlo, ylo, zlo, tminf,
        tmaxf - tsf[min_i]);

    if (res.has_value())
      return res;

    // try next by removing cur and re reducing min
    _mm_blendv_ps(tc, inf, min_eq);
  }
}
#endif // AVX
