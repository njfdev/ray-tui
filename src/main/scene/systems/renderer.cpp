#include "renderer.hpp"
#include "ecs/component.hpp"
#include "ecs/entity_manager.hpp"
#include "render/framebuffer.hpp"
#include "scene/components/camera.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"

Renderer::Renderer(Framebuffer* fb) {
  scene = nullptr;
  this->fb = fb;
}

std::vector<int> Renderer::requiredComponents() {
  return { component_id<Position>(), component_id<Renderable>() };
}

void Renderer::init(EntityManager* entityManager) {
  scene = new Scene(entityManager);

  // build the scene
  scene->background = Color{0.08, 0.08, 0.12};
  scene->fovh = 1.2;
  scene->construct();
}

void Renderer::update(double dt, EntityManager* entityManager, std::vector<int> entityIds) {
  int cameraId = entityManager->getEntityIdsWithComponents({ component_id<Position>(), component_id<Camera>() })[0];
  Position* cameraOrigin = entityManager->getComponent<Position>(cameraId);
  Camera* cameraDirection = entityManager->getComponent<Camera>(cameraId);

  // update the framebuffer
  scene->render(fb, Ray(cameraDirection->direction, cameraOrigin->pos));//Ray{Vec3{0,1,0},Vec3{0,-10,2}});
}

void Renderer::cleanup(EntityManager* entityManager) {}
