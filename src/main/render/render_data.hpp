#pragma once

#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"
#include <cstdint>

/**
 * Data associated with an object that is in the scene.
 */
struct RenderData {
  Position origin;
  Renderable appearance;
  uint32_t id;
};
