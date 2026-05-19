#include "math/intersection.hpp"
#include "render/bvh.hpp"
#include <algorithm>
#include <limits>

// same logic as avx intersection, but scalar.
Intersection BVH::BVH4Node::intersect_c(BVH *bvh, float rix, float riy,
                                        float riz, float roix, float roiy,
                                        float roiz, float sxf, float syf,
                                        float szf, float ploxf, float ployf,
                                        float plozf, float tminf, float tmaxf) {
  // base case: ray bounds impossible. ordering this before other base case
  // because its cheaper.
  if (tminf > tmaxf)
    return {};

  // base case: union is BVH4Leaf, not BVH4Internal. call ray-shape
  // intersection.
  if (internal.children[0] == 0) {
    auto res = ::intersect(bvh->cur_ray, bvh->objs[leaf.idx].origin.pos,
                           bvh->objs[leaf.idx].appearance.geometry);
    if (res.valid()) {
      res.obj = &bvh->objs[leaf.idx];
      return res;
    } else
      return {};
  }

  float ts[4];
  bool hit[4];
  float xlo_w[4], xhi_w[4], ylo_w[4], yhi_w[4], zlo_w[4], zhi_w[4];

  for (int i = 0; i < 4; i++) {
    // dequantize
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

    // slab method
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
    
    // if hit invalid dist = infinity
    ts[i] = hit[i] ? tc : std::numeric_limits<float>::infinity();
  }

  // visit by increasing intersection distance
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

    // base case: no more children that could possibly contain closer intersection
    if (offset == 0 || ts[i] > tmaxf)
      return best;

    // visit child
    auto res = this[internal.children[i]].intersect_c(
        bvh, rix, riy, riz, roix, roiy, roiz, child_sxf, child_syf, child_szf,
        xlo_w[i], ylo_w[i], zlo_w[i], tminf, best.dist);

    if (res.valid() && res.dist < best.dist)
      best = res;
  }

  return best;
}

// intersects a ray with the BVH tree, doing a DFS ordered by bounding box
// intersection distance.
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
  return intersect_avx(
      bvh, _mm256_set1_ps(rix), _mm256_set1_ps(riy), _mm256_set1_ps(riz),
      _mm256_set1_ps(roix), _mm256_set1_ps(roiy), _mm256_set1_ps(roiz),
      _mm256_set1_ps(sxf), _mm256_set1_ps(syf), _mm256_set1_ps(szf),
      _mm256_set1_ps(parent_lo.x), _mm256_set1_ps(parent_lo.y),
      _mm256_set1_ps(parent_lo.z), tminf, tmaxf);
#else
  return intersect_c(bvh, rix, riy, riz, roix, roiy, roiz, sxf, syf, szf,
                     parent_lo.x, parent_lo.y, parent_lo.z, tminf, tmaxf);
#endif
}

