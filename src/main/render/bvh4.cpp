#include "math/intersection.hpp"
#include "render/bvh.hpp"
#include <algorithm>
#include <limits>

Intersection BVH::BVH4Node::intersect_c(BVH *bvh, float rix, float riy,
                                        float riz, float roix, float roiy,
                                        float roiz, float sxf, float syf,
                                        float szf, float ploxf, float ployf,
                                        float plozf, float tminf, float tmaxf) {
  if (internal.children[0] == 0) {
    auto res = ::intersect(bvh->cur_ray, bvh->objs[leaf.ptr].origin.pos, bvh->objs[leaf.ptr].appearance.geometry);
    if (res.valid()) {
      res.obj = &bvh->objs[leaf.ptr];
      return res;
    } else
      return {};
  }

  if (tminf > tmaxf)
    return {};

  float ts[4];
  bool hit[4];
  float xlo_w[4], xhi_w[4], ylo_w[4], yhi_w[4], zlo_w[4], zhi_w[4];

  for (int i = 0; i < 4; i++) {
    xlo_w[i] =
        static_cast<float>((internal.xlohi >> (i * 8)) & 0xFF) * sxf + ploxf;
    xhi_w[i] =
        static_cast<float>((internal.xlohi >> (32 + i * 8)) & 0xFF) * sxf +
        ploxf;
    ylo_w[i] =
        static_cast<float>((internal.ylohi >> (i * 8)) & 0xFF) * syf + ployf;
    yhi_w[i] =
        static_cast<float>((internal.ylohi >> (32 + i * 8)) & 0xFF) * syf +
        ployf;
    zlo_w[i] =
        static_cast<float>((internal.zlohi >> (i * 8)) & 0xFF) * szf + plozf;
    zhi_w[i] =
        static_cast<float>((internal.zlohi >> (32 + i * 8)) & 0xFF) * szf +
        plozf;

    float tx0 = xlo_w[i] * rix - roix, tx1 = xhi_w[i] * rix - roix;
    float tc = std::min(tx0, tx1), tf = std::max(tx0, tx1);

    float ty0 = ylo_w[i] * riy - roiy, ty1 = yhi_w[i] * riy - roiy;
    tc = std::max(tc, std::min(ty0, ty1));
    tf = std::min(tf, std::max(ty0, ty1));

    float tz0 = zlo_w[i] * riz - roiz, tz1 = zhi_w[i] * riz - roiz;
    tc = std::max(tc, std::min(tz0, tz1));
    tf = std::min(tf, std::max(tz0, tz1));

    tc = std::max(tc, tminf);
    tf = std::min(tf, tmaxf);

    hit[i] = tc <= tf;
    ts[i] = hit[i] ? tc : std::numeric_limits<float>::infinity();
  }

  int order[4] = {0, 1, 2, 3};
  std::sort(order, order + 4, [&](int a, int b) { return ts[a] < ts[b]; });

  Intersection best = {};

  for (int oi = 0; oi < 4; oi++) {
    int i = order[oi];
    if (!hit[i])
      break;

    float child_sxf = (xhi_w[i] - xlo_w[i]) / 255.0f;
    float child_syf = (yhi_w[i] - ylo_w[i]) / 255.0f;
    float child_szf = (zhi_w[i] - zlo_w[i]) / 255.0f;

    int offset = internal.children[i];

    if (offset == 0 || ts[i] > tmaxf)
      return best;

    auto res = this[internal.children[i]].intersect_c(
        bvh, rix, riy, riz, roix, roiy, roiz, child_sxf, child_syf, child_szf,
        xlo_w[i], ylo_w[i], zlo_w[i], tminf, best.dist);

    if (res.valid() && res.dist < best.dist) best = res;
  }

  return best;
}

#ifdef ARM_NEON
#include <arm_neon.h>

