#pragma once

#include "ecs/system.hpp"
#include "scene/scene.hpp"

/**
 * System to handling ray tracing of the 3D environment.
 */
class RayTracer : public System {
public:
  RayTracer(Framebuffer* fb);

private:
  Scene* scene;
  Framebuffer* fb;

  std::vector<int> requiredComponents() override;

  void init(EntityManager* entityManager) override;
  void update(double dt, EntityManager* entityManager, std::vector<int> entityIds) override;
  void cleanup(EntityManager* entityManager) override;
};
