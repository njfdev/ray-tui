#pragma once

#include "object.hpp"
#include "vec3.hpp"

struct Sphere: public RayObject {
  Vec3 origin;
  double radius;

  std::optional<Intersection> intersect(const Ray &ray) override;
};

struct Plane: public RayObject {
  Vec3 origin;
  Vec3 normal;

  std::optional<Intersection> intersect(const Ray &ray) override;
};
