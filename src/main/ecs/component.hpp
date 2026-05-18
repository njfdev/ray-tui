#pragma once

inline int component_type_index = 0;

/**
 * Generates a unique ID for each Component subclass.
 *
 * @return the component type's unique ID
 */
template<typename T>
inline int component_id() {
    static const int id = component_type_index++;
    return id;
}

/**
 * Superclass container to implement for a component
 * in the ECS system.
 */
class Component {
public:
  /**
   * Checks the unique ID of the type of component this
   * is.
   *
   * Must be implemented as: `{ return component_id<NameOfComponentSubClass>(); }`
   *
   * @return the component type's unique ID
   */
  virtual inline int id() = 0;
};