#ifdef AVX
// with a lot of help from
// https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf or random
// searches
//
// ri is 1/ray.direction component wise
// roi is ray.origin/ray.direction component wise (= ray.origin * ri)
// sxf/syf/szf is (parent_hi - parent_lo) / 255 per axis
// ploxf/ployf/plozf is parent_lo per axis
// see intersect(...) for info
Intersection BVH::BVH4Node::intersect_avx(BVH *bvh, __m256 rix, __m256 riy,
                                          __m256 riz, __m256 roix, __m256 roiy,
                                          __m256 roiz, __m256 sx, __m256 sy,
                                          __m256 sz, __m256 plox, __m256 ploy,
                                          __m256 ploz, float tminf,
                                          float tmaxf) {

  // base case: ray bounds impossible. ordering this before other base case
  // because its cheaper.
  if (tminf > tmaxf)
    return {};

  // base case: union is BVH4Leaf, not BVH4Internal. call ray-shape
  // intersection.
  if (internal.children[0] == 0) {
    auto res = ::intersect(bvh->cur_ray, bvh->objs[leaf.idx].origin.pos,
                           bvh->objs[leaf.idx].appearance.geometry);
    if (res.valid()) {
      res.obj = &bvh->objs[leaf.idx];
      return res;
    } else
      return {};
  }

  // dequantize bytes to floats:
  // - load copies of the 8 bytes into the 8 slots (loading as i64)
  // - _mm256_cvtepu8_epi32 unpacks bytes to i32s with leading 0s
  // - i32 -> float
  // - scale down of 255 handled via sx, sy, sz (built in 1/255)
  // layout of x,y,z is (x as example):
  //    bottom 128 bits     |     top 128 bits
  // xlo0, xlo1, xlo2, xlo3 | xhi0, xhi1, xhi2, xhi3
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

  // since rix is 1/ray.dir.x, roix is ray.origin/ray.dir.x,
  // need (p - ray.origin)/ray.dir component wise for slab method,
  // (for x) = p.x/ray.dir.x - ray.origin/ray.dir.x = x*rix - roix
  // since its a*b - c can use fused sub
  __m256 tx = _mm256_fmsub_ps(x, rix, roix);
  __m256i txi = _mm256_castps_si256(tx);
  // swap lower/upper 128-bit halves (swap lo hi), this lets the min and max be
  // taken between lo and hi
  __m256 txs =
      _mm256_castsi256_ps(_mm256_permute2x128_si256(txi, txi, _MM_PERM_AAAB));

  // close and far ts (rn for x but these will be used to reduce all axes)
  // tx = [ los, his ]
  // tcs= [ his, los ]
  // max/min(los,his) = max/min(his,los), so only need bottom 128 bits
  __m128 tc = _mm256_castps256_ps128(_mm256_min_ps(tx, txs));
  __m128 tf = _mm256_castps256_ps128(_mm256_max_ps(tx, txs));

  {
    // same thing but for y, and also reducing into tc/tf
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
    // same thing but for z
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

  // tc <= tf ? if not, hit invalid
  __m128 hit_mask = _mm_cmple_ps(tc, tf);

  __m128 inf = _mm_set1_ps(std::numeric_limits<float>::infinity());
  __m128 ts = _mm_blendv_ps(
      inf, tc, hit_mask); // ray hit distances (set misses to infinity)

  Intersection best = {};

  // computation of vectors for next iteration
  //
  // x is [ xlos, xhis ]
  // to get xlos, permute so it is [ xlos, xlos ]
  // sx is xhis-xlos, just sub x-xlos
  // then copy uppper 128bits into lower 128 bits
  __m256 inv255 = _mm256_set1_ps(1.0f / 255.0f);

  __m256 xlo_all = _mm256_permute2f128_ps(x, x, _MM_PERM_AAAA);
  __m256 sx_all = _mm256_mul_ps(_mm256_sub_ps(x, xlo_all), inv255);
  sx_all = _mm256_permute2f128_ps(sx_all, sx_all, _MM_PERM_ABAB);

  __m256 ylo_all = _mm256_permute2f128_ps(y, y, _MM_PERM_AAAA);
  __m256 sy_all = _mm256_mul_ps(_mm256_sub_ps(y, ylo_all), inv255);
  sy_all = _mm256_permute2f128_ps(sy_all, sy_all, _MM_PERM_ABAB);

  __m256 zlo_all = _mm256_permute2f128_ps(z, z, _MM_PERM_AAAA);
  __m256 sz_all = _mm256_mul_ps(_mm256_sub_ps(z, zlo_all), inv255);
  sz_all = _mm256_permute2f128_ps(sz_all, sz_all, _MM_PERM_ABAB);

  while (true) {
    // find child with closest bounding box to test first.
    // this shuffle then min causes then lane with the min value to be spread to
    // all other lanes
    // sorting lanes of a vector is unfortunately not supported so we have to do
    // this lol
    __m128 shuf = _mm_shuffle_ps(ts, ts, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 min1 = _mm_min_ps(ts, shuf);
    shuf = _mm_shuffle_ps(min1, min1, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 min = _mm_min_ps(min1, shuf); // [min, min, min, min]
    __m128 min_eq = _mm_cmpeq_ps(
        ts, min); // [0 in lanes that did not contain min, 1 in lanes that did]

    __m128 valid_min = _mm_and_ps(
        hit_mask,
        min_eq); // makes sure the min dist was actually a hit, if there is 1
                 // hit left it will always be because dist of miss is infinity.
    int min_bit = _mm_movemask_ps(valid_min);
    if (min_bit == 0)
      return {}; // no collision
    int min_i = _tzcnt_u32(min_bit);

    int offset = internal.children[min_i];

    // base case: no more children that could possibly contain closer intersection
    if (offset == 0 || ts[min_i] > tmaxf)
      return best;

    // spread valid child lane (avoids extract->scalar load)
    __m256i idx = _mm256_set1_epi32(min_i);
    __m256 sx = _mm256_permutevar8x32_ps(sx_all, idx);
    __m256 sy = _mm256_permutevar8x32_ps(sy_all, idx);
    __m256 sz = _mm256_permutevar8x32_ps(sz_all, idx);
    __m256 xlo = _mm256_permutevar8x32_ps(xlo_all, idx);
    __m256 ylo = _mm256_permutevar8x32_ps(ylo_all, idx);
    __m256 zlo = _mm256_permutevar8x32_ps(zlo_all, idx);

    // visit child
    auto res = this[internal.children[min_i]].intersect_avx(
        bvh, rix, riy, riz, roix, roiy, roiz, sx, sy, sz, xlo, ylo, zlo, tminf,
        best.dist);

    if (res.valid() && res.dist < best.dist)
      best = res;

    // try next by removing cur (setting dist to infinity) and re reducing on
    // next iter. need to recreate mask from min_i incase there are distance
    // ties.
    __m128 visited = _mm_castsi128_ps(
        _mm_cmpeq_epi32(_mm_set_epi32(3, 2, 1, 0), _mm_set1_epi32(min_i)));
    ts = _mm_blendv_ps(ts, inf, visited);
  }

  return best;
}
#endif // AVX
