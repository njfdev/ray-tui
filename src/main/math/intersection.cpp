#include "intersection.hpp"
#include "scene/geometry.hpp"
#include <iostream>
#include <stdexcept>

Intersection intersect(const Ray &ray, Vec3 origin, const Sphere &sphere) {
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
  Vec3 OS = origin - ray.origin;
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

  Vec3 normal = (p - origin).normalize();

  return Intersection{t0, p, normal};
}

// inspired by
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
// ray collides with face that has normal going out of it
Intersection intersect(const Ray &ray, Vec3 origin, const Plane &plane) {
  double denom = (ray.direction * plane.normal);

  // ray must have a negative component along normal for it to collide
  // (because ray travels in +dir direction)
  if (denom > 0.0)
    return {};

  double t = ((origin - ray.origin) * plane.normal) / denom;

  // collision point cannot be behind ray origin
  if (t < 0.0)
    return {};

  Vec3 p = t * ray.direction + ray.origin;

  return Intersection{t, p, plane.normal};
}

AABB bounds(Vec3 origin, const Sphere &sphere) {
  return AABB{
      origin - Vec3{1.0, 1.0, 1.0} * sphere.radius,
      origin + Vec3{1.0, 1.0, 1.0} * sphere.radius,
  };
}

AABB bounds(Vec3 origin, const Plane &plane) {
  Vec3 z = Vec3{0.0, 0.0, 1.0};
  if (plane.normal == z) {
    z = Vec3{1.0, 0.0, 0.0};
  }

  Vec3 axis1 = (plane.normal ^ z).normalize();
  Vec3 axis2 = plane.normal ^ axis1;

  Vec3 c1 = axis1 * plane.extent + axis2 * plane.extent;

  Vec3 offset = Vec3{std::abs(c1.x) , std::abs(c1.y), std::abs(c1.z)};

  return AABB{origin - offset, origin + offset};
}

AABB bounds(Vec3 origin, const EmptyGeometry &empty) {
  return AABB{};
}

Intersection intersect(const Ray &r, Vec3 origin, const Geometry &shape) {
  // leaving normal unnormalized, normalizin
  return std::visit([&](const auto &s) { return intersect(r, origin, s); }, shape);
}

AABB bounds(Vec3 origin, const Geometry &shape) {
  // leaving normal unnormalized, normalizin
  return std::visit([&](const auto &s) {
    return bounds(origin, s);
  }, shape);
}
