#include "shapes.hpp"

constexpr double EPSILON = 1e-6;

std::optional<Intersection> Sphere::intersect(const Ray &ray) {
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
  double r2 = radius * radius;

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

  // spherical coordinates for uv
  double u = atan2(normal.y, normal.x);
  double v = acos(normal.z / radius);

  return std::optional<Intersection>(Intersection{p, normal, t0, u, v});
}

// inspired by
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
// ray collides with face that has normal going out of it
std::optional<Intersection> Plane::intersect(const Ray &ray) {
  double denom = (ray.direction * normal);

  // ray must have a negative component along normal for it to collide
  // (because ray travels in +dir direction)
  if (denom > 0.0)
    return {};

  double t = ((origin - ray.origin) * normal) / denom;

  // collision point cannot be behind ray origin
  if (t < 0.0)
    return {};

  Vec3 p = t * ray.direction + ray.origin;
  Vec3 d = p - origin;

  // below are simplified cross products, doing the same as the commented code:
  // Vec3 uAxis = (Vec3 {0.0,0.0,1.0}^normal).normalize();
  // Vec3 vAxis = uAxis^normal;
  // double u = d*uAxis;
  // double v = d*vAxis;

  Vec3 uAxis = (Vec3{-normal.y, normal.x, 0.0}).normalize();

  // uAxis and normal are perpendicular and normalized, so their cross will be
  // normalized already.
  Vec3 vAxis = Vec3{// y*rhs.z - z*rhs.y -> y*rhs.z -> uAxis.x*normal.z
                    normal.x * normal.z,
                    // z*rhs.x - x*rhs.z -> -x*rhs.z -> uAxis.y*normal.z,
                    normal.y * normal.z,
                    // x*rhs.y - y*rhs.x -> uAxis.x*normal.y - uAxis.y*normal.x
                    -normal.y * normal.y - normal.x * normal.x};

  double u = uAxis * d;
  double v = vAxis * d;

  return std::optional<Intersection>{Intersection{p, normal, t, u, v}};
}
