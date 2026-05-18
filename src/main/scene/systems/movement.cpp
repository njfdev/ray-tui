#include "movement.hpp"
#include "ecs/entity_manager.hpp"
#include "scene/components/camera.hpp"
#include "scene/components/position.hpp"

CameraMovement::CameraMovement(Input* input) {
  this->input = input;
}

std::vector<int> CameraMovement::requiredComponents() {
  return { component_id<Position>(), component_id<Camera>() };
}

void CameraMovement::init(EntityManager* entityManager) {}

void CameraMovement::processInputs(double dt) {
  // movement velocities
  double vx = 0.0;
  double vy = 0.0;
  double vz = 0.0;
  double vw = 0.0;

  // check keyboard inputs and update corresponding velocity
  if (input->isKeyPressed(Key::W)) {
      vx += 1.0;
  }
  if (input->isKeyPressed(Key::S)) {
      vx -= 1.0;
  }
  if (input->isKeyPressed(Key::A)) {
      vy += 1.0;
  }
  if (input->isKeyPressed(Key::D)) {
      vy -= 1.0;
  }
  if (input->isKeyPressed(Key::R)) {
      vz += 1.0;
  }
  if (input->isKeyPressed(Key::F)) {
      vz -= 1.0;
  }
  if (input->isKeyPressed(Key::Q)) {
      vw += 1.0;
  }
  if (input->isKeyPressed(Key::E)) {
      vw -= 1.0;
  }

  // apply velocities and scale based on movement speeds
  x += (cos(angle)*vx - sin(angle)*vy) * MOVEMENT_SPEED * dt;
  y += (sin(angle)*vx + cos(angle)*vy) * MOVEMENT_SPEED * dt;
  z += vz * MOVEMENT_SPEED * dt;
  angle += vw * ROTATION_SPEED * dt;
}

void CameraMovement::update(double dt, EntityManager* entityManager, std::vector<int> entityIds) {
  processInputs(dt);

  Position* cameraOrigin = entityManager->getComponent<Position>(entityIds[0]);
  Camera* cameraDirection = entityManager->getComponent<Camera>(entityIds[0]);

  // update camera pose based on inputs
  cameraDirection->direction = Vec3{static_cast<float>(cos(angle)), static_cast<float>(sin(angle)), 0.0};
  cameraOrigin->pos = Vec3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
}

void CameraMovement::cleanup(EntityManager* entityManager) {}
