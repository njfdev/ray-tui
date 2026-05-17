#pragma once

#include "render/color.hpp"

struct Material {
  Color color;
  double specular = 0.0;
  double diffuse = 1.0;
  double opacity = 1.0;
  double refract = 0.0;
  double refractIdx = 1.0;
};
