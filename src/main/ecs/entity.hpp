#pragma once

#include "component.hpp"
#include <vector>

class Entity {
public:
    std::vector<Component> components;
};
