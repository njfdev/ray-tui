#include <cmath>

class Vec3 {
public: 
  double x;
  double y;
  double z;

  Vec3() {
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
  }

  Vec3(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  
  inline double sqrLength() {
    return x*x + y*y + z*z;
  }

  inline double length() {
    return std::sqrt(x*x + y*y + z*z);
  }

  Vec3 normalize() {
    return *this/length();
  }

  inline Vec3 operator +(const Vec3& rhs) {
    return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  inline Vec3 operator -(const Vec3& rhs) {
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  inline Vec3 operator /(const double s) {
    return Vec3(x/s, y/s, z/s);
  }

  inline Vec3 operator *(const double s) {
    return Vec3(s*x, s*y, s*z);
  }

  inline double operator *(const Vec3& rhs) {
    return x*rhs.x + y*rhs.y + z*rhs.z;
  }
};
