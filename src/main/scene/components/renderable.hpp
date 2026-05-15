#pragma once

#include "ecs/component.hpp"
#include "scene/geometry.hpp"
#include "scene/material.hpp"

struct Renderable : Component {
    int id = component_id<Renderable>();
    Material mat;
    Geometry geometry;
};
