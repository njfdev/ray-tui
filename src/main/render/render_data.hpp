#pragma once

#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"

/**
 * Data associated with an object that is in the scene.
 */
struct RenderData {
  Position origin;
  Renderable appearance;
};
