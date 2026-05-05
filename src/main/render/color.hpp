#pragma once
#include <algorithm>
#include <cstdint>

struct Color {
  double r;
  double g;
  double b;
  double a;

  inline Color operator+(const Color &rhs) const {
    return Color{r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a};
  }

  inline Color operator-(const Color &rhs) const {
    return Color{r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a};
  }

  inline Color operator/(const double s) const {
    return Color{r / s, g / s, b / s, a / s};
  }

  friend inline Color operator/(const double s, const Color rhs) {
    return Color{rhs.r / s, rhs.g / s, rhs.b / s, rhs.a / s};
  }

  inline Color operator*(const Color rhs) const {
    return Color{rhs.r * r, rhs.g * g, rhs.b * b, rhs.a * a};
  }

  inline Color operator*(const double s) const {
    return Color{s * r, s * g, s * b, s * a};
  }

  friend inline Color operator*(const double s, const Color rhs) {
    return Color{rhs.r * s, rhs.g * s, rhs.b * s, rhs.a * s};
  }

  void validate() {
    r = std::clamp(r,0.0,1.0);
    g = std::clamp(g,0.0,1.0);
    b = std::clamp(b,0.0,1.0);
    a = std::clamp(a,0.0,1.0);
  }
};

struct Pixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};
