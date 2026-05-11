#include "bvh4.hpp"
#include "math/vec3.hpp"
#include <cmath>
#include <cstdint>
#include <smmintrin.h>
#include <utility>
#include <xmmintrin.h>

#ifdef SSE
#include <immintrin.h>
#endif // SSE

void BVHNode::intersect(Ray ray, Vec3 parent_lo, Vec3 parent_hi) {
  float scale_x = (parent_hi.x - parent_lo.x) / 255.f;
  float scale_y = (parent_hi.y - parent_lo.y) / 255.f;
  float scale_z = (parent_hi.z - parent_lo.z) / 255.f;

#ifdef SSE
  // with a lot of help from
  // https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf or random
  // searches

  // dequantize bytes to floats:
  // - load copies of the 4 bytes into the 4 slots (loading as i32)
  // - _mm_cvtepu8_epi32 unpacks bytes to i32s with leading 0s
  // - i32 -> float
  __m128 xlo = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.xlo)));
  __m128 xhi = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.xhi)));

  __m128 ylo = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.ylo)));
  __m128 yhi = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.yhi)));

  __m128 zlo = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.zlo)));
  __m128 zhi = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.zhi)));

  // transform to world space: world = parent_lo + local*parent_scale
  __m128 plo_x = _mm_set1_ps(parent_lo.x);
  __m128 plo_y = _mm_set1_ps(parent_lo.y);
  __m128 plo_z = _mm_set1_ps(parent_lo.z);
  __m128 sx = _mm_set1_ps(scale_x);
  __m128 sy = _mm_set1_ps(scale_y);
  __m128 sz = _mm_set1_ps(scale_z);

  xlo = _mm_fmadd_ps(xlo, sx, plo_x);
  xhi = _mm_fmadd_ps(xhi, sx, plo_x);

  ylo = _mm_fmadd_ps(ylo, sy, plo_y);
  yhi = _mm_fmadd_ps(yhi, sy, plo_y);

  zlo = _mm_fmadd_ps(zlo, sz, plo_z);
  zhi = _mm_fmadd_ps(zhi, sz, plo_z);

  // slab method intersection test (https://en.wikipedia.org/wiki/Slab_method)

#endif // SSE
}

#ifdef SSE

// with a lot of help from
// https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf or random
// searches
void BVHNode::intersect_sse(__m128 rix, __m128 riy, __m128 riz, __m128 roix,
                            __m128 roiy, __m128 roiz, float sxf, float syf,
                            float szf, float ploxf, float ployf, float plozf,
                            float tminf, float tmaxf) {

  // dequantize bytes to floats:
  // - load copies of the 4 bytes into the 4 slots (loading as i32)
  // - _mm_cvtepu8_epi32 unpacks bytes to i32s with leading 0s
  // - i32 -> float
  __m128 xlo = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.xlo)));
  __m128 xhi = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.xhi)));

  __m128 ylo = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.ylo)));
  __m128 yhi = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.yhi)));

  __m128 zlo = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.zlo)));
  __m128 zhi = _mm_cvtepi32_ps(
      _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int *)internal.zhi)));

  // transform to world space: world = parent_lo + local*parent_scale
  __m128 plox = _mm_set1_ps(ploxf);
  __m128 ploy = _mm_set1_ps(ployf);
  __m128 ploz = _mm_set1_ps(plozf);
  __m128 sx = _mm_set1_ps(sxf);
  __m128 sy = _mm_set1_ps(syf);
  __m128 sz = _mm_set1_ps(szf);

  xlo = _mm_fmadd_ps(xlo, sx, plox);
  xhi = _mm_fmadd_ps(xhi, sx, plox);

  ylo = _mm_fmadd_ps(ylo, sy, ploy);
  yhi = _mm_fmadd_ps(yhi, sy, ploy);

  zlo = _mm_fmadd_ps(zlo, sz, ploz);
  zhi = _mm_fmadd_ps(zhi, sz, ploz);

  // https://en.wikipedia.org/wiki/Slab_method
  __m128 tlox = _mm_fmsub_ps(xlo, rix, roix);
  __m128 thix = _mm_fmsub_ps(xhi, rix, roix);

  __m128 tc = _mm_min_ps(tlox, thix);
  __m128 tf = _mm_max_ps(tlox, thix);

  __m128 tloy = _mm_fmsub_ps(ylo, riy, roiy);
  __m128 thiy = _mm_fmsub_ps(yhi, riy, roiy);

  __m128 tcy = _mm_min_ps(tloy, thiy);
  __m128 tfy = _mm_max_ps(tloy, thiy);

  tc = _mm_max_ps(tc, tcy);
  tf = _mm_min_ps(tf, tfy);

  __m128 tloz = _mm_fmsub_ps(zlo, riz, roiz);
  __m128 thiz = _mm_fmsub_ps(zhi, riz, roiz);

  __m128 tcz = _mm_min_ps(tloz, thiz);
  __m128 tfz = _mm_max_ps(tloz, thiz);

  tc = _mm_max_ps(tc, tcz);
  tf = _mm_min_ps(tf, tfz);

  __m128 tmin = _mm_set1_ps(tminf);
  __m128 tmax = _mm_set1_ps(tmaxf);

  tc = _mm_max_ps(tc, tmin);
  tf = _mm_min_ps(tf, tmax);

  __m128 hit_mask = _mm_cmple_ps(tc, tf);
  tc = _mm_and_ps(tc, hit_mask);

  int m = _mm_movemask_ps(hit_mask);

  if (m == 0)
    return; // no collision
  float ts[4];
  _mm_store_ps(ts, tc);

  // merge sort lol
  bool s01 = ts[0] > ts[1] && (m & 0b10);
  bool s23 = ts[2] > ts[3] && (m & 0b1000);

  if (s01) {
    std::swap(ts[0], ts[1]);
    int s = 0b0101;
    m ^= 0b11;
  }
  if (s23) {
    std::swap(ts[2], ts[3]);
    m ^= 0b1100;
  }

  bool s02 = ts[0] > ts[2] && (m & 0b1000);

  int i0 = s02 ? s23+2 : s01;

  // merge
  if () {
    i0 = 
    std::swap(ts[0], ts[2]);
    m ^= 0b1100;
  }

}
#endif // SSE
