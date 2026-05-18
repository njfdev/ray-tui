#pragma once

#include "math/vec3.hpp"
#include <variant>

/**
 * Used for a sphere with a certain radius.
 */
struct Sphere {
  double radius;
};

/**
 * Used for a flat square only viewable from one side (the normal facing).
 */
struct Plane {
  Vec3 normal;
  double extent = 10;
};

/**
 * Used for shapeless objects, such as a point light.
 */
struct EmptyGeometry {};

/**
 * Union type for one of the various shapes an object can take on.
 */
using Geometry = std::variant<Sphere, Plane, EmptyGeometry>;
