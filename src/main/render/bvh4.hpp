#include "math/ray.hpp"
#include <cstdint>
#include <optional>

#ifdef AVX
#include <immintrin.h>
#endif // AVX

// storing quantized positions, 0 is lo of parent and 255 is hi of parent
// aligned to 32 bytes so all data of an instance will be on the same cache line
// children[4] - location of each child stored as offset from current pos
// offset is stored in units of 32 bytes (children[0]=1 means its 32 bytes away)
// xlohi - [xlo[4], xhi[4]]
// this is inspired by what embree does
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

union BVHNode {
  BVH4Internal internal;
  BVH4Leaf leaf;

  void intersect(Ray ray, Vec3 parent_lo, Vec3 parent_hi);
#ifdef AVX

  // ri is 1/ray.direction component wise
  // roiz is ray.origin/ray.direction component wise
  // this is done so slab method is a bunch of fmsubs
  std::optional<uint32_t> intersect_avx(__m256 rix, __m256 riy, __m256 riz, __m256 roix,
                     __m256 roiy, __m256 roiz, float sxf, float syf, float szf,
                     float ploxf, float ployf, float plozf, float tminf,
                     float tmaxf);
#endif // AVX
};

static_assert(sizeof(BVHNode) == 32);
static_assert(alignof(BVHNode) == 32);
