#include "ecs/entity_manager.hpp"
#include "gameloop.hpp"
#include "math/vec3.hpp"
#include "render/color.hpp"
#include "scene/components/camera.hpp"
#include "scene/components/position.hpp"
#include "scene/components/renderable.hpp"
#include "scene/geometry.hpp"
#include "scene/material.hpp"
#include "scene/systems/movement.hpp"
#include "scene/systems/ray_tracer.hpp"

class Demo : public GameLoop {
  EntityManager em;

  void init() override {
    FRAMERATE = 30.0;

    em.createEntity({
        new Position({0.0f, -10.0f, 2.0f}),
        new Camera({0.0f, 1.0f, 0.0f}),
    });

    // ground plane
    em.createEntity({
        new Position({0.0f, 0.0f, -0.5f}),
        new Renderable(
            Material{
                .color = {0.55, 0.55, 0.55, 1.0},
                .specular = 0.25,
                .diffuse = 0.9,
                .opacity = 1.0,
            },
            Plane{{0.0f, 0.0f, 1.0f}, 18.0}),
    });

    // back wall (slighly blue)
    em.createEntity({
        new Position({0.0f, 12.0f, 0.0f}),
        new Renderable(
            Material{
                .color = {0.18, 0.22, 0.38, 1.0},
                .specular = 0.05,
                .diffuse = 1.0,
                .opacity = 1.0,
            },
            Plane{{0.0f, -1.0f, 0.0f}, 18.0}),
    });

    // mirror ball
    em.createEntity({
        new Position({0.0f, 3.0f, 1.0f}),
        new Renderable(
            Material{
                .color = {0.9, 0.9, 0.9, 1.0},
                .specular = 0.95,
                .diffuse = 0.05,
                .opacity = 1.0,
            },
            Sphere{1.1}),
    });

    // clear ball
    em.createEntity({
        new Position({-3.5f, 3.0f, 0.8f}),
        new Renderable(
            Material{
                .color = {0.95, 0.98, 1.0, 1.0},
                .specular = 0.0,
                .diffuse = 0.05,
                .opacity = 0.08,
                .refract = 0.92,
                .refractIdx = 1.52,
            },
            Sphere{1.0}),
    });

    // orange clear
    em.createEntity({
        new Position({3.5f, 3.0f, 0.8f}),
        new Renderable(
            Material{
                .color = {1.0, 0.75, 0.2, 1.0},
                .specular = 0.0,
                .diffuse = 0.1,
                .opacity = 0.18,
                .refract = 0.82,
                .refractIdx = 1.45,
            },
            Sphere{1.0}),
    });

    // gold metallic
    em.createEntity({
        new Position({-6.5f, 4.5f, 0.7f}),
        new Renderable(
            Material{
                .color = {1.0, 0.78, 0.12, 1.0},
                .specular = 0.72,
                .diffuse = 0.6,
                .opacity = 1.0,
            },
            Sphere{0.9}),
    });

    // copper metallic
    em.createEntity({
        new Position({6.5f, 4.5f, 0.7f}),
        new Renderable(
            Material{
                .color = {0.9, 0.42, 0.2, 1.0},
                .specular = 0.68,
                .diffuse = 0.55,
                .opacity = 1.0,
            },
            Sphere{0.9}),
    });

    // red, transparent
    em.createEntity({
        new Position({0.0f, 6.5f, 0.7f}),
        new Renderable(
            Material{
                .color = {0.95, 0.08, 0.08, 1.0},
                .specular = 0.0,
                .diffuse = 0.3,
                .opacity = 0.25,
                .refract = 0.75,
                .refractIdx = 1.65,
            },
            Sphere{0.85}),
    });

    // blue, transparent
    em.createEntity({
        new Position({-3.0f, 6.5f, 0.6f}),
        new Renderable(
            Material{
                .color = {0.08, 0.08, 0.95, 1.0},
                .specular = 0.0,
                .diffuse = 0.3,
                .opacity = 0.25,
                .refract = 0.75,
                .refractIdx = 1.65,
            },
            Sphere{0.75}),
    });

    // green, transparent
    em.createEntity({
        new Position({3.0f, 6.5f, 0.6f}),
        new Renderable(
            Material{
                .color = {0.08, 0.95, 0.08, 1.0},
                .specular = 0.0,
                .diffuse = 0.3,
                .opacity = 0.25,
                .refract = 0.75,
                .refractIdx = 1.65,
            },
            Sphere{0.75}),
    });

    // red solid ball
    em.createEntity({
        new Position({-2.0f, 1.2f, 0.4f}),
        new Renderable(
            Material{
                .color = {0.9, 0.15, 0.1, 1.0},
                .specular = 0.08,
                .diffuse = 1.0,
                .opacity = 1.0,
            },
            Sphere{0.5}),
    });

    // teal solid ball
    em.createEntity({
        new Position({2.0f, 1.2f, 0.4f}),
        new Renderable(
            Material{
                .color = {0.05, 0.8, 0.75, 1.0},
                .specular = 0.08,
                .diffuse = 1.0,
                .opacity = 1.0,
            },
            Sphere{0.5}),
    });

    // semi transparent white ball
    em.createEntity({
        new Position({0.0f, 1.5f, 0.45f}),
        new Renderable(
            Material{
                .color = {1.0, 1.0, 1.0, 1.0},
                .specular = 0.0,
                .diffuse = 0.8,
                .opacity = 0.45,
                .refract = 0.0,
            },
            Sphere{0.55}),
    });

    // warm white skylight
    em.createEntity({
        new Position({-6.0f, -2.0f, 9.0f}),
        new Renderable(Material{.color = {1.0, 0.92, 0.82, 1.0}},
                       EmptyGeometry{}),
    });

    // blue light
    em.createEntity({
        new Position({8.0f, 1.0f, 7.0f}),
        new Renderable(Material{.color = {0.05, 0.05, 1.0, 1.0}},
                       EmptyGeometry{}),
    });

    em.createEntity({
        new Position({0.0f, 14.0f, 5.0f}),
        new Renderable(Material{.color = {0.75, 0.5, 1.0, 1.0}},
                       EmptyGeometry{}),
    });


    em.createEntity({
        new Position({0.0f, 0.0f, 10.0f}),
        new Renderable(Material{.color = {1.0, 0.5, 0.75, 1.0}},
                       EmptyGeometry{}),
    });

    em.addSystem(new RayTracer(&fb));
    em.addSystem(new CameraMovement(&input));
  }

  void update(double dt) override { em.update(dt); }
  void cleanup() override {}
};

int main() {
  Demo demo;
  demo.run();
}
