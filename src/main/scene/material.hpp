#pragma once

#include "render/color.hpp"

/**
 * Data for the physical material appearance of an object
 */
struct Material {
  Color color;
  double specular = 0.0;
  double diffuse = 1.0;
  double opacity = 1.0;
  double refract = 0.0;
  double refractIdx = 1.0;
};
