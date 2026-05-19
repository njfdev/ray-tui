#pragma once

#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "render/render_data.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"
#include "scene/geometry.hpp"
#include <cstdint>
#include <memory>
#include <vector>

#ifdef AVX
#include <immintrin.h>
#endif // AVX

class BVH {
private:
  /**
   * Internal node of the BVH tree. Has 4 children.
   *
   * storing quantized positions, 0 is lo of parent and 255 is hi of parent
   * aligned to 32 bytes so all data of an instance will be on the same cache
   * line
   */
  struct alignas(32) BVH4Internal {
    /**
     * children[4] - location of each child stored as offset from current pos
     * offset is stored in units of 32 bytes (children[0]=1 means its 32 bytes
     * away)
     */
    uint16_t children[4];

    /**
     * xlohi - [xlo[4], xhi[4]] (each is a byte) this is inspired by what
     * embree does.
     * ylohi - same
     * zlohi - same
     */
    uint64_t xlohi, ylohi, zlohi;
  };

  /**
   * Leaf of the BVH tree.
   */
  struct alignas(32) BVH4Leaf {
    uint16_t children[4]; // needed to be able to use this as tag, will be all
                          // 0s for leaf.
    uint32_t idx; // idx of the object corresponding to this leaf in the obj and
                  // shape arrays.
  };

  /**
   * children[0] used as union tag.
   *
   * if children[0] = 0 then BVH4Internal else BVH4Leaf, child cannot exist at 0
   * offset from parent and children are stored with trailing 0s, not leading.
   * this is used as a zero-overhead tag in the Node union. This is explicitly
   * not UB according to C++17 12.2.23 (standard version N4713) since all
   * structs in the union have the same layout and initial sequence
   */
  union BVH4Node {
    BVH4Internal internal;
    BVH4Leaf leaf;

    /**
     * intersects {ray} with the BVH
     *
     * hands off to {intersect_c} or {intersect_avx} depending on compile flags.
     */
    Intersection intersect(BVH *bvh, Ray ray, Vec3 parent_lo, Vec3 parent_hi);

    // ri is 1/ray.direction component wise
    // roi is ray.origin/ray.direction component wise (= ray.origin * ri)
    // sxf/syf/szf is (parent_hi - parent_lo) / 255 per axis
    // ploxf/ployf/plozf is parent_lo per axis
    Intersection intersect_c(BVH *bvh, float rix, float riy, float riz,
                             float roix, float roiy, float roiz, float sxf,
                             float syf, float szf, float ploxf, float ployf,
                             float plozf, float tminf, float tmaxf);

#ifdef AVX
    // ri is 1/ray.direction component wise
    // roi is ray.origin/ray.direction component wise (= ray.origin * ri)
    // sxf/syf/szf is (parent_hi - parent_lo) / 255 per axis
    // ploxf/ployf/plozf is parent_lo per axis
    Intersection intersect_avx(BVH *bvh, __m256 rix, __m256 riy, __m256 riz,
                               __m256 roix, __m256 roiy, __m256 roiz, __m256 sx,
                               __m256 sy, __m256 sz, __m256 plox, __m256 ploy,
                               __m256 ploz, float tminf, float tmaxf);
#endif // AVX
  };

  // needed for children[4] offset logic to work properly
  static_assert(sizeof(BVH4Node) == 32);
  static_assert(alignof(BVH4Node) == 32);

  std::vector<BVH4Node> nodes = {};
  std::vector<RenderData> objs = {};
  AABB root_bounds = {};
  Ray cur_ray = Ray(Vec3{1.0, 0.0, 0.0});

  struct TempNode;
  struct BuildPrim;

  static int serialize(std::vector<BVH::BVH4Node> &nodes, const TempNode *node);

  static std::unique_ptr<BVH::TempNode>
  build_node(std::vector<BuildPrim> &prims, int lo, int hi);

  static AABB prims_bounds(std::vector<BVH::BuildPrim> &prims, int lo, int hi);

public:
  struct BVHObjectID {
    uint32_t id;
  };

  /**
   * Inserts an object into the BVH. The BVH must be constructed before the
   * object is visible
   *
   * @param pos the position component of the object
   * @param renderable the renderable component of the object
   *
   * @returns the ID of the inserted object
   *
   *
   * Time complexity: O(1)
   */
  BVHObjectID insert(Position pos, Renderable renderable);

  /**
   * Removes an object from the BVH. It will first hide it, then the next time
   * {construct} is called it will be eliminated
   *
   * @param obj the id of the object to remove
   *
   * Time complexity: O(N) (needs to search for matching ID)
   */
  void remove(BVHObjectID obj);

  /**
   * Constructs the BVH.
   *
   * Time complexity:
   * At each depth it
   * - computes prims_bounds -> O(N)
   * - sorts the range using std::sort -> O(N log(N)) (has to resort bc it
   * chooses different split axi)
   * - splits into 4 equal groups and recurses (next layer still visits all
   * nodes)
   *
   * result: O(N log(N)) * depth. Since splits into 4 for every depth, depth =
   * log(N)
   *
   * total: O(N*log(N)*log(N))
   */
  void construct();

  /**
   * Intersects a ray with the BVH.
   *
   * @param ray the ray to intersect
   *
   * Time complexity (average):
   * N is the number of objects.
   * for a well built sparse BVH, ray will only hit 1/2 child BVHs per node,
   * resulting in O(log N) traversal. all ray-geometry intersections are O(1).
   *
   * Average: O(log N)
   *
   * Time complexity (worst case):
   * N is the number of objects.
   * if the ray intersects with every bounding box at every level, will visit
   * O(N + N/4 + N/16 < 2N) nodes.
   *
   * Worse case: O(N)
   */
  Intersection intersect(Ray ray);
};
