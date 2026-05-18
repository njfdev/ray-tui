#pragma once

#include "ecs/component.hpp"
#include "math/vec3.hpp"

/**
 * Component to store position within 3D space.
 */
class Position : public Component {
public:
    /**
     * Unique id of the position component type
     *
     * @returns Positions's unique type id
     */
    inline int id() override { return component_id<Position>(); }

    Vec3 pos;

    /**
     * Constructor of a Position component.
     *
     * @param pos origin of the entity
     * @return new Position component
     */
    Position(Vec3 pos) {
      this->pos = pos;
    }
};
