#pragma once
#include "./vec3.hpp"

/**
 * A ray, defined by its origin and normalized direction.
 */
struct Ray {
  /**
   * origin of the ray
   */
  Vec3 origin;

  /**
   * direction of the ray
   */
  Vec3 direction;

  /**
   * constructs a ray centered at the origin
   *
   * @param direction the direction of the ray
   */
  Ray(Vec3 direction) {
    this->direction = Vec3(direction);
    this->origin = Vec3();
  }

  /**
   * constructs a ray, copying the vectors used
   *
   * @param direction the direction of the ray
   * @param direction the origin of the ray
   */
  Ray(Vec3 direction, Vec3 origin) {
    this->direction = Vec3(direction);
    this->origin = Vec3(origin);
  }
};
