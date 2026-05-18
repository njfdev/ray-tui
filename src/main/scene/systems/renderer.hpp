#pragma once

#include "ecs/system.hpp"
#include "scene/scene.hpp"

class Renderer : public System {
public:
  Renderer(Framebuffer* fb);

private:
  Scene* scene;
  Framebuffer* fb;

  std::vector<int> requiredComponents() override;

  void init(EntityManager* entityManager) override;
  void update(double dt, EntityManager* entityManager, std::vector<int> entityIds) override;
  void cleanup(EntityManager* entityManager) override;
};
