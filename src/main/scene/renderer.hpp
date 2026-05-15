#pragma once

#include "ecs/system.hpp"

class Renderer : System {

  std::vector<int> requiredComponents() override;

  void init() override;
  void update(std::vector<int> entityIds) override;
  void cleanup() override;
};
