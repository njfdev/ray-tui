#include "gameloop.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "render/color.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"
#include <cmath>
#include <iostream>
#include <vector>

class PlaneCastTest : public GameLoop {
  Scene scene;
  Ray camera{Vec3{1.0, 0.0, 0.0}.normalize(), Vec3{-5.0, 0.0, 1.0}};

  double t = 0.0;

  void init() override {
    Plane plane;
    Sphere sphere;
    plane.origin = Vec3{0, 0, 0};
    plane.normal = Vec3{0, 0, 1};

    sphere.origin = Vec3{0.0, 0.0, 0.0};
    sphere.radius = 0.4;

    Material mat1{Color{0.5, 0.5, 0.7}, 0.0, 1.0, 1.0, 0.0, 1.0};
    Material mat2{Color{0.7, 0.7, 0.7}, 0.0, 1.0, 1.0, 0.0, 1.0};

    scene.add_object(plane, mat1);
    scene.add_object(sphere, mat2);

    scene.add_object(Plane{Vec3{0.02, 0.0, 0.0}, Vec3{1.0, 0.0, 0.0}}, mat1);
    scene.add_object(Plane{Vec3{-0.02, 0.0, 0.0}, Vec3{-1.0, 0.0, 0.0}}, mat1);

    scene.lights.push_back(PointLight{Vec3{-10, 0, 5}, Color{1.0, 0.0, 0.0}});
    scene.lights.push_back(PointLight{Vec3{10, 0, 5}, Color{0.0, 0.0, 1.0}});
    scene.lights.push_back(PointLight{Vec3{0, 0, 10}, Color{1.0, 1.0, 1.0}});
    scene.background = Color{0.08, 0.08, 0.12};
    scene.fovh = 1.2;
  }

  double angle = 0.0;
  double x = 0.0;
  double y = 0.0;

  void update(double_t dt) override {
    scene.render(&fb, camera);

    double SPEED = 5.0;
    double ANG_SPEED = 1.2;

    double vx = 0.0;
    double vy = 0.0;
    double w = 0.0;

    if (input.isKeyPressed(KeyCode::W)) {
        vx += 1000.0*dt;
    }
    if (input.isKeyPressed(KeyCode::S)) {
        vx -= 1000.0*dt;
    }
    if (input.isKeyPressed(KeyCode::A)) {
        vy += 1000.0*dt;
    }
    if (input.isKeyPressed(KeyCode::D)) {
        vy -= 1000.0*dt;
    }
    if (input.isKeyPressed(KeyCode::Q)) {
        w += 1000.0*dt;
    }
    if (input.isKeyPressed(KeyCode::E)) {
        w -= 1000.0*dt;
    }

    x += (cos(angle)*vx - sin(angle)*vy) * SPEED * dt;
    y += (sin(angle)*vx + cos(angle)*vy) * SPEED * dt;
    angle += w * ANG_SPEED * dt;

    camera.direction = Vec3{cos(angle), sin(angle), 0.0};
    camera.origin.x = x;
    camera.origin.y = y;

    t += dt;
  }

  void cleanup() override {}
};

int main() {
  PlaneCastTest game{};

  game.run();
}
