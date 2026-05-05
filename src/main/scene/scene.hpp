#pragma once

#include "material.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "render/color.hpp"
#include "render/framebuffer.hpp"
#include "scene/light.hpp"
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

private:
  std::vector<Shape> shapes;
  std::vector<SceneObject> object_data;
  Intersection trace(Ray &ray);
  double traceLight(Vec3 p, Vec3 normal, PointLight light);
};
