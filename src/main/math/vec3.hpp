#pragma once
#include <cmath>

/**
 * 3D Vector class, all operations O(1)
 */
struct Vec3 {
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;

  /** @returns the squared magnitude of this vector */
  inline float sqrLength() const { return x * x + y * y + z * z; }

  /** @returns the magnitude of this vector */
  inline float length() const { return std::sqrt(x * x + y * y + z * z); }

  /** @returns a vector with the same direction as this vector, but with a
   * magnitude of 1 */
  Vec3 normalize() const { return *this / length(); }

  /** vector addition */
  inline Vec3 operator+(const Vec3 &rhs) const {
    return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
  }

  /** in place vector addition */
  inline void operator+=(const Vec3 &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }

  /** vector equality */
  inline bool operator==(const Vec3 &rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }

  /** vector subtraction */
  inline Vec3 operator-(const Vec3 &rhs) const {
    return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
  }

  /** vector-scalar division */
  inline Vec3 operator/(const float s) const {
    return Vec3{x / s, y / s, z / s};
  }

  /** vector-scalar multiplication */
  inline Vec3 operator*(const float s) const {
    return Vec3{s * x, s * y, s * z};
  }

  /** vector-scalar multiplication */
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

  /** component-wise multiplication of two vectors */
  inline Vec3 component_mul(const Vec3 &rhs) const {
    return Vec3{rhs.x * x, rhs.y * y, rhs.z * z};
  }

  /** component-wise division of two vectors */
  inline Vec3 component_div(const Vec3 &rhs) const {
    return Vec3{x / rhs.x, y / rhs.y, z / rhs.z};
  }
};
