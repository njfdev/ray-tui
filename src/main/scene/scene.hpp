#pragma once

#include "material.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "render/bvh.hpp"
#include "render/color.hpp"
#include "render/framebuffer.hpp"
#include "scene/light.hpp"
#include <functional>
#include <queue>
#include <vector>

struct BounceConfig {
  int reflect;
  int diffuse;
  int nDiffuse;
};

class Scene {
public:
  std::vector<PointLight> lights;
  Color background;
  double fovh;
  int cur_frame;

  void render(Framebuffer* fb, Ray camera);

  SceneObject* add_object(Shape shape, Material mat);
  void remove_object(SceneObject*);

  BVH bvh = BVH();
private:
  Intersection trace(Ray &ray, double min_dist);
  double traceLight(Vec3 p, Vec3 normal, PointLight light);
};
