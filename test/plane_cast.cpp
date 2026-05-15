#include "gameloop.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "render/color.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"
#include <cmath>
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

    Material mat1{Color{0.7, 0, 0}, 0.0, 1.0, 1.0, 0.0, 1.0};
    Material mat2{Color{0, 0, 0.7}, 0.0, 1.0, 1.0, 0.0, 1.0};

    scene.add_object(plane, mat1);
    scene.add_object(sphere, mat2);

    scene.lights.push_back(PointLight{Vec3{0, 0, 10}, Color{1.0, 0.7, 1.0}});
    scene.background = Color{0.08, 0.08, 0.12};
    scene.fovh = 1.2;
  }

  void update(double_t dt) override {
    scene.render(&fb, camera);
    camera.direction = Vec3{(float) cos(t), (float) sin(t), 0.0};
    t += dt;
  }

  void cleanup() override {}
};

int main() {
  PlaneCastTest game{};

  game.run();
}
