#pragma once

#include "ecs/component.hpp"
#include "math/vec3.hpp"

class Camera : public Component {
public:
    inline int id() override { return component_id<Camera>(); }
    Vec3 direction;

    Camera(Vec3 direction) {
      this->direction = direction;
    }
};
