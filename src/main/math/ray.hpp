#pragma once
#include "./vec3.hpp"

class Ray {
public:
  Vec3 origin;
  Vec3 direction;

  Ray(Vec3 direction) {
    this->direction = Vec3(direction);
    this->origin = Vec3();
  }

  Ray(Vec3 direction, Vec3 origin) {
    this->direction = Vec3(direction);
    this->origin = Vec3(origin);
  }
};
