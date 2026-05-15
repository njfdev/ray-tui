#pragma once

#include "ecs/component.hpp"
#include "math/vec3.hpp"

struct Position : Component {
    Vec3 pos;
};
