#include "shapes.hpp"

constexpr double EPSILON = 1e-6;

bool solveQuadratic(const double &b, const double &c, double &x0, double &x1) {
  double discr = b * b - 4 * c;
  if (discr < 0)
    return false;
  else if (discr == 0)
    x0 = x1 = -0.5 * b;
  else {
    double q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
    x0 = q;
    x1 = c / q;
  }
  if (x0 > x1)
    std::swap(x0, x1);

  return true;
}

// from
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
// with changes: assumed ray.direction is normalized, doubles instead of floats.
std::optional<Intersection> Sphere::intersect(const Ray &ray) {
  double t0, t1; // solutions for t if the ray intersects
  // Analytic solution
  Vec3 L = ray.origin - origin;
  double b = 2.0 * (ray.direction * L);
  double c = L * L - radius * radius;
  if (!solveQuadratic(b, c, t0, t1))
    return {};
  if (t0 > t1)
    std::swap(t0, t1);

  if (t0 < 0) {
    t0 = t1; // If t0 is negative, let's use t1 instead.
    if (t0 < 0)
      return {}; // Both t0 and t1 are negative.
  }

  double t = t0;
  Vec3 p = ray.origin + t * ray.direction;
  Vec3 normal = (p - origin).normalize();

  // spherical coordinates for uv
  double u = atan2(normal.y, normal.x);
  double v = acos(normal.z / radius);

  return std::optional<Intersection>(Intersection{p, normal, u, v});
}

// inspired by
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
// ray collides with face that has normal going out of it
std::optional<Intersection> Plane::intersect(const Ray &ray) {
  double denom = (ray.direction * normal);

  // ray and normal must be in opposite directions (ray must go towards
  // normal-out face)
  if (denom > -EPSILON)
    return {};

  double t = ((origin - ray.origin) * normal) / denom;

  // collision point cannot be behind ray origin
  if (t < EPSILON)
    return {};

  Vec3 p = t * ray.direction + ray.origin;
  Vec3 d = p - origin;

  // what we're doing:
  // Vec3 uAxis = (Vec3 {0.0,0.0,1.0}^normal).normalize();
  // Vec3 vAxis = uAxis^normal;
  // double u = d*uAxis;
  // double v = d*vAxis;

  Vec3 uAxis = (Vec3{-normal.y, normal.x, 0.0}).normalize();
  Vec3 vAxis = Vec3{// y*rhs.z - z*rhs.y -> y*rhs.z -> uAxis.x*normal.z
                    normal.x * normal.z,
                    // z*rhs.x - x*rhs.z -> -x*rhs.z -> uAxis.y*normal.z,
                    normal.y * normal.z,
                    // x*rhs.y - y*rhs.x -> uAxis.x*normal.y - uAxis.y*normal.x
                    -normal.y * normal.y - normal.x * normal.x};

  double u = uAxis * d;
  double v = vAxis * d;

  return std::optional<Intersection>{Intersection{p, normal, u, v}};
}
