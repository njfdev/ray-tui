#pragma once

#include "math/ray.hpp"
#include "vec3.hpp"
#include <cmath>
#include <variant>

struct SceneObject;

struct Intersection {
  double dist = INFINITY;
  Vec3 p;
  Vec3 normal;
  SceneObject* obj = nullptr;

  inline bool valid() { return dist != INFINITY; }
};

struct Sphere {
  Vec3 origin;
  double radius;
};

struct Plane {
  Vec3 origin;
  Vec3 normal;
};

inline Intersection intersect(const Ray &ray, const Sphere &sphere) {
  // tried analytical approach and it wasnt working for some reason, using
  // geometric.
  //
  // O: ray origin
  // S: sphere origin
  // A: closest point on ray to sphere center (right angle at this point)
  // I: actual intersection point
  //
  // right triangle OSA with A being right angle
  //
  // right triangle ISA with A being right angle

  // ray origin->sphere center
  Vec3 OS = sphere.origin - ray.origin;
  // project onto ray dir, dir is normalized so this is same as length of OA
  double oa = OS * ray.direction;

  // sqr dist from sphere center to A
  double sa2 = OS * OS - oa * oa;
  double r2 = sphere.radius * sphere.radius;

  // if closest ray dist > radius -> no intersection
  if (sa2 > r2)
    return {};

  // two intersection points will be at length (oa +- ai) along ray
  double ai = std::sqrt(r2 - sa2);

  // first possible intersection distance along the ray
  double t0 = oa - ai;
  // if t0 is behind ray origin, try second intersection point
  if (t0 < 0.0)
    t0 = oa + ai;
  // if both intersections are behind ray origin, no intersection
  if (t0 < 0.0)
    return {};

  Vec3 p = ray.origin + t0 * ray.direction;

  Vec3 normal = (p - sphere.origin).normalize();

  return Intersection{t0, p, normal};
}

// inspired by
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
// ray collides with face that has normal going out of it
inline Intersection intersect(const Ray &ray, const Plane &plane) {
  double denom = (ray.direction * plane.normal);

  // ray must have a negative component along normal for it to collide
  // (because ray travels in +dir direction)
  if (denom > 0.0)
    return {};

  double t = ((plane.origin - ray.origin) * plane.normal) / denom;

  // collision point cannot be behind ray origin
  if (t < 0.0)
    return {};

  Vec3 p = t * ray.direction + ray.origin;

  return Intersection{t, p, plane.normal};
}

using Shape = std::variant<Sphere, Plane>;

inline Intersection intersect(const Ray &r, const Shape &shape) {
  // leaving normal unnormalized, normalizin
  return std::visit([&](const auto &s) { return intersect(r, s); }, shape);
}
