#pragma once

#include "ecs/component.hpp"
#include "math/vec3.hpp"

struct Position : Component {
public:
    int id = component_id<Position>();
    Vec3 pos;
};
