#pragma once

#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"

struct RenderData {
  Position origin;
  Renderable appearance;
};
