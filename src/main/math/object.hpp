#pragma once
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include <optional>

struct Intersection {
  Vec3 p;
  Vec3 normal;
  double u;
  double v;
};

class RayObject {
public:
  virtual std::optional<Intersection> intersect(const Ray &ray) = 0;
};
