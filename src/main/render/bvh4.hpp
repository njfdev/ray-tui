#include "math/ray.hpp"
#include <cstdint>
#include <xmmintrin.h>

// storing quantized positions, 0 is lo of parent and 255 is hi of parent
// aligned to 32 bytes so all data of an instance will be on the same cache line
// children[4] - location of each child stored as offset from current pos
// offset is stored in units of 32 bytes (children[0]=1 means its 32 bytes away)
// xlohi - [xlo[4], xhi[4]]
// this is inspired by what embree does
struct alignas(32) BVH4Internal {
  uint16_t children[4];
  uint64_t xlohi, ylohi, zlohi;
};

struct BVH4Leaf {
  uint16_t children[4];
  uint32_t ptr;
};

// if origin_dist = +INF then ObjRef, if -INF then ChunkRef, if NAN then
// Empty. this is used as a zero-overhead tag in the Node union. This is
// explicitly not UB according to C++17 12.2.23 (standard version N4713) since
// all structs in the union have the same layout and initial sequence
union BVHNode {
  BVH4Internal internal;
  BVH4Leaf leaf;

  void intersect(Ray ray, Vec3 parent_lo, Vec3 parent_hi);
#ifdef SSE

  // ri is 1/ray.direction component wise
  // roiz is ray.origin/ray.direction component wise
  // this is done so slab method is a bunch of fmsubs 
  void intersect_sse(__m128 rix, __m128 riy, __m128 riz, __m128 roix, __m128 roiy,
                     __m128 roiz, float sxf, float syf, float szf, float ploxf,
                     float ployf, float plozf, float tminf, float tmaxf);
#endif // SSE
};

static_assert(sizeof(BVHNode) == 32);
static_assert(alignof(BVHNode) == 32);
