#include "gameloop.hpp"
#include "math/vec3.hpp"
#include "render/color.hpp"
#include "ecs/entity_manager.hpp"
#include "scene/components/camera.hpp"
#include "scene/components/renderable.hpp"
#include "scene/components/position.hpp"
#include "scene/geometry.hpp"
#include "scene/systems/movement.hpp"
#include "scene/systems/ray_tracer.hpp"
#include <cmath>
#include <vector>

class PlaneCastTest : public GameLoop {
    EntityManager entityManager{};

    void init() override {
        // create object materials
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

        // camera
        entityManager.createEntity({
            new Position(Vec3{0, -10, 2}),
            new Camera(Vec3{0, 1, 0}.normalize())
        });

        entityManager.addSystem(new RayTracer(&fb));
        entityManager.addSystem(new CameraMovement(&input));
    }

  void update(double_t dt) override {
    entityManager.update(dt);
  }

  void cleanup() override {}
};

int main() {
  PlaneCastTest game{};

  game.run();
}
