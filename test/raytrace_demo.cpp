#include "gameloop.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "render/color.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"
#include <cmath>
#include <vector>

const double MOVEMENT_SPEED = 10.0;
const double ROTATION_SPEED = 5.0;

class PlaneCastTest : public GameLoop {
    Scene scene;
    Ray camera{Vec3{0.0, 1.0, 0.0}.normalize(), Vec3{0.0, 0.0, 0.0}};

    double t = 0.0;

    void init() override {
        Plane floor;
        floor.origin = Vec3{0, 0, 0};
        floor.normal = Vec3{0, 0, 1};

        Plane backWall;
        backWall.origin = Vec3{0, 10, 0};
        backWall.normal = Vec3{0, -1, 0};

        Plane leftWall;
        leftWall.origin = Vec3{-10, 0, 0};
        leftWall.normal = Vec3{1, 0, 0};

        Plane rightWall;
        rightWall.origin = Vec3{10, 0, 0};
        rightWall.normal = Vec3{-1, 0, 0};

        Sphere yellowSphere;
        yellowSphere.origin = Vec3{2.0, 6.0, 3.0};
        yellowSphere.radius = 2.0;

        Sphere purpleSphere;
        purpleSphere.origin = Vec3{-5.0, 3.0, 1.0};
        purpleSphere.radius = 1.5;

        Sphere greenSphere;
        greenSphere.origin = Vec3{5.0, -1.0, 1.5};
        greenSphere.radius = 0.75;

        Material red{Color{0.8, 0.0, 0.0}, 0.0, 1.0, 1.0, 0.0, 1.0};
        Material blue{Color{0.0, 0.0, 0.8}, 0.0, 1.0, 1.0, 0.0, 1.0};
        Material gray{Color{0.2, 0.2, 0.2}, 0.0, 1.0, 1.0, 0.0, 1.0};
        Material yellow{Color{0.8, 0.8, 0.0}, 0.0, 1.0, 1.0, 0.0, 1.0};
        Material purple{Color{0.5, 0.0, 0.5}, 0.0, 1.0, 1.0, 0.0, 1.0};
        Material green{Color{0.0, 0.8, 0.0}, 0.0, 1.0, 1.0, 0.0, 1.0};

        scene.add_object(floor, gray);
        scene.add_object(backWall, gray);
        scene.add_object(leftWall, blue);
        scene.add_object(rightWall, red);
        scene.add_object(yellowSphere, yellow);
        scene.add_object(purpleSphere, purple);
        scene.add_object(greenSphere, green);

        scene.lights.push_back(PointLight{Vec3{-2, 0, 7}, Color{1.0, 1.0, 1.0}});
        scene.lights.push_back(PointLight{Vec3{8.0, 0, 1}, Color{1.0, 0.0, 0.0}});
        scene.lights.push_back(PointLight{Vec3{-9.0, -5, 4}, Color{0.0, 0.0, 1.0}});

        scene.background = Color{0.08, 0.08, 0.12};
        scene.fovh = 1.2;
    }

  double angle = 3.14159/2;
  double x = 0.0;
  double y = -10.0;
  double z = 2.0;

  void update(double_t dt) override {
    scene.render(&fb, camera);

    double vx = 0.0;
    double vy = 0.0;
    double vz = 0.0;
    double w = 0.0;

    if (input.isKeyPressed(Key::W)) {
        vx += 1.0;
    }
    if (input.isKeyPressed(Key::S)) {
        vx -= 1.0;
    }
    if (input.isKeyPressed(Key::A)) {
        vy += 1.0;
    }
    if (input.isKeyPressed(Key::D)) {
        vy -= 1.0;
    }
    if (input.isKeyPressed(Key::R)) {
        vz += 1.0;
    }
    if (input.isKeyPressed(Key::F)) {
        vz -= 1.0;
    }
    if (input.isKeyPressed(Key::Q)) {
        w += 1.0;
    }
    if (input.isKeyPressed(Key::E)) {
        w -= 1.0;
    }

    x += (cos(angle)*vx - sin(angle)*vy) * MOVEMENT_SPEED * dt;
    y += (sin(angle)*vx + cos(angle)*vy) * MOVEMENT_SPEED * dt;
    z += vz * MOVEMENT_SPEED * dt;
    angle += w * ROTATION_SPEED * dt;

    camera.direction = Vec3{cos(angle), sin(angle), 0.0};
    camera.origin.x = x;
    camera.origin.y = y;
    camera.origin.z = z;

    t += dt;
  }

  void cleanup() override {}
};

int main() {
  PlaneCastTest game{};

  game.run();
}
