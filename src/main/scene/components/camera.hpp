#pragma once

#include "ecs/component.hpp"
#include "math/vec3.hpp"

/**
 * Component to store the orientation of a camera.
 */
class Camera : public Component {
public:
    /**
     * Unique id of the camera component type
     *
     * @returns Camera's unique type id
     */
    inline int id() override { return component_id<Camera>(); }

    Vec3 direction;

    /**
     * Constructor of a Camera component.
     *
     * @param direction orientation of the camera
     * @return new Camera component
     */
    Camera(Vec3 direction) {
      this->direction = direction;
    }
};
