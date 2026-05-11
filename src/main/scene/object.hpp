#pragma once
#include "math/intersection.hpp"
#include "scene/material.hpp"

struct SceneObject {
  Material m;
  Shape* shape;
};
