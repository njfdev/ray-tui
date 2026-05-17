#pragma once

#include "math/ray.hpp"
#include "render/render_data.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdlib>
#include "scene/geometry.hpp"

struct AABB {
  Vec3 lo;
  Vec3 hi;
};

struct Intersection {
  double dist = INFINITY;
  Vec3 p;
  Vec3 normal;
  RenderData *obj = nullptr;

  inline bool valid() { return dist != INFINITY; }
};

Intersection intersect(const Ray &ray, Vec3 origin, const Sphere &sphere);
Intersection intersect(const Ray &ray, Vec3 origin, const Plane &plane);

AABB bounds(Vec3 origin, const Sphere &sphere);
AABB bounds(Vec3 origin, const Plane &plane);
AABB bounds(Vec3 origin, const EmptyGeometry &empty);

Intersection intersect(const Ray &r, Vec3 origin, const Geometry &shape);
AABB bounds(Vec3 origin, const Geometry &shape);
