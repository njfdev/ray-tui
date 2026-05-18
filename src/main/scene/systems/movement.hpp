#pragma once

#include "ecs/system.hpp"
#include "input/input.hpp"

const double MOVEMENT_SPEED = 10.0;
const double ROTATION_SPEED = 5.0;

class CameraMovement : public System {
  public:
    CameraMovement(Input* input);

  private:
    Input* input;

    // variables to keep track of camera movement
    double angle = 3.14159/2;
    double x = 0.0;
    double y = -10.0;
    double z = 2.0;

    void processInputs(double dt);

    std::vector<int> requiredComponents() override;

    void init(EntityManager* entityManager) override;
    void update(double dt, EntityManager* entityManager, std::vector<int> entityIds) override;
    void cleanup(EntityManager* entityManager) override;
};
