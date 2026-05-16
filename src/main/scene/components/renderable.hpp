#pragma once

#include "ecs/component.hpp"
#include "scene/geometry.hpp"
#include "scene/material.hpp"

struct Renderable : public Component {
public:
    inline int id() override { return component_id<Renderable>(); }
    Material mat;
    Geometry geometry;

    Renderable(Material mat, Geometry geom) {
      this->mat = mat;
      this->geometry = geom;
    }
};