std::optional<uint32_t>
BVH4Node::intersect_neon(float32x4_t rix, float32x4_t riy, float32x4_t riz,
                         float32x4_t roix, float32x4_t roiy, float32x4_t roiz,
                         float32x4_t sx, float32x4_t sy, float32x4_t sz,
                         float32x4_t plox, float32x4_t ploy, float32x4_t ploz,
                         float tminf, float tmaxf) {
  if (internal.children[0] == 0)
    return std::optional(leaf.ptr);

  // basically the same as avx.

  // dequantize
  uint16x8_t x = vmovl_u8(vcreate_u8(internal.xlohi));
  uint16x8_t y = vmovl_u8(vcreate_u8(internal.ylohi));
  uint16x8_t z = vmovl_u8(vcreate_u8(internal.zlohi));

  float32x4_t xlo =
      vmlaq_f32(plox, vcvtq_f32_u32(vmovl_u16(vget_low_u16(x))), sx);
  float32x4_t xhi =
      vmlaq_f32(plox, vcvtq_f32_u32(vmovl_u16(vget_high_u16(x))), sx);
  float32x4_t ylo =
      vmlaq_f32(ploy, vcvtq_f32_u32(vmovl_u16(vget_low_u16(y))), sy);
  float32x4_t yhi =
      vmlaq_f32(ploy, vcvtq_f32_u32(vmovl_u16(vget_high_u16(y))), sy);
  float32x4_t zlo =
      vmlaq_f32(ploz, vcvtq_f32_u32(vmovl_u16(vget_low_u16(z))), sz);
  float32x4_t zhi =
      vmlaq_f32(ploz, vcvtq_f32_u32(vmovl_u16(vget_high_u16(z))), sz);

  // slab test
  float32x4_t txlo = vfmaq_f32(roix, xlo, rix);
  float32x4_t txhi = vfmaq_f32(roix, xhi, rix);
  float32x4_t tc = vminq_f32(txlo, txhi);
  float32x4_t tf = vmaxq_f32(txlo, txhi);

  float32x4_t tylo = vfmaq_f32(roiy, ylo, riy);
  float32x4_t tyhi = vfmaq_f32(roiy, yhi, riy);
  tc = vmaxq_f32(tc, vminq_f32(tylo, tyhi));
  tf = vminq_f32(tf, vmaxq_f32(tylo, tyhi));

  float32x4_t tzlo = vfmaq_f32(roiz, zlo, riz);
  float32x4_t tzhi = vfmaq_f32(roiz, zhi, riz);
  tc = vmaxq_f32(tc, vminq_f32(tzlo, tzhi));
  tf = vminq_f32(tf, vmaxq_f32(tzlo, tzhi));

  tc = vmaxq_f32(tc, vdupq_n_f32(tminf));
  tf = vminq_f32(tf, vdupq_n_f32(tmaxf));

  uint32x4_t hit_mask = vcleq_f32(tc, tf);
  float32x4_t inf = vdupq_n_f32(std::numeric_limits<float>::infinity());
  float32x4_t ts = vbslq_f32(hit_mask, tc, inf);

  float tsf[4];
  float xlo_a[4], xhi_a[4], ylo_a[4], yhi_a[4], zlo_a[4], zhi_a[4];
  vst1q_f32(tsf, ts);
  vst1q_f32(xlo_a, xlo);
  vst1q_f32(xhi_a, xhi);
  vst1q_f32(ylo_a, ylo);
  vst1q_f32(yhi_a, yhi);
  vst1q_f32(zlo_a, zlo);
  vst1q_f32(zhi_a, zhi);

  // lane i contributes bit i of the mask. vaddvq is horizontal sum
  static const uint32_t lane_bits_arr[4] = {1u, 2u, 4u, 8u};
  const uint32x4_t lane_bits = vld1q_u32(lane_bits_arr);

  while (true) {
    // min reduction like avx shuffle
    float32x4_t min = vdupq_n_f32(vminvq_f32(ts));
    uint32x4_t is_min = vceqq_f32(ts, min);
    uint32_t min_bit =
        vaddvq_u32(vandq_u32(vandq_u32(hit_mask, is_min), lane_bits));
    if (min_bit == 0)
      return {};
    int min_i = __builtin_ctz(min_bit);

    float child_sx = (xhi_a[min_i] - xlo_a[min_i]) / 255.0f;
    float child_sy = (yhi_a[min_i] - ylo_a[min_i]) / 255.0f;
    float child_sz = (zhi_a[min_i] - zlo_a[min_i]) / 255.0f;

    auto res = this[internal.children[min_i]].intersect_neon(
        rix, riy, riz, roix, roiy, roiz, vdupq_n_f32(child_sx),
        vdupq_n_f32(child_sy), vdupq_n_f32(child_sz), vdupq_n_f32(xlo_a[min_i]),
        vdupq_n_f32(ylo_a[min_i]), vdupq_n_f32(zlo_a[min_i]), tminf,
        tmaxf - tsf[min_i]);

    if (res.has_value())
      return res;

    // remove closest and re sorts to find next closest
    hit_mask = vandq_u32(hit_mask, vmvnq_u32(is_min));
    ts = vbslq_f32(is_min, inf, ts);
  }
}
#endif // ARM_NEON

