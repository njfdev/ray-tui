#pragma once

#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"
#include "scene/geometry.hpp"
#include <cstdint>
#include <vector>

#ifdef AVX
#include <immintrin.h>
#endif // AVX

class BVH {
public:
  // storing quantized positions, 0 is lo of parent and 255 is hi of parent
  // aligned to 32 bytes so all data of an instance will be on the same cache
  // line children[4] - location of each child stored as offset from current pos
  // offset is stored in units of 32 bytes (children[0]=1 means its 32 bytes
  // away) xlohi - [xlo[4], xhi[4]] this is inspired by what embree does
  //
  // if children[0] = 0 then BVH4Internal else BVH4Leaf, child cannot exist at 0
  // offset from parent and children are stored with trailing 0s, not leading.
  // this is used as a zero-overhead tag in the Node union. This is
  // explicitly not UB according to C++17 12.2.23 (standard version N4713) since
  // all structs in the union have the same layout and initial sequence
  struct alignas(32) BVH4Internal {
    uint16_t children[4];
    uint64_t xlohi, ylohi, zlohi;
  };

  struct alignas(32) BVH4Leaf {
    uint16_t children[4];
    uint32_t ptr;
  };

  union BVH4Node {
    BVH4Internal internal;
    BVH4Leaf leaf;

    // ri is 1/ray.direction component wise
    // roi is ray.origin/ray.direction component wise (= ray.origin * ri)
    // sxf/syf/szf is (parent_hi - parent_lo) / 255 per axis
    // ploxf/ployf/plozf is parent_lo per axis
    Intersection intersect(BVH *bvh, Ray ray, Vec3 parent_lo, Vec3 parent_hi);

    Intersection intersect_c(BVH *bvh, float rix, float riy, float riz,
                             float roix, float roiy, float roiz, float sxf,
                             float syf, float szf, float ploxf, float ployf,
                             float plozf, float tminf, float tmaxf);

#ifdef AVX
    Intersection intersect_avx(BVH *bvh, __m256 rix, __m256 riy,
                                          __m256 riz, __m256 roix, __m256 roiy,
                                          __m256 roiz, __m256 sx, __m256 sy,
                                          __m256 sz, __m256 plox, __m256 ploy,
                                          __m256 ploz, float tminf,
                                          float tmaxf);
#endif // AVX
  };

private:
  static_assert(sizeof(BVH4Node) == 32);
  static_assert(alignof(BVH4Node) == 32);

  std::vector<BVH4Node> nodes = {};
  std::vector<Geometry> shapes = {};
  std::vector<RenderData> objs = {};
  std::vector<AABB> bounds = {};
  AABB root_bounds = {};
  Ray cur_ray = Ray(Vec3{1.0, 0.0, 0.0});

public:
  RenderData *insert(AABB bounds, Position origin, Renderable appearance);
  void remove(uint32_t obj);

  void construct();

  Intersection intersect(Ray ray);
};
