#pragma once

#include <type_traits>

class Component {};

static int component_type_index = 0;
template<typename T>
static int component_id() {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
    return component_type_index++;
}
