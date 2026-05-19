#pragma once
#include <algorithm>
#include <cstdint>

/**
 * RGBA color with double-precision components in the range [0.0, 1.0].
 */
struct Color {
  double r; // red channel
  double g; // green channel
  double b; // blue channel
  double a; // alpha channel (currently unused)

  /** component-wise addition. */
  inline Color operator+(const Color &rhs) const {
    return Color{r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a};
  }

  /** component-wise subtraction. */
  inline Color operator-(const Color &rhs) const {
    return Color{r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a};
  }

  /** scalar division */
  inline Color operator/(const double s) const {
    return Color{r / s, g / s, b / s, a / s};
  }

  /** scalar division */
  friend inline Color operator/(const double s, const Color rhs) {
    return Color{rhs.r / s, rhs.g / s, rhs.b / s, rhs.a / s};
  }

  /** component-wise multiplication */
  inline Color operator*(const Color rhs) const {
    return Color{rhs.r * r, rhs.g * g, rhs.b * b, rhs.a * a};
  }

  /** component-wise multiplication  */
  inline Color operator*(const double s) const {
    return Color{s * r, s * g, s * b, s * a};
  }

  /** scalar multiplication */
  friend inline Color operator*(const double s, const Color rhs) {
    return Color{rhs.r * s, rhs.g * s, rhs.b * s, rhs.a * s};
  }

  /** clamps all components to be in the range [0.0, 1.0]. */
  void validate() {
    r = std::clamp(r, 0.0, 1.0);
    g = std::clamp(g, 0.0, 1.0);
    b = std::clamp(b, 0.0, 1.0);
    a = std::clamp(a, 0.0, 1.0);
  }
};

/**
 * 8-bit RGB pixel, used as the format written to the framebuffer.
 */
struct Pixel {
  uint8_t r; // red channel
  uint8_t g; // green channel
  uint8_t b; // blue channel
};
