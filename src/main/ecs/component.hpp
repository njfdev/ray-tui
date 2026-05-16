#pragma once

#include <type_traits>

static int component_type_index = 0;
template<typename T>
static int component_id() {
    static const int id = component_type_index++;
    return id;
}

class Component {
public:
  virtual inline int id() { return component_id<Component>(); }
};
