#pragma once

#include "component.hpp"
#include <vector>

/**
 * Stores all the components associated with one object/entity.
 */
class Entity {
public:
    std::vector<Component> components;
};
