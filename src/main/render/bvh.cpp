#include "render/bvh.hpp"
#include "math/intersection.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

struct BuildPrim {
  uint32_t idx;
  AABB bounds;
  Vec3 centroid;
};

struct TempNode {
  bool is_leaf;
  AABB bounds;
  uint32_t prim_idx;
  std::size_t n_children;
  std::unique_ptr<TempNode> children[4];
};

static AABB aabb_union(const AABB &a, const AABB &b) {
  return {
      {std::min(a.lo.x, b.lo.x), std::min(a.lo.y, b.lo.y),
       std::min(a.lo.z, b.lo.z)},
      {std::max(a.hi.x, b.hi.x), std::max(a.hi.y, b.hi.y),
       std::max(a.hi.z, b.hi.z)},
  };
}

static AABB prims_bounds(std::vector<BuildPrim> &prims, int lo, int hi) {
  AABB b = prims[lo].bounds;
  for (int i = lo + 1; i < hi; i++)
    b = aabb_union(b, prims[i].bounds);
  return b;
}

static std::unique_ptr<TempNode> build_node(std::vector<BuildPrim> &prims,
                                            int lo, int hi) {
  int len = hi - lo;
  assert(n > 0);

  if (len == 1) {
    auto node = std::make_unique<TempNode>();
    node->is_leaf = true;
    node->bounds = prims[lo].bounds;
    node->prim_idx = prims[lo].idx;
    node->n_children = 0;
    return node;
  }

  AABB pb = prims_bounds(prims, lo, hi);
  Vec3 span = {pb.hi.x - pb.lo.x, pb.hi.y - pb.lo.y, pb.hi.z - pb.lo.z};
  int axis = 0;
  if (span.y > span.x)
    axis = 1;
  if (span.z > (axis == 0 ? span.x : span.y))
    axis = 2;

  // parition by centroids on longest axis
  std::sort(prims.begin() + lo, prims.begin() + hi,
            [axis](const BuildPrim &a, const BuildPrim &b) {
              if (axis == 0)
                return a.centroid.x < b.centroid.x;
              if (axis == 1)
                return a.centroid.y < b.centroid.y;
              return a.centroid.z < b.centroid.z;
            });

  // equally parition into 4 groups, def a better way to do this but wtv.
  int mid = lo + (hi - lo) / 2;
  int q1 = lo + (mid - lo) / 2;
  int q2 = mid + (hi - mid) / 2;

  int ranges[5] = {lo, q1, mid, q2, hi};
  std::unique_ptr<TempNode> child_nodes[4];
  int n = 0;

  for (int r = 0; r < 4; r++) {
    int rlo = ranges[r], rhi = ranges[r + 1];
    if (rlo >= rhi) // disabled
      continue;
    child_nodes[n] = build_node(prims, rlo, rhi);
    n++;
  }

  // if this node only has 1 child delete it (child becomes child of this nodes
  // parent)
  if (n == 1)
    return std::move(child_nodes[0]);

  auto node = std::make_unique<TempNode>();
  node->is_leaf = false;
  node->bounds = pb;
  node->n_children = n;
  for (int i = 0; i < n; i++)
    node->children[i] = std::move(child_nodes[i]);

  for (int i = n; i < 4; i++)
    node->children[i] = 0;
  return node;
}

static uint64_t pack_axis(uint8_t lo4[4], uint8_t hi4[4]) {
  return (uint64_t)lo4[0] | ((uint64_t)lo4[1] << 8) | ((uint64_t)lo4[2] << 16) |
         ((uint64_t)lo4[3] << 24) | ((uint64_t)hi4[0] << 32) |
         ((uint64_t)hi4[1] << 40) | ((uint64_t)hi4[2] << 48) |
         ((uint64_t)hi4[3] << 56);
}

static uint8_t quantize(double val, double lo, double span, bool is_lo) {
  if (span <= 0.0)
    return 0;
  double q = (val - lo) / span * 255.0;
  if (is_lo) {
    // lo, need to round down to not underestimate bounding box
    q = std::floor(q);
  } else {
    q = std::ceil(q);
  }
  if (q < 0.0)
    q = 0.0;
  if (q > 255.0)
    q = 255.0;
  return static_cast<uint8_t>(q);
}

