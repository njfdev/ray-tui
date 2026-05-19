#pragma once

#include "ecs/entity_manager.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "render/bvh.hpp"
#include "render/color.hpp"
#include "render/framebuffer.hpp"

struct BounceConfig {
  int reflect;
  int diffuse;
  int nDiffuse;
};

class Scene {
public:
  EntityManager* entityManager;
  Color background;
  double fovh;
  int cur_frame;

  Scene(EntityManager* entityManager);

  void construct();

  void render(Framebuffer* fb, Ray camera);

  BVH bvh = BVH();
  BounceConfig bounceConfig = {2, 1, 0};

private:
  Intersection trace(Ray &ray, double min_dist);
  double traceLight(Vec3 p, Vec3 normal, Vec3 lightOrigin);
  Color castRay(Ray ray, int depth);
};
