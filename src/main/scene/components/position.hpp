#pragma once

#include "ecs/component.hpp"
#include "math/vec3.hpp"

class Position : public Component {
public:
    inline int id() override { return component_id<Position>(); }
    Vec3 pos;

    Position(Vec3 pos) {
      this->pos = pos;
    }
};
