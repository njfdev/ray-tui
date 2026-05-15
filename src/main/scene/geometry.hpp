#pragma once

#include "math/vec3.hpp"
#include <variant>

struct Sphere {
  double radius;
};

struct Plane {
  Vec3 normal;
  double extent = 10;
};

struct PointLight {};

using Geometry = std::variant<Sphere, Plane, PointLight>;
