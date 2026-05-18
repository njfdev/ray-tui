#pragma once

#include "ecs/component.hpp"
#include "scene/geometry.hpp"
#include "scene/material.hpp"

/**
 * Component to store physical renderable properties of an
 * entity within the 3D render space.
 */
struct Renderable : public Component {
public:
    /**
     * Unique id of the renderable component type
     *
     * @returns Renderable's unique type id
     */
    inline int id() override { return component_id<Renderable>(); }

    Material mat;
    Geometry geometry;

    /**
     * Constructor of a Renderable component.
     *
     * @param mat surface/material/physical appearance of the entity
     * @param geom space/geometry of the entity
     * @return new Renderable component
     */
    Renderable(Material mat, Geometry geom) {
      this->mat = mat;
      this->geometry = geom;
    }
};
