#pragma once
#include "render/color.hpp"

struct Material {
  Color color;
  double specular;
  double diffuse;
  double opacity;
  double refract;
  double refractIdx;
};
