#pragma once

#include "ecs/system.hpp"

class Renderer : System {

  std::list<int> requiredComponents() override;

  void init() override;
  void update(std::list<int> entityIds) override;
  void cleanup() override;
};
