#include "ray_tracer.hpp"
#include "ecs/component.hpp"
#include "ecs/entity_manager.hpp"
#include "render/framebuffer.hpp"
#include "scene/components/camera.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"

RayTracer::RayTracer(Framebuffer* fb) {
  // scene will be assigned later, since it needs access to EntityManager
  scene = nullptr;
  this->fb = fb;
}

std::vector<int> RayTracer::requiredComponents() {
  return { component_id<Position>(), component_id<Renderable>() };
}

void RayTracer::init(EntityManager* entityManager) {
  scene = new Scene(entityManager);

  // build the scene
  scene->background = Color{0.08, 0.08, 0.12};
  scene->fovh = 1.2;
  scene->construct();
}

void RayTracer::update(double dt, EntityManager* entityManager, std::vector<int> entityIds) {
  // get pointers to the first camera's components
  int cameraId = entityManager->getEntityIdsWithComponents({ component_id<Position>(), component_id<Camera>() })[0];
  Position* cameraOrigin = entityManager->getComponent<Position>(cameraId);
  Camera* cameraDirection = entityManager->getComponent<Camera>(cameraId);

  // update the framebuffer
  scene->render(fb, Ray(cameraDirection->direction, cameraOrigin->pos));
}

void RayTracer::cleanup(EntityManager* entityManager) {}
