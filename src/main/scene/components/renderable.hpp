#pragma once

#include "ecs/component.hpp"
#include "scene/geometry.hpp"
#include "scene/material.hpp"

struct Renderable : Component {
    Material mat;
    Geometry geometry;
};