Intersection BVH::BVH4Node::intersect(BVH *bvh, Ray ray, Vec3 parent_lo,
                                      Vec3 parent_hi) {
  float sxf = (parent_hi.x - parent_lo.x) / 255.0f;
  float syf = (parent_hi.y - parent_lo.y) / 255.0f;
  float szf = (parent_hi.z - parent_lo.z) / 255.0f;

  float rix = 1.0f / ray.direction.x;
  float riy = 1.0f / ray.direction.y;
  float riz = 1.0f / ray.direction.z;
  float roix = ray.origin.x * rix;
  float roiy = ray.origin.y * riy;
  float roiz = ray.origin.z * riz;

  float tminf = 0.0f;
  float tmaxf = std::numeric_limits<float>::infinity();

#ifdef AVX
  return intersect_avx(bvh,
      _mm256_set1_ps(rix), _mm256_set1_ps(riy), _mm256_set1_ps(riz),
      _mm256_set1_ps(roix), _mm256_set1_ps(roiy), _mm256_set1_ps(roiz),
      _mm256_set1_ps(sxf), _mm256_set1_ps(syf), _mm256_set1_ps(szf),
      _mm256_set1_ps(parent_lo.x), _mm256_set1_ps(parent_lo.y),
      _mm256_set1_ps(parent_lo.z), tminf, tmaxf);
#elif defined(ARM_NEON)
  return intersect_neon(
      vdupq_n_f32(rix), vdupq_n_f32(riy), vdupq_n_f32(riz), vdupq_n_f32(-roix),
      vdupq_n_f32(-roiy), vdupq_n_f32(-roiz), vdupq_n_f32(sxf),
      vdupq_n_f32(syf), vdupq_n_f32(szf), vdupq_n_f32(parent_lo.x),
      vdupq_n_f32(parent_lo.y), vdupq_n_f32(parent_lo.z), tminf, tmaxf);
#else
  return intersect_c(bvh, rix, riy, riz, roix, roiy, roiz, sxf, syf, szf,
                     parent_lo.x, parent_lo.y, parent_lo.z, tminf, tmaxf);
#endif
}

#ifdef AVX

// with a lot of help from
// https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf or random
// searches
Intersection
BVH::BVH4Node::intersect_avx(BVH* bvh, __m256 rix, __m256 riy, __m256 riz, __m256 roix,
                        __m256 roiy, __m256 roiz, __m256 sx, __m256 sy,
                        __m256 sz, __m256 plox, __m256 ploy, __m256 ploz,
                        float tminf, float tmaxf) {

  if (internal.children[0] == 0) {
    auto res = ::intersect(bvh->cur_ray, bvh->objs[leaf.ptr].origin.pos, bvh->objs[leaf.ptr].appearance.geometry);
    if (res.valid()) {
      res.obj = &bvh->objs[leaf.ptr];
      return res;
    } else
      return {};
  }

  if (tminf > tmaxf)
    return {};

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
  x = _mm256_fmadd_ps(x, sx, plox);
  y = _mm256_fmadd_ps(y, sy, ploy);
  z = _mm256_fmadd_ps(z, sz, ploz);

  // https://en.wikipedia.org/wiki/Slab_method
  __m256 tx = _mm256_fmsub_ps(x, rix, roix);
  __m256i txi = _mm256_castps_si256(tx);
  // swap lower/upper 128-bit halves (swap lo hi)
  __m256 txs =
      _mm256_castsi256_ps(_mm256_permute2x128_si256(txi, txi, _MM_PERM_AAAB));

  __m128 tc = _mm256_castps256_ps128(_mm256_min_ps(tx, txs));
  __m128 tf = _mm256_castps256_ps128(_mm256_max_ps(tx, txs));

  {
    __m256 ty = _mm256_fmsub_ps(y, riy, roiy);
    __m256i tyi = _mm256_castps_si256(ty);
    __m256 tys =
        _mm256_castsi256_ps(_mm256_permute2x128_si256(tyi, tyi, _MM_PERM_AAAB));

    __m128 tcy = _mm256_castps256_ps128(_mm256_min_ps(ty, tys));
    __m128 tfy = _mm256_castps256_ps128(_mm256_max_ps(ty, tys));
    tc = _mm_max_ps(tc, tcy);
    tf = _mm_min_ps(tf, tfy);
  }
  {

    __m256 tz = _mm256_fmsub_ps(z, riz, roiz);
    __m256i tzi = _mm256_castps_si256(tz);
    __m256 tzs =
        _mm256_castsi256_ps(_mm256_permute2x128_si256(tzi, tzi, _MM_PERM_AAAB));

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

    Intersection best = {};

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

    __m256 xlo = _mm256_permute2f128_ps(x, x, _MM_PERM_AAAA);
    __m256 sx = _mm256_sub_ps(x, xlo);
    sx = _mm256_permute2f128_ps(sx, sx, _MM_PERM_ABAB);

    __m256 ylo = _mm256_permute2f128_ps(y, y, _MM_PERM_AAAA);
    __m256 sy = _mm256_sub_ps(y, ylo);
    sy = _mm256_permute2f128_ps(sy, sy, _MM_PERM_ABAB);

    __m256 zlo = _mm256_permute2f128_ps(z, z, _MM_PERM_AAAA);
    __m256 sz = _mm256_sub_ps(z, zlo);
    sz = _mm256_permute2f128_ps(sz, sz, _MM_PERM_ABAB);

    int offset = internal.children[min_i];

    if (offset == 0 || ts[min_i] > tmaxf)
      return best;

    auto res = this[internal.children[min_i]].intersect_avx(bvh,
        rix, riy, riz, roix, roiy, roiz, sx, sy, sz, xlo, ylo, zlo, tminf,
        best.dist);

    if (res.valid() && res.dist < best.dist) best = res;

    // try next by removing cur and re reducing min
    _mm_blendv_ps(tc, inf, min_eq);
  }

    return best;
}
#endif // AVX
