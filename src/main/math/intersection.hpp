#pragma once

#include "math/ray.hpp"
#include "render/render_data.hpp"
#include "scene/geometry.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdlib>

/**
 * Axis aligned bounding box, defined by lo/his of each axis.
 */
struct AABB {
  /**
   * The position of the corner that is at the lower end of each axis
   */
  Vec3 lo;

  /**
   * The position of the corner that is at the upper end of each axis
   */
  Vec3 hi;
};

/**
 * Data about an intersection
 * default constructor represents no intersection (checked via .valid()) -
 * avoids needing to make std::option<Intersection>
 */
struct Intersection {
  /**
   * Distance from ray origin to intersection
   */
  double dist = INFINITY;

  /**
   * Point of intersection
   */
  Vec3 p;

  /**
   * normal at intersction point. will be a normalized vector.
   */
  Vec3 normal;

  /**
   * optional: render data associated with hit object
   */
  RenderData *obj = nullptr;

  /**
   * @returns whether or not this represents a valid intersection
   *
   * O(1)
   */
  inline bool valid() { return dist != INFINITY; }
};

/**
 * Intersects a ray with a Geometry via std::visit, resulting in compile-time
 * dynamic dispatch. std::visit with single input and a variant with <11
 * possibilities inlines in modern gcc, so this will prob inline (w gcc atleast)
 *
 * @param r the ray to intersect
 * @param origin the origin of the shape
 * @param shape the shape to intersect
 *
 * @returns a possibly-invalid instance of Intersection that describes the
 * collision (or lack thereof) between {r} and {shape}.
 *
 * all paths O(1)
 */
Intersection intersect(const Ray &r, Vec3 origin, const Geometry &shape);

/**
 * Calculates axis-aligned bounds via std::visit, resulting in compile-time
 * dynamic dispatch. std::visit with single input and a variant with <11
 * possibilities inlines in modern gcc, so this will prob inline (w gcc atleast)
 *
 * @param origin the origin of the shape
 * @param shape the shape to bound
 *
 * @returns The AABB that bounds {shape}
 *
 * all paths O(1)
 */
AABB bounds(Vec3 origin, const Geometry &shape);
