#pragma once
#include <cmath>

/**
 * 3D Vector class, all operations O(1)
 */
struct Vec3 {
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;

  inline float sqrLength() const { return x * x + y * y + z * z; }

  inline float length() const { return std::sqrt(x * x + y * y + z * z); }

  Vec3 normalize() const { return *this / length(); }

  inline Vec3 operator+(const Vec3 &rhs) const {
    return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
  }

  inline void operator+=(const Vec3 &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }

  inline bool operator==(const Vec3 &rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }

  inline Vec3 operator-(const Vec3 &rhs) const {
    return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
  }

  inline Vec3 operator/(const float s) const {
    return Vec3{x / s, y / s, z / s};
  }

  friend inline Vec3 operator/(const float s, const Vec3 rhs) {
    return Vec3{rhs.x / s, rhs.y / s, rhs.z / s};
  }

  inline Vec3 operator*(const float s) const {
    return Vec3{s * x, s * y, s * z};
  }

  friend inline Vec3 operator*(const float s, const Vec3 rhs) {
    return Vec3{rhs.x * s, rhs.y * s, rhs.z * s};
  }

  /**
   * Dot product
   *
   * @param rhs the other vector
   *
   * @returns the dot product of {this} and {rhs}
   */
  inline float operator*(const Vec3 &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  /**
   * Cross product
   *
   * @param rhs the other vector
   *
   * @returns the cross product of {this} and {rhs}
   */
  inline Vec3 operator^(const Vec3 &rhs) const {
    return Vec3{y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x};
  }

  inline Vec3 component_mul(const Vec3 &rhs) const {
    return Vec3{rhs.x * x, rhs.y * y, rhs.z * z};
  }

  inline Vec3 component_div(const Vec3 &rhs) const {
    return Vec3{x / rhs.x, y / rhs.y, z / rhs.z};
  }
};