static int serialize(std::vector<BVH::BVH4Node> &nodes, const TempNode *node) {
  int idx = static_cast<int>(nodes.size());
  nodes.push_back(BVH::BVH4Node{}); // add temp for this node, need to visit children
                               // before filling it.

  if (node->is_leaf) {
    nodes[idx].leaf.children[0] = 0; // leaf tag
    nodes[idx].leaf.ptr = node->prim_idx;
    return idx;
  }

  // recursively seriealize children (ends up preorder)
  int child_idxs[4] = {};
  int n = node->n_children;
  for (int i = 0; i < n; i++)
    child_idxs[i] = serialize(nodes, node->children[i].get());

  double spanx = node->bounds.hi.x - node->bounds.lo.x;
  double spany = node->bounds.hi.y - node->bounds.lo.y;
  double spanz = node->bounds.hi.z - node->bounds.lo.z;
  double lox = node->bounds.lo.x;
  double loy = node->bounds.lo.y;
  double loz = node->bounds.lo.z;

  uint8_t xlo[4], xhi[4], ylo[4], yhi[4], zlo[4], zhi[4];

  for (int i = 0; i < n; i++) {
    const AABB &cb = node->children[i]->bounds;

    xlo[i] = quantize(cb.lo.x, lox, spanx, true);
    xhi[i] = quantize(cb.hi.x, lox, spanx, false);
    ylo[i] = quantize(cb.lo.y, loy, spany, true);
    yhi[i] = quantize(cb.hi.y, loy, spany, false);
    zlo[i] = quantize(cb.lo.z, loz, spanz, true);
    zhi[i] = quantize(cb.hi.z, loz, spanz, false);
  }

  nodes[idx].internal.xlohi = pack_axis(xlo, xhi);
  nodes[idx].internal.ylohi = pack_axis(ylo, yhi);
  nodes[idx].internal.zlohi = pack_axis(zlo, zhi);

  for (int i = 0; i < n; i++) {
    int offset = child_idxs[i] - idx;
    assert(offset > 0 && offset <= 0xFFFF);
    nodes[idx].internal.children[i] = static_cast<uint16_t>(offset);
  }

  return idx;
}

SceneObject* BVH::insert(Shape shape, AABB bounds, SceneObject obj) {
  uint32_t idx = static_cast<uint32_t>(shapes.size());
  shapes.push_back(shape);
  objs.push_back(obj);
  this->bounds.push_back(bounds);
  return &objs.back();
}

void BVH::remove(uint32_t idx) {
  // set to degenerate aabb, will be cleaned up on next construct.
  bounds[idx] = {{(float)INFINITY, (float)INFINITY, (float)INFINITY},
                 {(float)-INFINITY, (float)-INFINITY, (float)-INFINITY}};
}

void BVH::construct() {
  nodes.clear();

  // alive primitives
  std::vector<BuildPrim> prims;
  prims.reserve(shapes.size());

  AABB rb = {{(float)INFINITY, (float)INFINITY, (float)INFINITY},
             {(float)-INFINITY, (float)-INFINITY, (float)-INFINITY}};
  bool any = false;

  for (uint32_t i = 0; i < static_cast<uint32_t>(shapes.size()); i++) {
    const AABB &b = bounds[i];
    if (b.lo.x > b.hi.x)
      continue; // disabled

    Vec3 centroid = {(b.lo.x + b.hi.x) * 0.5f, (b.lo.y + b.hi.y) * 0.5f,
                     (b.lo.z + b.hi.z) * 0.5f};
    prims.push_back({i, b, centroid});

    rb = aabb_union(rb, b);
    any = true;
  }

  if (!any)
    return;

  root_bounds = rb;

  auto root = build_node(prims, 0, static_cast<int>(prims.size()));
  serialize(nodes, root.get());
}

Intersection BVH::intersect(Ray ray) {
  cur_ray = ray;
  if (nodes.empty())
    return {};

  return nodes[0].intersect(this, ray, root_bounds.lo, root_bounds.hi);
}
