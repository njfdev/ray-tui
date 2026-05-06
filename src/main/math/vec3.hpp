#pragma once
#include <cmath>

struct Vec3 {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  inline double sqrLength() const { return x * x + y * y + z * z; }

  inline double length() const { return std::sqrt(x * x + y * y + z * z); }

  Vec3 normalize() const { return *this / length(); }

  inline Vec3 operator+(const Vec3 &rhs) const {
    return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
  }

  inline Vec3 operator-(const Vec3 &rhs) const {
    return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
  }

  inline Vec3 operator/(const double s) const {
    return Vec3{x / s, y / s, z / s};
  }

  friend inline Vec3 operator/(const double s, const Vec3 rhs) {
    return Vec3{rhs.x / s, rhs.y / s, rhs.z / s};
  }

  inline Vec3 operator*(const double s) const {
    return Vec3{s * x, s * y, s * z};
  }

  friend inline Vec3 operator*(const double s, const Vec3 rhs) {
    return Vec3{rhs.x * s, rhs.y * s, rhs.z * s};
  }

  // dot product
  inline double operator*(const Vec3 &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  // cross product
  inline Vec3 operator^(const Vec3 &rhs) const {
    return Vec3 {
      y*rhs.z - z*rhs.y,
      z*rhs.x - x*rhs.z,
      x*rhs.y - y*rhs.x
    };
  }
};
