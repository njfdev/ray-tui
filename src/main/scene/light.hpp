#pragma once
#include "render/color.hpp"
#include "math/vec3.hpp"

struct PointLight {
  Vec3 p;
  Color color;
};
