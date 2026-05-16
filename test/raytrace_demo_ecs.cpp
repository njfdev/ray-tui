#include "gameloop.hpp"
#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "render/color.hpp"
#include "ecs/entity_manager.hpp"
#include "scene/components/renderable.hpp"
#include "scene/components/position.hpp"
#include "scene/geometry.hpp"
#include "scene/scene.hpp"
#include <cmath>
#include <iostream>
#include <vector>

const double MOVEMENT_SPEED = 10.0;
const double ROTATION_SPEED = 5.0;

class PlaneCastTest : public GameLoop {
    EntityManager entityManager{};
    Scene scene{&entityManager};
    Ray camera{Vec3{0.0, 1.0, 0.0}.normalize(), Vec3{0.0, 0.0, 0.0}};

    double t = 0.0;

    void init() override {
        Material red{Color{0.8, 0.0, 0.0}};
        Material blue{Color{0.0, 0.0, 0.8}};
        Material gray{Color{0.2, 0.2, 0.2}};
        Material yellow{Color{0.8, 0.8, 0.0}};
        Material purple{Color{0.5, 0.0, 0.5}};
        Material green{Color{0.0, 0.8, 0.0}};
        Material white{Color{1.0, 1.0, 1.0}};

        // floor
        entityManager.createEntity({
            new Position(Vec3{0,0,0}),
            new Renderable(gray, Plane{ .normal = Vec3{0,0,1} })
        });

        // back wall
        entityManager.createEntity({
            new Position(Vec3{0,10,0}),
            new Renderable(gray, Plane{ .normal = Vec3{0,-1,0} })
        });

        // left wall
        entityManager.createEntity({
            new Position(Vec3{-10,0,0}),
            new Renderable(gray, Plane{ .normal = Vec3{1,0,0} })
        });

        // right wall
        entityManager.createEntity({
            new Position(Vec3{10,0,0}),
            new Renderable(gray, Plane{ .normal = Vec3{-1,0,0} })
        });

        // yellow sphere
        entityManager.createEntity({
            new Position(Vec3{2,6,3}),
            new Renderable(yellow, Sphere{ .radius = 2.0 })
        });

        // purple sphere
        entityManager.createEntity({
            new Position(Vec3{-5,3,1}),
            new Renderable(purple, Sphere{ .radius = 1.5 })
        });

        // green sphere
        entityManager.createEntity({
            new Position(Vec3{5,-1,1.5}),
            new Renderable(green, Sphere{ .radius = 0.75 })
        });

        // white light
        entityManager.createEntity({
            new Position(Vec3{-2,0,7}),
            new Renderable(white, EmptyGeometry {})
        });

        // red light
        entityManager.createEntity({
            new Position(Vec3{8,0,1}),
            new Renderable(red, EmptyGeometry {})
        });

        // blue light
        entityManager.createEntity({
            new Position(Vec3{-9,-5,4}),
            new Renderable(blue, EmptyGeometry {})
        });

        scene.background = Color{0.08, 0.08, 0.12};
        scene.fovh = 1.2;
        scene.construct();
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

    camera.direction = Vec3{static_cast<float>(cos(angle)), static_cast<float>(sin(angle)), 0.0};
    camera.origin.x = x;
    camera.origin.y = y;
    camera.origin.z = z;

    t += dt;
  }

  void cleanup() override {}
};

int main() {
  PlaneCastTest game{};
    game.disable_render = true;

  game.run();
}
