#pragma once

#include "material.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "render/color.hpp"
#include "render/framebuffer.hpp"
#include "scene/light.hpp"
#include <functional>
#include <queue>
#include <vector>

struct SceneObject {
  Material m;
  Shape* shape;
};

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

private:
  std::vector<Shape> shapes;
  std::vector<SceneObject> object_data;
  std::priority_queue<int, std::vector<int>, std::greater<int>> empty_slots;
  Intersection trace(Ray &ray, double min_dist);
  double traceLight(Vec3 p, Vec3 normal, PointLight light);
};
